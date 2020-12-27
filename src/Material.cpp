#include "Material.h"
#include "OrthonormalBasis.h"
#include "MathUtils.h"
#include "BSDF.h"

#include <cassert>

namespace pt {

Material::Material(const Vec3& baseColor, float roughness, float metalness,
        float transmission, float ior, const Vec3& emittance)
    : baseColor_(srgbToLinear(baseColor))
    , roughness_(roughness)
    , metalness_(metalness)
    , transmission_(transmission)
    , ior_(ior)
    , emittance_(emittance)
{
    alpha_ = max(0.001f, roughness * roughness);
}

Vec3 Material::evaluate(const Vec3& wi, const Vec3& wo) const {
    float eta = computeRelativeIOR(wo);
    Vec3 f0 = lerp(schlickF0FromRelativeIOR(eta), baseColor_, metalness_);

    Vec3 diffuse = diffuse_Lambert(wi, wo, baseColor_);
    Vec3 specular = microfacetReflection_GGX(wi, wo, f0, eta, alpha_);
    Vec3 transmission = baseColor_ * microfacetTransmission_GGX(wi, wo, f0, eta, alpha_);
    
    float diffuseWeight = (1.0f - metalness_) * (1.0f - transmission_);
    float transmissionWeight = (1.0f - metalness_) * transmission_;

    return diffuseWeight * diffuse + specular + transmissionWeight * transmission;
}

Vec3 Material::sampleDirection(const Vec3& wo, float u1, float u2, float* pdf) const {
    float eta = computeRelativeIOR(wo);
    float pDiffuse, pSpecular, pTransmission;
    computeLobeProbabilities(pDiffuse, pSpecular, pTransmission);

    Vec3 wi;
    if (u1 < pDiffuse) {
        u1 = remap(u1,
            0.0f, pDiffuse - std::numeric_limits<float>::epsilon(),
            0.0f, oneMinusEpsilon<float>);
        assert(u1 >= 0.0f && u1 < 1.0f);

        wi = sign(cosTheta(wo)) * sampleCosineHemisphere(u1, u2);
        assert(isNormalized(wi));
    }
    else if (u1 < pDiffuse + pSpecular) {
        u1 = remap(u1,
            pDiffuse, pDiffuse + pSpecular - std::numeric_limits<float>::epsilon(),
            0.0f, oneMinusEpsilon<float>);
        assert(u1 >= 0.0f && u1 < 1.0f);

        Vec3 wo_upper = sign(cosTheta(wo)) * wo; // sign(+wo) * +wo = +wo, sign(-wo) * -wo = +wo
        Vec3 wh = sign(cosTheta(wo)) * sampleGGX_VNDF(wo_upper, alpha_, u1, u2);
        if (dot(wo, wh) < 0.0f) {
            return Vec3(0.0f);
        }

        wi = reflect(wo, wh);
        if (!sameHemisphere(wi, wo)) {
            return Vec3(0.0f);
        }
    }
    else {
        u1 = remap(u1,
            pDiffuse + pSpecular, oneMinusEpsilon<float>,
            0.0f, oneMinusEpsilon<float>);
        assert(u1 >= 0.0f && u1 < 1.0f);

        Vec3 wo_upper = sign(cosTheta(wo)) * wo; // sign(+wo) * +wo = +wo, sign(-wo) * -wo = +wo
        Vec3 wh = sign(cosTheta(wo)) * sampleGGX_VNDF(wo_upper, alpha_, u1, u2);
        if (dot(wo, wh) < 0.0f) {
            return Vec3(0.0f);
        }

        if (!refract(wo, wh, eta, wi)) {
            return Vec3(0.0f);
        }

        if (sameHemisphere(wi, wo)) {
            return Vec3(0.0f);
        }

        bool sameSide = dot(wo, wh) * dot(wi, wh) > 0.0f;
        if (sameSide) {
            return Vec3(0.0f);
        }
    }

    if (pdf) {
        *pdf = pDiffuse * pdfCosineHemisphere(wi, wo)
            + pSpecular * pdfGGX_VNDF_reflection(wi, wo, alpha_)
            + pTransmission * pdfGGX_VNDF_transmission(wi, wo, eta, alpha_);
    }

    return wi;
}

float Material::pdf(const Vec3& wi, const Vec3& wo) const {
    float eta = computeRelativeIOR(wo);

    float pDiffuse, pSpecular, pTransmission;
    computeLobeProbabilities(pDiffuse, pSpecular, pTransmission);

    return pDiffuse * pdfCosineHemisphere(wi, wo)
        + pSpecular * pdfGGX_VNDF_reflection(wi, wo, alpha_)
        + pTransmission * pdfGGX_VNDF_transmission(wi, wo, eta, alpha_);
}

float Material::computeRelativeIOR(const Vec3& wo) const {
    bool entering = cosTheta(wo) > 0.0f;
    return entering ? 1.0f / ior_ : ior_;
}

void Material::computeLobeProbabilities(float& pDiffuse, float& pSpecular, float& pTransmission) const {
    float dielectricBrdfWeight = (1.0f - metalness_) * (1.0f - transmission_);
    float specularBsdfWeight = (1.0f - metalness_) * transmission_;
    float metallicBrdfWeight = metalness_;
    
    pDiffuse = dielectricBrdfWeight;
    pSpecular = dielectricBrdfWeight + metallicBrdfWeight + specularBsdfWeight;
    pTransmission = specularBsdfWeight;
    
    float normFactor = 1.0f / (pDiffuse + pSpecular + pTransmission);
    pDiffuse *= normFactor;
    pSpecular *= normFactor;
    pTransmission *= normFactor;
}

} // namespace pt

#include "Material.h"
#include "OrthonormalBasis.h"
#include "MathUtils.h"
#include "BSDF.h"

#include <cassert>

namespace pt {

Material::Material(const Vec3& baseColor, float specular, float roughness,
        float metalness, float transmission, float ior, const Vec3& emittance)
    : baseColor_(baseColor)
    , specular_(specular)
    , roughness_(roughness)
    , metalness_(metalness)
    , transmission_(transmission)
    , ior_(ior)
    , emittance_(emittance)
{
    alpha_ = max(0.001f, roughness * roughness);
    kD_ = baseColor * (1.0f - metalness);
    kS_ = lerp(Vec3(specular * 0.08f), baseColor, metalness);
}

Vec3 Material::evaluate(const Vec3& wi, const Vec3& wo, const Vec3& normal) const {
    bool entering = cosTheta(wo) > 0.0f;
    float eta = entering ? 1.0f / ior_ : ior_;
    return diffuse_Lambert(kD_) + microfacetReflection_GGX(wi, wo, kS_, alpha_)
        + baseColor_ * microfacetTransmission_GGX(wi, wo, eta, alpha_);
}

Vec3 Material::sampleDirection(const Vec3& wo, const Vec3& normal,
        float u1, float u2, float* pdf) const {
    if (transmission_ < 0.5f) {
        float maxD = maxComponent(kD_);
        float maxS = maxComponent(Fr_Schlick(dot(normal, wo), kS_));
        float Pd = maxD / (maxD + maxS);
        float Ps = maxS / (maxD + maxS);

        Vec3 wi;
        if (u1 < Pd) {
            // Reuse random variable
            u1 = remap(u1,
                0.0f, Pd - std::numeric_limits<float>::epsilon(),
                0.0f, oneMinusEpsilon<float>);

            wi = sampleCosineHemisphere(u1, u2);
            assert(isNormalized(wi));
        }
        else {
            // Reuse random variable
            u1 = remap(u1,
                Pd, oneMinusEpsilon<float>,
                0.0f, oneMinusEpsilon<float>);

            Vec3 wh = sampleGGX_VNDF(wo, alpha_, u1, u2);
            if (pt::dot(wo, wh) < 0.0f) {
                return pt::Vec3(0.0f);
            }

            wi = normalize(reflect(wo, wh));
            assert(isNormalized(wh));
            assert(isNormalized(wi));
            assert(dot(wh, wo) >= 0.0f);
        }

        if (pdf) {
            *pdf = Pd * pdfCosineHemisphere(wi, wo) + Ps * pdfGGX_VNDF_reflection(wi, wo, alpha_);
        }

        return wi;
    }
    else {
        bool entering = cosTheta(wo) > 0.0f;
        float eta = entering ? 1.0f / ior_ : ior_;
        Vec3 wh = sampleGGX_VNDF(entering ? wo : -wo, alpha_, u1, u2);

        if (!pt::sameHemisphere(wo, wh)) {
            wh = -wh;
        }
        if (pt::dot(wo, wh) < 0.0f) {
            return pt::Vec3(0.0f);
        }

        Vec3 wi;
        if (!refract(wo, wh, eta, wi)) {
            wi = normalize(reflect(wo, wh));

            if (!sameHemisphere(wi, wo)) {
                return Vec3(0.0f);
            }
        }
        else {
            wi = normalize(wi);
            if (sameHemisphere(wi, wo)) {
                return Vec3(0.0f);
            }
            if (dot(wo, wh) * dot(wi, wh) > 0.0f) {
                return pt::Vec3(0.0f);
            }
            if (cosTheta(wi) == 0.0f || cosTheta(wo) == 0.0f) {
                return pt::Vec3(0.0f);
            }
        }

        if (pdf) {
            // One term will always be 0
            float pdf_refl = pdfGGX_VNDF_reflection(wi, wo, alpha_);
            float pdf_tran = pdfGGX_VNDF_transmission(wi, wo, eta, alpha_);
            *pdf = pdf_refl + pdf_tran;
        }

        return wi;
    }
}

float Material::pdf(const Vec3& wi, const Vec3& wo, const Vec3& normal) const {
    if (transmission_ < 0.5f) {
        float maxD = maxComponent(kD_);
        float maxS = maxComponent(Fr_Schlick(dot(normal, wo), kS_));
        float Pd = maxD / (maxD + maxS);
        float Ps = maxS / (maxD + maxS);

        return Pd * pdfCosineHemisphere(wi, wo) + Ps * pdfGGX_VNDF_reflection(wi, wo, alpha_);
    }
    else {
        bool entering = cosTheta(wo) > 0.0f;
        float eta = entering ? 1.0f / ior_ : ior_;
        // One term will always be 0
        return pdfGGX_VNDF_reflection(wi, wo, alpha_) + pdfGGX_VNDF_transmission(wi, wo, eta, alpha_);
    }
}

} // namespace pt

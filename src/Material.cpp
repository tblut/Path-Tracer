#include "Material.h"
#include "OrthonormalBasis.h"
#include "MathUtils.h"
#include "BSDF.h"

#include <cassert>

namespace pt {

Material::Material(const Vec3& baseColor, float roughness,
        float metalness, const Vec3& emittance)
    : baseColor_(baseColor)
    , roughness_(roughness)
    , metalness_(metalness)
    , emittance_(emittance)
{
    alpha_ = max(0.001f, roughness * roughness);
    kD_ = baseColor * (1.0f - metalness);
    kS_ = lerp(Vec3(0.04f), baseColor, metalness);
}

Vec3 Material::evaluate(const Vec3& wi, const Vec3& wo, const Vec3& normal) const {
    return diffuse_Lambert(kD_) + specular_GGX(wo, wi, kS_, alpha_);
}

Vec3 Material::sampleDirection(const Vec3& wo, const Vec3& normal,
        float u1, float u2, float* pdf) const {
    float maxD = maxComponent(kD_);
    float maxS = maxComponent(Fr_Schlick(normal, wo, kS_));
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

        Vec3 wh = sampleGGXVNDF(wo, alpha_, u1, u2);
        wi = reflect(wo, wh);
        assert(isNormalized(wh));
        assert(isNormalized(wi));
        assert(dot(wh, wo) >= 0.0f);
    }

    if (pdf) {
        *pdf = Pd * pdfCosineHemisphere(wi, wo) + Ps * pdfGGXVNDF(wi, wo, alpha_);
    }

    return wi;
}

float Material::pdf(const Vec3& wi, const Vec3& wo, const Vec3& normal) const {
    float maxD = maxComponent(kD_);
    float maxS = maxComponent(Fr_Schlick(normal, wo, kS_));
    float Pd = maxD / (maxD + maxS);
    float Ps = maxS / (maxD + maxS);

    return Pd * pdfCosineHemisphere(wi, wo) + Ps * pdfGGXVNDF(wi, wo, alpha_);
}

} // namespace pt

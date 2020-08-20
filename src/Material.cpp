#include "Material.h"
#include "OrthonormalBasis.h"
#include "MathUtils.h"

#include <cassert>

namespace {

using namespace pt;

Vec3 Fr_Schlick(const Vec3& wh, const Vec3& wi, const Vec3& f0) {
    return f0 + (Vec3(1.0f) - f0) * std::pow(1.0f - dot(wh, wi), 5);
}

float D_GGX(const Vec3& wh, const Vec3& normal, float alpha) {
    const float dotNH = dot(wh, normal);
    if (dotNH <= 0.0f) {
        return 0.0f;
    }

    const float alpha2 = alpha * alpha;
    const float a = 1.0f + dotNH * dotNH * (alpha2 - 1.0f);
    return alpha2 / (pi<float> * a * a);
}

float G1_Smith_GGX(const Vec3& w, const Vec3& wh, const Vec3& normal, float alpha) {
    if (dot(w, wh) <= 0.0f) {
        return 0.0f;
    }

    const float alpha2 = alpha * alpha;
    const float dotNW = dot(w, normal);
    const float dotNW2 = dotNW * dotNW;
    const float lambda = (std::sqrt(1.0f + alpha2 * (1.0f - dotNW2) / dotNW2) - 1.0f) / 2.0f;
    return 1.0f / (1.0f + lambda);
}

float G2_SmithUncorrelated_GGX(const Vec3& wi, const Vec3& wo, const Vec3& wh, const Vec3& normal, float alpha) {
    return G1_Smith_GGX(wi, wh, normal, alpha) * G1_Smith_GGX(wo, wh, normal, alpha);
}

float G2_SmithHeightCorrelated_GGX(const Vec3& wi, const Vec3& wo, const Vec3& wh, const Vec3& normal, float alpha) {
    if (dot(wi, wh) <= 0.0f || dot(wo, wh) <= 0.0f) {
        return 0.0f;
    }

    const float dotNO = dot(normal, wo);
    const float dotNI = dot(normal, wi);
    const float dotNO2 = dotNO * dotNO;
    const float dotNI2 = dotNI * dotNI;
    assert(dotNO2 > 0.0f);
    assert(dotNI2 > 0.0f);

    const float alpha2 = alpha * alpha;
    const float lambda_wo = (-1.0f + std::sqrt(alpha2 * (1.0f - dotNO2) / dotNO2 + 1.0f)) / 2.0f;
    const float lambda_wi = (-1.0f + std::sqrt(alpha2 * (1.0f - dotNI2) / dotNI2 + 1.0f)) / 2.0f;
    return 1.0f / (1.0f + lambda_wo + lambda_wi);
}

float G2_None(const Vec3& wi, const Vec3& wo, const Vec3& wh, const Vec3& normal, float alpha) {
    return 1.0f;
}

Vec3 diffuse_Lambert(const Vec3& diffuseColor) {
    return diffuseColor / pi<float>;
}

Vec3 specular_GGX(const Vec3& normal, const Vec3& wo, const Vec3& wi, const Vec3& f0, float alpha) {
    const Vec3 wh = normalize(wo + wi);
    const Vec3 F = Fr_Schlick(wh, wi, f0);
    const float dotNO = dot(normal, wo);
    const float dotNI = dot(normal, wi);
    const float dotNH = dot(normal, wh);

    if (dotNO <= 0.0f || dotNI <= 0.0f || dotNH <= 0.0f) {
        return Vec3(0.0f);
    }
    assert(dotNO > 0.0f);
    assert(dotNI > 0.0f);
    assert(dotNH > 0.0f);

    const float G = G2_SmithUncorrelated_GGX(wi, wo, wh, normal, alpha);
    const float D = D_GGX(wh, normal, alpha);
    return F * G * D / (4.0f * dotNO * dotNI);
}

Vec3 sampleCosineHemisphere(float u1, float u2) {
    const float r = std::sqrt(u1);
    const float phi = 2.0f * pi<float> * u2;
    return Vec3(r * std::sin(phi), r * std::cos(phi), std::sqrt(1.0f - u1));
}

float pdfCosineHemisphere(const Vec3& normal, const Vec3& wi) {
    return dot(normal, wi) / pi<float>;
}

Vec3 sampleGGX(float alpha, float u1, float u2) {
    float phi = 2.0f * pi<float> * u1;
    float theta = std::acos(std::sqrt((1.0f - u2) / ((alpha * alpha - 1.0f) * u2 + 1.0f)));

    float x = std::sin(theta) * std::cos(phi);
    float y = std::sin(theta) * std::sin(phi);
    float z = std::cos(theta);

    return Vec3(x, y, z);
}

float pdfGGX(const Vec3& normal, const Vec3& wh, const Vec3& wo, float alpha) {
    return D_GGX(wh, normal, alpha) * dot(wh, normal) / (4 * dot(wo, wh));
}

} // namespace

namespace pt {

Vec3 Material::evaluate(const Vec3& wi, const Vec3& wo, const Vec3& normal) const {
    float alpha = roughness * roughness;
    Vec3 kD = baseColor * (1.0f - metalness);
    Vec3 kS = lerp(Vec3(0.04f), baseColor, metalness);
    return diffuse_Lambert(kD) + specular_GGX(normal, wo, wi, kS, alpha);
}

Vec3 Material::sampleDirection(const Vec3& wo, const Vec3& normal,
        float u1, float u2, float* pdf) const {
    float alpha = roughness * roughness;
    Vec3 kD = baseColor * (1.0f - metalness);
    Vec3 kS = lerp(Vec3(0.04f), baseColor, metalness);

    float maxD = maxComponent(kD);
    float maxS = maxComponent(Fr_Schlick(normal, wo, kS));
    float Pd = maxD / (maxD + maxS);
    float Ps = maxS / (maxD + maxS);

    Vec3 wi, wh;
    OrthonormalBasis basis(normal);
    if (u1 < Pd) {
        // Reuse random variable
        u1 = remap(u1,
            0.0f, Pd - std::numeric_limits<float>::epsilon(),
            0.0f, oneMinusEpsilon<float>);

        wi = sampleCosineHemisphere(u1, u2);
        wi = basis.localToWorld(wi);
        wh = normalize(wi + wo);
    }
    else {
        // Reuse random variable
        u1 = remap(u1,
            Pd, oneMinusEpsilon<float>,
            0.0f, oneMinusEpsilon<float>);

        wh = sampleGGX(alpha, u1, u2);
        assert(length(wh) - 1.0f < 1e-6f);
        wh = basis.localToWorld(wh);
        wi = normalize(reflect(wo, wh));
    }

    if (pdf) {
        *pdf = Pd * pdfCosineHemisphere(normal, wi) + Ps * pdfGGX(normal, wh, wo, alpha);
    }

    return wi;
}

float Material::pdf(const Vec3& wi, const Vec3& wo, const Vec3& normal) const {
    float alpha = roughness * roughness;
    Vec3 kD = baseColor * (1.0f - metalness);
    Vec3 kS = lerp(Vec3(0.04f), baseColor, metalness);

    float maxD = maxComponent(kD);
    float maxS = maxComponent(Fr_Schlick(normal, wo, kS));
    float Pd = maxD / (maxD + maxS);
    float Ps = maxS / (maxD + maxS);
    Vec3 wh = normalize(wi + wo);

    return Pd * pdfCosineHemisphere(normal, wi) + Ps * pdfGGX(normal, wh, wo, alpha);
}

} // namespace pt

#pragma once

#include "MathUtils.h"
#include "OrthonormalBasis.h"

// NOTE: Many of these functions are not optimized at all for clarity and debugging
namespace pt {

// All shading is done in local space where X=(1,0,0), Y=(0,1,0), Z=(0,0,1)
// and the shading normal N=Z=(0,0,1). Thus, cosTheta = dot(n,w) = w.z
inline float cosTheta(const Vec3& w) {
    return w.z;
}

inline float cosTheta2(const Vec3& w) {
    return w.z * w.z;
}

inline bool sameHemisphere(const Vec3& w, const Vec3& wp) {
    return w.z * wp.z > 0.0f;
}


inline Vec3 Fr_Schlick(float cosThetaI, const Vec3& f0) {
    float a = 1.0f - cosThetaI;
    float a2 = a * a;
    float a5 = a2 * a2 * a;
    return f0 + (Vec3(1.0f) - f0) * a5;
}

inline float Fr_Dielectric(float cosThetaI, float eta) {
    float cosTheta2_T = 1.0f - (1.0f - cosThetaI * cosThetaI) / (eta * eta);
    if (cosTheta2_T <= 0.0f) {
        return 1.0f; // Total internal reflection
    }

    float cosThetaT = std::sqrt(cosTheta2_T);
    float r_parallel = (cosThetaI - eta * cosThetaT) / (cosThetaI + eta * cosThetaT);
    float r_perpendicular = (cosThetaT - eta * cosThetaI) / (cosThetaT + eta * cosThetaI);
    return 0.5f * (r_parallel * r_parallel + r_perpendicular * r_perpendicular);
}

inline float D_GGX(const Vec3& wh, float alpha) {
    float cosThetaH = cosTheta(wh);
    if (cosThetaH <= 0.0f) {
        return 0.0f;
    }

    float alpha2 = alpha * alpha;
    float a = 1.0f + cosThetaH * cosThetaH * (alpha2 - 1.0f);
    return alpha2 / (pi<float> * a * a);
}

inline float G1_Smith_GGX(const Vec3& w, const Vec3& wh, float alpha) {
    if (dot(w, wh) <= 0.0f) {
        return 0.0f;
    }

    float alpha2 = alpha * alpha;
    float cos2ThetaW = cosTheta2(w);
    float lambda = (-1.0f + std::sqrt(alpha2 * (1.0f - cos2ThetaW) / cos2ThetaW + 1.0f)) / 2.0f;
    return 1.0f / (1.0f + lambda);
}

inline float G2_SmithUncorrelated_GGX(const Vec3& wi, const Vec3& wo, const Vec3& wh, float alpha) {
    return G1_Smith_GGX(wi, wh, alpha) * G1_Smith_GGX(wo, wh, alpha);
}

inline float G2_SmithHeightCorrelated_GGX(const Vec3& wi, const Vec3& wo, const Vec3& wh, float alpha) {
    if (dot(wi, wh) <= 0.0f || dot(wo, wh) <= 0.0f) {
        return 0.0f;
    }

    float cos2ThetaO = cosTheta2(wo);
    float cos2ThetaI = cosTheta2(wi);
    assert(cos2ThetaO > 0.0f);
    assert(cos2ThetaI > 0.0f);

    float alpha2 = alpha * alpha;
    float lambda_wo = (-1.0f + std::sqrt(alpha2 * (1.0f - cos2ThetaO) / cos2ThetaO + 1.0f)) / 2.0f;
    float lambda_wi = (-1.0f + std::sqrt(alpha2 * (1.0f - cos2ThetaI) / cos2ThetaI + 1.0f)) / 2.0f;
    return 1.0f / (1.0f + lambda_wo + lambda_wi);
}

inline float G2_None(const Vec3& wi, const Vec3& wo, const Vec3& wh, float alpha) {
    return 1.0f;
}


// BxDF functions
inline Vec3 diffuse_Lambert(const Vec3& diffuseColor) {
    return diffuseColor / pi<float>;
}

inline Vec3 microfacetReflection_GGX(const Vec3& wi, const Vec3& wo, const Vec3& f0, float alpha) {
    if (!sameHemisphere(wi, wo)) {
        return Vec3(0.0f);
    }

    Vec3 wh = normalize(wi + wo);
    float cosThetaO = cosTheta(wo);
    float cosThetaI = cosTheta(wi);
    float cosThetaH = cosTheta(wh);

    if (cosThetaO <= 0.0f || cosThetaI <= 0.0f || cosThetaH <= 0.0f) {
        return Vec3(0.0f);
    }
    assert(cosThetaO > 0.0f);
    assert(cosThetaI > 0.0f);
    assert(cosThetaH > 0.0f);

    Vec3 F = Fr_Schlick(dot(wh, wi), f0);
    float G = G2_SmithHeightCorrelated_GGX(wi, wo, wh, alpha);
    float D = D_GGX(wh, alpha);
    return F * G * D / (4.0f * cosThetaO * cosThetaI);
}

inline float microfacetTransmission_GGX(const Vec3& wi, const Vec3& wo, float eta, float alpha) {
    if (sameHemisphere(wi, wo)) {
        return 0.0f;
    }

    Vec3 wh = normalize(eta * wi + wo);
    float F = Fr_Dielectric(dot(wh, wi), eta);
    float G = G2_SmithHeightCorrelated_GGX(wi, wo, wh, alpha);
    float D = D_GGX(wh, alpha);

    float denomSqrt = eta * dot(wi, wh) + dot(wo, wh);
    return D * G * (1.0f - F) * abs(dot(wi, wh)) * abs(dot(wo, wh))
        / (denomSqrt * denomSqrt * cosTheta(wo) * cosTheta(wi));
}


inline Vec3 sampleUniformSphere(float u1, float u2) {
    float cosTheta = 1.0f - 2.0f * u1;
    float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
    float phi = 2.0f * pi<float> * u2;
    return Vec3(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
}

inline float pdfUniformSphere(const Vec3& wi, const Vec3& wo) {
    return 1.0f / (4.0f * pi<float>);
}


inline Vec3 sampleCosineHemisphere(float u1, float u2) {
    float cosTheta = std::sqrt(1.0f - u1);
    float sinTheta = std::sqrt(u1);
    float phi = 2.0f * pi<float> * u2;
    return Vec3(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
}

inline float pdfCosineHemisphere(const Vec3& wi, const Vec3& wo) {
    return sameHemisphere(wi, wo) ? cosTheta(wi) / pi<float> : 0.0f;
}


inline Vec3 sampleGGX(float alpha, float u1, float u2) {
    float phi = 2.0f * pi<float> * u1;
    float cosTheta2 = (1.0f - u2) / ((alpha * alpha - 1.0f) * u2 + 1.0f);
    float sinTheta = std::sqrt(1.0f - cosTheta2);
    Vec3 wh(sinTheta * std::cos(phi), sinTheta * std::sin(phi), std::sqrt(cosTheta2));
    return wh;
}

inline float pdfGGX_reflection(const Vec3& wi, const Vec3& wo, float alpha) {
    if (!sameHemisphere(wi, wo)) {
        return 0.0f;
    }

    Vec3 wh = normalize(wi + wo);
    float pdf_h = D_GGX(wh, alpha) * abs(cosTheta(wh));
    float dwh_dwi = 1.0f / (4.0f * dot(wi, wh));
    return pdf_h * dwh_dwi;
}

inline float pdfGGX_transmission(const Vec3& wi, const Vec3& wo, float etaI, float etaO, float alpha) {
    if (sameHemisphere(wi, wo)) {
        return 0.0f;
    }

    Vec3 wh = normalize(etaI * wi + etaO * wo);
    float cosThetaH = cosTheta(wh);
    float cosThetaO = cosTheta(wo);
    float cosThetaI = cosTheta(wi);
    float dotOH = dot(wo, wh);
    float dotIH = dot(wi, wh);
    if (dot(wo, wh) * dot(wi, wh) > 0.0f) {
        return 0.0f;
    }

    float pdf_h = D_GGX(wh, alpha) * abs(cosTheta(wh));
    float sqrtDenom = etaI * dot(wi, wh) + etaO * dot(wo, wh);
    float dwh_dwi = etaO * etaO * abs(dot(wo, wh)) / (sqrtDenom * sqrtDenom);
    return pdf_h * dwh_dwi;
}


// See: http://jcgt.org/published/0007/04/01/paper.pdf
inline Vec3 sampleGGX_VNDF(const Vec3& wo, float alpha, float u1, float u2) {
    // Transform view direction to hemisphere configuration
    Vec3 woHemi = normalize(Vec3(alpha * wo.x, alpha * wo.y, wo.z));

    // Create orthonormal basis
    float length2 = woHemi.x * woHemi.x + woHemi.y * woHemi.y;
    Vec3 b1 = length2 > 0.0f
        ? Vec3(-woHemi.y, woHemi.x, 0.0f) * (1.0f / std::sqrt(length2))
        : Vec3(1.0f, 0.0f, 0.0f);
    Vec3 b2 = cross(woHemi, b1);

    // Parameterization of projected area
    float r = std::sqrt(u1);
    float phi = 2.0f * pi<float> * u2;
    float t1 = r * std::cos(phi);
    float t2 = r * std::sin(phi);
    float s = 0.5f * (1.0f + woHemi.z);
    t2 = (1.0f - s) * std::sqrt(1.0f - t1 * t1) + s * t2;

    // Reprojection onto hemisphere
    Vec3 whHemi = t1 * b1 + t2 * b2 + std::sqrt(max(0.0f, 1.0f - t1 * t1 - t2 * t2)) * woHemi;

    // Transforming half vector back to ellipsoid configuration
    return normalize(Vec3(alpha * whHemi.x, alpha * whHemi.y, max(0.0f, whHemi.z)));
}

inline float pdfGGX_VNDF_reflection(const Vec3& wi, const Vec3& wo, float alpha) {
    if (!sameHemisphere(wi, wo)) {
        return 0.0f;
    }

    Vec3 wh = normalize(wi + wo);
    float pdf_h = G1_Smith_GGX(wo, wh, alpha) * D_GGX(wh, alpha) * dot(wh, wo) / abs(cosTheta(wo));
    float dwh_dwi = 4.0f * dot(wh, wi);
    return pdf_h / dwh_dwi;
}

inline float pdfGGX_VNDF_transmission(const Vec3& wi, const Vec3& wo, float eta, float alpha) {
    if (sameHemisphere(wi, wo)) {
        return 0.0f;
    }

    Vec3 wh = normalize(eta * wi + wo);
    float pdf_h = G1_Smith_GGX(wo, wh, alpha) * D_GGX(wh, alpha) * dot(wh, wo) / abs(cosTheta(wo));
    float sqrtDenom = eta * dot(wi, wh) + dot(wo, wh);
    float dwh_dwi = eta * eta * dot(wi, wh) / (sqrtDenom * sqrtDenom);
    return pdf_h / dwh_dwi;
}

} // namespace pt

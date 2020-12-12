#pragma once

#include "MathUtils.h"
#include "OrthonormalBasis.h"

namespace pt {

// All shading is done in local space where: X=(1,0,0), Y=(0,1,0), Z=(0,0,1)
inline float cosTheta(const Vec3& w) {
    return w.z;
}

inline float cosTheta2(const Vec3& w) {
    return w.z * w.z;
}

inline bool sameHemisphere(const Vec3& w, const Vec3& wp) {
    return w.z * wp.z > 0.0f;
}


inline Vec3 Fr_Schlick(const Vec3& wh, const Vec3& wi, const Vec3& f0) {
    float a = 1.0f - dot(wh, wi);
    float a2 = a * a;
    float a5 = a2 * a2 * a;
    return f0 + (Vec3(1.0f) - f0) * a5;
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
    float cosTheta2_W = cosTheta2(w);
    float lambda = (-1.0f + std::sqrt(alpha2 * (1.0f - cosTheta2_W) / cosTheta2_W + 1.0f)) / 2.0f;
    return 1.0f / (1.0f + lambda);
}

inline float G2_SmithUncorrelated_GGX(const Vec3& wi, const Vec3& wo, const Vec3& wh, float alpha) {
    return G1_Smith_GGX(wi, wh, alpha) * G1_Smith_GGX(wo, wh, alpha);
}

inline float G2_SmithHeightCorrelated_GGX(const Vec3& wi, const Vec3& wo, const Vec3& wh, float alpha) {
    if (dot(wi, wh) <= 0.0f || dot(wo, wh) <= 0.0f) {
        return 0.0f;
    }

    float cosTheta2_O = cosTheta2(wo);
    float cosTheta2_I = cosTheta2(wi);
    assert(cosTheta2_O > 0.0f);
    assert(cosTheta2_I > 0.0f);

    float alpha2 = alpha * alpha;
    float lambda_wo = (-1.0f + std::sqrt(alpha2 * (1.0f - cosTheta2_O) / cosTheta2_O + 1.0f)) / 2.0f;
    float lambda_wi = (-1.0f + std::sqrt(alpha2 * (1.0f - cosTheta2_I) / cosTheta2_I + 1.0f)) / 2.0f;
    return 1.0f / (1.0f + lambda_wo + lambda_wi);
}

inline float G2_None(const Vec3& wi, const Vec3& wo, const Vec3& wh, float alpha) {
    return 1.0f;
}


// BxDF functions
inline Vec3 diffuse_Lambert(const Vec3& diffuseColor) {
    return diffuseColor / pi<float>;
}

inline Vec3 specular_GGX(const Vec3& wo, const Vec3& wi, const Vec3& f0, float alpha) {
    Vec3 wh = normalize(wo + wi);
    Vec3 F = Fr_Schlick(wh, wi, f0);
    float cosThetaO = cosTheta(wo);
    float cosThetaI = cosTheta(wi);
    float cosThetaH = cosTheta(wh);

    if (cosThetaO <= 0.0f || cosThetaI <= 0.0f || cosThetaH <= 0.0f) {
        return Vec3(0.0f);
    }
    assert(cosThetaO > 0.0f);
    assert(cosThetaI > 0.0f);
    assert(cosThetaH > 0.0f);

    float G = G2_SmithUncorrelated_GGX(wi, wo, wh, alpha);
    float D = D_GGX(wh, alpha);
    return F * G * D / (4.0f * cosThetaO * cosThetaI);
}


inline Vec3 sampleCosineHemisphere(float u1, float u2) {
    float r = std::sqrt(u1);
    float phi = 2.0f * pi<float> *u2;
    return Vec3(r * std::sin(phi), r * std::cos(phi), std::sqrt(1.0f - u1));
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

inline float pdfGGX(const Vec3& wi, const Vec3& wo, float alpha) {
    if (!sameHemisphere(wi, wo)) {
        return 0.0f;
    }

    Vec3 wh = normalize(wi + wo);
    float pdf_h = D_GGX(wh, alpha) * abs(cosTheta(wh));
    float dwh_dwi = 4.0f * dot(wo, wh);
    return pdf_h / dwh_dwi;
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

inline float pdfGGX_VNDF(const Vec3& wi, const Vec3& wo, float alpha) {
    if (!sameHemisphere(wi, wo)) {
        return 0.0f;
    }

    Vec3 wh = normalize(wi + wo);
    float dotHO = dot(wh, wo);
    float pdf_h = G1_Smith_GGX(wo, wh, alpha) * D_GGX(wh, alpha) * dotHO / abs(cosTheta(wo));
    float dwh_dwi = 4.0f * dotHO;
    return pdf_h / dwh_dwi;
}

} // namespace pt

#pragma once

#include "MathUtils.h"
#include "OrthonormalBasis.h"

namespace pt {

// All shading is done in local space where: X=(1,0,0), Y=(0,1,0), Z=(0,0,1)
inline float cosTheta(const Vec3& w) {
    return w.z;
}

inline Vec3 Fr_Schlick(const Vec3& wh, const Vec3& wi, const Vec3& f0) {
    float a = 1.0f - dot(wh, wi);
    float a2 = a * a;
    float a5 = a2 * a2 * a;
    return f0 + (Vec3(1.0f) - f0) * a5;
}

inline float D_GGX(const Vec3& wh, float alpha) {
    float dotNH = cosTheta(wh);
    if (dotNH <= 0.0f) {
        return 0.0f;
    }

    float alpha2 = alpha * alpha;
    float a = 1.0f + dotNH * dotNH * (alpha2 - 1.0f);
    return alpha2 / (pi<float> * a * a);
}

inline float G1_Smith_GGX(const Vec3& w, const Vec3& wh, float alpha) {
    if (dot(w, wh) <= 0.0f) {
        return 0.0f;
    }

    float alpha2 = alpha * alpha;
    float dotNW = cosTheta(w);
    float dotNW2 = dotNW * dotNW;
    float lambda = (-1.0f + std::sqrt(alpha2 * (1.0f - dotNW2) / dotNW2 + 1.0f)) / 2.0f;
    return 1.0f / (1.0f + lambda);
}

inline float G2_SmithUncorrelated_GGX(const Vec3& wi, const Vec3& wo, const Vec3& wh, float alpha) {
    return G1_Smith_GGX(wi, wh, alpha) * G1_Smith_GGX(wo, wh, alpha);
}

inline float G2_SmithHeightCorrelated_GGX(const Vec3& wi, const Vec3& wo, const Vec3& wh, float alpha) {
    if (dot(wi, wh) <= 0.0f || dot(wo, wh) <= 0.0f) {
        return 0.0f;
    }

    float dotNO = cosTheta(wo);
    float dotNI = cosTheta(wi);
    float dotNO2 = dotNO * dotNO;
    float dotNI2 = dotNI * dotNI;
    assert(dotNO2 > 0.0f);
    assert(dotNI2 > 0.0f);

    float alpha2 = alpha * alpha;
    float lambda_wo = (-1.0f + std::sqrt(alpha2 * (1.0f - dotNO2) / dotNO2 + 1.0f)) / 2.0f;
    float lambda_wi = (-1.0f + std::sqrt(alpha2 * (1.0f - dotNI2) / dotNI2 + 1.0f)) / 2.0f;
    return 1.0f / (1.0f + lambda_wo + lambda_wi);
}

inline float G2_None(const Vec3& wi, const Vec3& wo, const Vec3& wh, float alpha) {
    return 1.0f;
}

inline Vec3 diffuse_Lambert(const Vec3& diffuseColor) {
    return diffuseColor / pi<float>;
}

inline Vec3 specular_GGX(const Vec3& wo, const Vec3& wi, const Vec3& f0, float alpha) {
    Vec3 wh = normalize(wo + wi);
    Vec3 F = Fr_Schlick(wh, wi, f0);
    float dotNO = cosTheta(wo);
    float dotNI = cosTheta(wi);
    float dotNH = cosTheta(wh);

    if (dotNO <= 0.0f || dotNI <= 0.0f || dotNH <= 0.0f) {
        return Vec3(0.0f);
    }
    assert(dotNO > 0.0f);
    assert(dotNI > 0.0f);
    assert(dotNH > 0.0f);

    float G = G2_SmithUncorrelated_GGX(wi, wo, wh, alpha);
    float D = D_GGX(wh, alpha);
    return F * G * D / (4.0f * dotNO * dotNI);
}

inline Vec3 sampleCosineHemisphere(float u1, float u2) {
    float r = std::sqrt(u1);
    float phi = 2.0f * pi<float> *u2;
    return Vec3(r * std::sin(phi), r * std::cos(phi), std::sqrt(1.0f - u1));
}

inline float pdfCosineHemisphere(const Vec3& wi) {
    return cosTheta(wi) / pi<float>;
}

inline Vec3 sampleGGX(float alpha, float u1, float u2) {
    float phi = 2.0f * pi<float> *u1;
    float theta = std::acos(std::sqrt((1.0f - u2) / ((alpha * alpha - 1.0f) * u2 + 1.0f)));

    float x = std::sin(theta) * std::cos(phi);
    float y = std::sin(theta) * std::sin(phi);
    float z = std::cos(theta);

    return Vec3(x, y, z);
}

inline float pdfGGX(const Vec3& wh, const Vec3& wo, float alpha) {
    return D_GGX(wh, alpha) * cosTheta(wh) / (4.0f * dot(wo, wh));
}

// See: http://jcgt.org/published/0007/04/01/paper.pdf
inline Vec3 sampleGGXVNDF(const Vec3& wo, float alpha, float u1, float u2) {
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
    float phi = 2.0f * pi<float> *u2;
    float t1 = r * std::cos(phi);
    float t2 = r * std::sin(phi);
    float s = 0.5f * (1.0f + woHemi.z);
    t2 = (1.0f - s) * std::sqrt(1.0f - t1 * t1) + s * t2;

    // Reprojection onto hemisphere
    Vec3 whHemi = t1 * b1 + t2 * b2 + std::sqrt(max(0.0f, 1.0f - t1 * t1 - t2 * t2)) * woHemi;

    // Transforming half vector back to ellipsoid configuration
    return normalize(Vec3(alpha * whHemi.x, alpha * whHemi.y, max(0.0f, whHemi.z)));
}

inline float pdfGGXVNDF(const Vec3& wh, const Vec3& wo, float alpha) {
    // dot(wo, wh) cancels out
    return G1_Smith_GGX(wo, wh, alpha) * D_GGX(wh, alpha) / (4.0f * cosTheta(wo));
}

} // namespace pt

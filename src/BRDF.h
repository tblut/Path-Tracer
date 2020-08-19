#pragma once

#include "MathUtils.h"
#include "Vector3.h"

#include <cmath>
#include <cassert>

namespace pt {

// See: https://graphics.pixar.com/library/OrthonormalB/paper.pdf
inline void makeOrthonormalBasis(const Vec3& normal, Vec3& b1, Vec3& b2) {
    const float sign = std::copysign(1.0f, normal.z);
    const float a = -1.0f / (sign + normal.z);
    const float b = normal.x * normal.y * a;
    b1 = Vec3(1.0f + sign * normal.x * normal.x * a, sign * b, -sign * normal.x);
    b2 = Vec3(b, sign + normal.y * normal.y * a, -normal.y);
}

inline Vec3 localToWorld(const Vec3& v, const Vec3& b1, const Vec3& b2, const Vec3& b3) {
    return Vec3(
        v.x * b1.x + v.y * b2.x + v.z * b3.x,
        v.x * b1.y + v.y * b2.y + v.z * b3.y,
        v.x * b1.z + v.y * b2.z + v.z * b3.z
    );
}

inline Vec3 worldToLocal(const Vec3& v, const Vec3& b1, const Vec3& b2, const Vec3& b3) {
    return Vec3(dot(v, b1), dot(v, b2), dot(v, b3));
}

inline Vec3 sampleCosineHemisphere(float u1, float u2) {
    const float r = std::sqrt(u1);
    const float phi = 2.0f * pi<float> * u2;
    return Vec3(r * std::sin(phi), r * std::cos(phi), std::sqrt(1.0f - u1));
}

inline float pdfCosineHemisphere(const Vec3& normal, const Vec3& wi) {
    return dot(normal, wi) / pi<float>;
}

inline Vec3 Fr_Schlick(const Vec3& wh, const Vec3& wi, const Vec3& f0) {
    return f0 + (Vec3(1.0f) - f0) * std::pow(1.0f - dot(wh, wi), 5);
}

inline float D_GGX(const Vec3& wh, const Vec3& normal, float alpha) {
    const float dotNH = dot(wh, normal);
    if (dotNH <= 0.0f) {
        return 0.0f;
    }

    const float alpha2 = alpha * alpha;
    const float a = 1.0f + dotNH * dotNH * (alpha2 - 1.0f);
    return alpha2 / (pi<float> * a * a);
}

inline float G1_Smith_GGX(const Vec3& w, const Vec3& wh, const Vec3& normal, float alpha) {
    if (dot(w, wh) <= 0.0f) {
        return 0.0f;
    }

    const float alpha2 = alpha * alpha;
    const float dotNW = dot(w, normal);
    const float dotNW2 = dotNW * dotNW;
    const float lambda = (std::sqrt(1.0f + alpha2 * (1.0f - dotNW2) / dotNW2) - 1.0f) / 2.0f;
    return 1.0f / (1.0f + lambda);
}

inline float G2_SmithUncorrelated_GGX(const Vec3& wi, const Vec3& wo, const Vec3& wh, const Vec3& normal, float alpha) {
    return G1_Smith_GGX(wi, wh, normal, alpha) * G1_Smith_GGX(wo, wh, normal, alpha);
}

inline float G2_SmithHeightCorrelated_GGX(const Vec3& wi, const Vec3& wo, const Vec3& wh, const Vec3& normal, float alpha) {
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

inline float G2_None(const Vec3& wi, const Vec3& wo, const Vec3& wh, const Vec3& normal, float alpha) {
    return 1.0f;
}

inline Vec3 diffuse_Lambert(const Vec3& diffuseColor) {
    return diffuseColor / pi<float>;
}

inline Vec3 specular_GGX(const Vec3& normal, const Vec3& wo, const Vec3& wi, const Vec3& f0, float alpha) {
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

inline Vec3 sampleGGX(float alpha, float u1, float u2) {
    float phi = 2.0f * pi<float> * u1;
    float theta = std::acos(std::sqrt((1.0f - u2) / ((alpha * alpha - 1.0f) * u2 + 1.0f)));

    float x = std::sin(theta) * std::cos(phi);
    float y = std::sin(theta) * std::sin(phi);
    float z = std::cos(theta);

    return Vec3(x, y, z);
}

inline float pdfGGX(const Vec3& normal, const Vec3& wh, const Vec3& wo, float alpha) {
    return D_GGX(wh, normal, alpha) * dot(wh, normal) / (4 * dot(wo, wh));
}

} // namespace pt

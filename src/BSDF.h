#pragma once

#include "MathUtils.h"
#include "OrthonormalBasis.h"

// NOTE: Many of these functions are not optimized at all for clarity and debugging
namespace pt {

// All shading is done in local space where X=(1,0,0), Y=(0,1,0), Z=(0,0,1)
// and the shading normal N=Z=(0,0,1). Thus, cosTheta = dot(n,w) = w.z
inline float cosTheta(const Vec3& w) { return w.z; }
inline float cosTheta2(const Vec3& w) { return w.z * w.z; }
inline float sinTheta2(const Vec3& w) { return max(0.0f, 1.0f - cosTheta2(w)); }
inline float sinTheta(const Vec3& w) { return std::sqrt(sinTheta2(w)); }
inline float tanTheta(const Vec3& w) { return sinTheta(w) / cosTheta(w); }
inline float tanTheta2(const Vec3& w) { return sinTheta2(w) / cosTheta2(w); }

inline bool sameHemisphere(const Vec3& w, const Vec3& wp) {
    return w.z * wp.z > 0.0f;
}


inline Vec3 schlickF0FromRelativeIOR(float eta) {
    float a = (1 - eta) / (1 + eta);
    return Vec3(a * a);
}

inline Vec3 Fr_Schlick(float cosThetaI, const Vec3& f0) {
    float a = max(0.0f, 1.0f - cosThetaI);
    float a2 = a * a;
    float a5 = a2 * a2 * a;
    return f0 + (Vec3(1.0f) - f0) * a5;
}

inline float D_GGX(const Vec3& wh, float alpha) {
    float alpha2 = alpha * alpha;
    float a = 1.0f + cosTheta2(wh) * (alpha2 - 1.0f);
    return alpha2 / (pi<float> * a * a);
}

inline float G1_Smith_GGX(const Vec3& w, float alpha) {
    float tan2ThetaW = tanTheta2(w);
    if (std::isinf(tan2ThetaW)) return 0.0f;
    float alpha2 = alpha * alpha;
    assert(alpha2 * tan2ThetaW >= -1.0f);
    float lambda = (-1.0f + std::sqrt(alpha2 * tan2ThetaW + 1.0f)) / 2.0f;
    return 1.0f / (1.0f + lambda);
}

inline float G2_SmithUncorrelated_GGX(const Vec3& wi, const Vec3& wo, float alpha) {
    return G1_Smith_GGX(wi, alpha) * G1_Smith_GGX(wo, alpha);
}

inline float G2_SmithHeightCorrelated_GGX(const Vec3& wi, const Vec3& wo, float alpha) {
    float tan2ThetaO = tanTheta2(wo);
    float tan2ThetaI = tanTheta2(wi);
    if (std::isinf(tan2ThetaO)) return 0.0f;
    if (std::isinf(tan2ThetaI)) return 0.0f;
    float alpha2 = alpha * alpha;
    assert(alpha2 * tan2ThetaO >= -1.0f);
    assert(alpha2 * tan2ThetaI >= -1.0f);
    float lambda_wo = (-1.0f + std::sqrt(alpha2 * tan2ThetaO + 1.0f)) / 2.0f;
    float lambda_wi = (-1.0f + std::sqrt(alpha2 * tan2ThetaI + 1.0f)) / 2.0f;
    return 1.0f / (1.0f + lambda_wo + lambda_wi);
}

inline float G2_None(const Vec3& wi, const Vec3& wo, float alpha) {
    return 1.0f;
}


// BxDF functions
inline Vec3 diffuse_Lambert(const Vec3& wi, const Vec3& wo, const Vec3& diffuseColor) {
    if (!sameHemisphere(wi, wo)) {
        return Vec3(0.0f);
    }

    return diffuseColor / pi<float>;
}

inline Vec3 microfacetReflection_GGX(const Vec3& wi, const Vec3& wo, const Vec3& f0, float eta, float alpha) {
    if (!sameHemisphere(wi, wo) || cosTheta(wi) == 0.0f || cosTheta(wo) == 0.0f) {
        return Vec3(0.0f);
    }

    Vec3 wh = wi + wo;
    if (wh.x == 0.0f && wh.y == 0.0f && wh.z == 0.0f) {
        return Vec3(0.0f);
    }
    wh = normalize(wh);

    Vec3 F;
    if (eta < 1.0f) {
        float cosThetaT = dot(wi, wh);
        float cos2ThetaT = cosThetaT * cosThetaT;
        F = cos2ThetaT > 0.0f ? Fr_Schlick(abs(cosThetaT), f0) : Vec3(1.0f);
    }
    else {
        F = Fr_Schlick(abs(dot(wh, wo)), f0);
    }

    float G = G2_SmithHeightCorrelated_GGX(wi, wo, alpha);
    float D = D_GGX(wh, alpha);
    return F * G * D / (4.0f * abs(cosTheta(wi)) * abs(cosTheta(wo)));
}

inline Vec3 microfacetTransmission_GGX(const Vec3& wi, const Vec3& wo, const Vec3& f0, float eta, float alpha) {
    if (sameHemisphere(wi, wo) || cosTheta(wi) == 0.0f || cosTheta(wo) == 0.0f) {
        return Vec3(0.0f);
    }

    Vec3 wh = normalize(wi + eta * wo);
    if (cosTheta(wh) < 0.0f) {
        wh = -wh;
    }

    bool sameSide = dot(wo, wh) * dot(wi, wh) > 0.0f;
    if (sameSide) {
        return Vec3(0.0f);
    }

    Vec3 F;
    if (eta < 1.0f) {
        float cosThetaT = dot(wi, wh);
        float cos2ThetaT = cosThetaT * cosThetaT;
        F = cos2ThetaT > 0.0f ? Fr_Schlick(abs(cosThetaT), f0) : Vec3(1.0f);
    }
    else {
        F = Fr_Schlick(abs(dot(wh, wo)), f0);
    }

    float G = G2_SmithHeightCorrelated_GGX(wi, wo, alpha);
    float D = D_GGX(wh, alpha);
    float denomSqrt = dot(wi, wh) + eta * dot(wo, wh);
    return (Vec3(1.0f) - F) * D * G * abs(dot(wi, wh)) * abs(dot(wo, wh))
        / (denomSqrt * denomSqrt * abs(cosTheta(wi)) * abs(cosTheta(wo)));
}


inline Vec3 sampleUniformSphere(float u1, float u2) {
    float cosTheta = 1.0f - 2.0f * u1;
    float sinTheta = std::sqrt(max(0.0f, 1.0f - cosTheta * cosTheta));
    float phi = 2.0f * pi<float> * u2;
    return Vec3(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
}

inline float pdfUniformSphere(const Vec3& wi, const Vec3& wo) {
    return 1.0f / (4.0f * pi<float>);
}


inline Vec3 sampleCosineHemisphere(float u1, float u2) {
    float cosTheta = std::sqrt(max(0.0f, 1.0f - u1));
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
    float sinTheta = std::sqrt(max(0.0f, 1.0f - cosTheta2));
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

inline float pdfGGX_transmission(const Vec3& wi, const Vec3& wo, float eta, float alpha) {
    if (sameHemisphere(wi, wo)) {
        return 0.0f;
    }

    Vec3 wh = normalize(wi + eta * wo);
    bool sameSide = dot(wo, wh) * dot(wi, wh) > 0.0f;
    if (sameSide) return 0.0f;

    float pdf_h = D_GGX(wh, alpha) * abs(cosTheta(wh));
    float sqrtDenom = dot(wi, wh) + eta * dot(wo, wh);
    float dwh_dwi = abs(dot(wi, wh)) / (sqrtDenom * sqrtDenom);
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
    float pdf_h = G1_Smith_GGX(wo, alpha) * D_GGX(wh, alpha) * abs(dot(wh, wo)) / abs(cosTheta(wo));
    float dwh_dwi = 1.0f / (4.0f * dot(wi, wh));
    return pdf_h * dwh_dwi;
}

inline float pdfGGX_VNDF_transmission(const Vec3& wi, const Vec3& wo, float eta, float alpha) {
    if (sameHemisphere(wi, wo)) {
        return 0.0f;
    }

    Vec3 wh = normalize(wi + eta * wo);
    bool sameSide = dot(wo, wh) * dot(wi, wh) > 0.0f;
    if (sameSide) return 0.0f;

    float pdf_h = G1_Smith_GGX(wo, alpha) * D_GGX(wh, alpha) * abs(dot(wh, wo)) / abs(cosTheta(wo));
    float sqrtDenom = dot(wi, wh) + eta * dot(wo, wh);
    float dwh_dwi = abs(dot(wi, wh)) / (sqrtDenom * sqrtDenom);
    return pdf_h * dwh_dwi;
}

} // namespace pt

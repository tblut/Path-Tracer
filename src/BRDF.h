#pragma once

#include "MathUtils.h"
#include "Vector3.h"

#include <cmath>

namespace pt {

// See: https://graphics.pixar.com/library/OrthonormalB/paper.pdf
inline void makeOrthonormalBasis(const Vec3& normal, Vec3& b1, Vec3& b2) {
    const float sign = std::copysign(1.0f, normal.z);
    const float a = -1.0f / (sign + normal.z);
    const float b = normal.x * normal.y * a;
    b1 = Vec3(1.0f + sign * normal.x * normal.x * a, sign * b, -sign * normal.x);
    b2 = Vec3(b, sign + normal.y * normal.y * a, -normal.y);
}

inline Vec3 transformVectorToBasis(const Vec3& v, const Vec3& b1, const Vec3& b2, const Vec3& b3) {
    return Vec3(
        v.x * b1.x + v.y * b2.x + v.z * b3.x,
        v.x * b1.y + v.y * b2.y + v.z * b3.y,
        v.x * b1.z + v.y * b2.z + v.z * b3.z
    );
}

inline Vec3 sampleCosineHemisphere(float u1, float u2) {
    const float r = std::sqrt(u1);
    const float phi = 2.0f * pi<float> * u2;
    return Vec3(r * std::sin(phi), r * std::cos(phi), std::sqrt(1.0f - u1));
}

inline float pdfCosineHemisphere(const Vec3& normal, const Vec3& wi) {
    return dot(normal, wi) / pi<float>;
}

inline Vec3 brdfLambert(const Vec3& diffuseColor) {
    return diffuseColor / pi<float>;
}

} // namespace pt

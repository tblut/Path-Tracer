#pragma once

#include "Vector3.h"

namespace pt {

class Material {
public:
    Vec3 evaluate(const Vec3& wi, const Vec3& wo, const Vec3& normal) const;
    Vec3 sampleDirection(Vec3 wo, Vec3 normal,
        float u1, float u2, float* pdf = nullptr) const;
    float pdf(const Vec3& wi, const Vec3& wo, const Vec3& normal) const;

    Vec3 baseColor;
    float roughness;
    float metalness;
    Vec3 emittance;
};

} // namespace pt

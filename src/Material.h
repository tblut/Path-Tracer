#pragma once

#include "Vector3.h"

namespace pt {

class Material {
public:
    // All vectors are expected to be in local space
    Vec3 evaluate(const Vec3& wi, const Vec3& wo, const Vec3& normal) const;
    Vec3 sampleDirection(Vec3 wo, Vec3 normal,
        float u1, float u2, float* pdf = nullptr) const;
    float pdf(const Vec3& wi, const Vec3& wo, const Vec3& normal) const;

    Vec3 baseColor;
    float roughness;
    float metalness;
    Vec3 emittance;
};

// All shading is done in local space where: X=(1,0,0), Y=(0,1,0), Z=(0,0,1)
inline float cosTheta(const Vec3& w) {
    return w.z;
}

} // namespace pt

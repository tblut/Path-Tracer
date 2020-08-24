#pragma once

#include "Vector3.h"

namespace pt {

class Material {
public:
    Material(const Vec3& baseColor, float roughness,
        float metalness, const Vec3& emittance = Vec3(0.0f));

    // All vectors are expected to be in local space
    Vec3 evaluate(const Vec3& wi, const Vec3& wo, const Vec3& normal) const;
    Vec3 sampleDirection(const Vec3& wo, const Vec3& normal,
        float u1, float u2, float* pdf = nullptr) const;
    float pdf(const Vec3& wi, const Vec3& wo, const Vec3& normal) const;

    const Vec3& getBaseColor() const { return baseColor_; }
    float getRoughness() const { return roughness_; }
    float getMetalness() const { return metalness_; }
    const Vec3& getEmittance() const { return emittance_; }

private:
    Vec3 baseColor_;
    float roughness_;
    float metalness_;
    Vec3 emittance_;

    // Precomputed values
    float alpha_;
    Vec3 kD_;
    Vec3 kS_;
};

// All shading is done in local space where: X=(1,0,0), Y=(0,1,0), Z=(0,0,1)
inline float cosTheta(const Vec3& w) {
    return w.z;
}

} // namespace pt

#pragma once

#include "Vector3.h"

namespace pt {

class Material {
public:
    Material(const Vec3& baseColor, float specular,
        float roughness, float metalness,
        float transmission = 0.0f, float ior = 1.0f,
        const Vec3& emittance = Vec3(0.0f));

    // All vectors are expected to be in local space
    Vec3 evaluate(const Vec3& wi, const Vec3& wo, const Vec3& normal) const;
    Vec3 sampleDirection(const Vec3& wo, const Vec3& normal,
        float u1, float u2, float* pdf = nullptr) const;
    float pdf(const Vec3& wi, const Vec3& wo, const Vec3& normal) const;

    const Vec3& getBaseColor() const { return baseColor_; }
    float getSpecular() const { return specular_; }
    float getRoughness() const { return roughness_; }
    float getMetalness() const { return metalness_; }
    float getTransmission() const { return transmission_; }
    float getIOR() const { return ior_; }
    const Vec3& getEmittance() const { return emittance_; }

private:
    Vec3 baseColor_;
    float specular_;
    float roughness_;
    float metalness_;
    float transmission_;
    float ior_;
    Vec3 emittance_;

    // Precomputed values
    float alpha_;
    Vec3 kD_;
    Vec3 kS_;
};

} // namespace pt

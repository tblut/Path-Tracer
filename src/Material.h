#pragma once

#include "Vector3.h"

namespace pt {

class Material {
public:
    Material(const Vec3& baseColor, float roughness, float metalness,
        float transmission = 0.0f, float ior = 1.5f,
        const Vec3& emittance = Vec3(0.0f));

    // All vectors are expected to be in local space
    Vec3 evaluate(const Vec3& wi, const Vec3& wo) const;
    Vec3 sampleDirection(const Vec3& wo, float u1, float u2, float* pdf = nullptr) const;
    float pdf(const Vec3& wi, const Vec3& wo) const;

    const Vec3& getBaseColor() const { return baseColor_; }
    float getRoughness() const { return roughness_; }
    float getMetalness() const { return metalness_; }
    float getTransmission() const { return transmission_; }
    float getIOR() const { return ior_; }
    const Vec3& getEmittance() const { return emittance_; }

private:
    float computeRelativeIOR(const Vec3& wo) const;
    void computeLobeProbabilities(const Vec3& wo, float& pDiffuse, float& pSpecular, float& pTransmission) const;

    Vec3 baseColor_;
    float roughness_;
    float metalness_;
    float transmission_;
    float ior_;
    Vec3 emittance_;

    // Precomputed values
    float alpha_;
};

} // namespace pt

#pragma once

#include "Vector3.h"
#include "Shape.h"

#include <vector>

namespace pt {

class Triangle : public Shape {
public:
    Triangle(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Material& material);
    virtual ~Triangle() = default;

    virtual float intersect(const Ray& ray) const override;
    virtual Vec3 normalAt(const Vec3& p) const override;
    virtual Vec3 sampleDirection(const Vec3& p, float u1, float u2, float* pdf = nullptr) const override;
    virtual float pdf(const Vec3& p, const Vec3& wi) const override;

private:
    Vec3 p0_, p1_, p2_;
    Vec3 normal_;
    float area_;
};

} // namespace pt

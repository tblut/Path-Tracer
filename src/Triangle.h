#pragma once

#include "Shape.h"
#include "Vector3.h"

namespace pt {

class Triangle : public Shape {
public:
    Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Material& material);
    Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2,
        const Vec3& n0, const Vec3& n1, const Vec3& n2, const Material& material);
    virtual ~Triangle() = default;

    virtual RayHit intersect(const Ray& ray) const override;
    virtual BoundingBox getWorldBounds() const override;
    virtual Vec3 sampleDirection(const Vec3& p, float u1, float u2, float* pdf = nullptr) const override;
    virtual float pdf(const Vec3& p, const Vec3& wi) const override;

private:
    Vec3 vertices_[3];
    Vec3 normals_[3];
    float area_;
};

} // namespace pt

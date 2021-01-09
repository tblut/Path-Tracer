#pragma once

#include "Shape.h"
#include "Vector3.h"

namespace pt {

class Sphere : public Shape {
public:
    Sphere(const Vec3& center, float radius, const Material& material);
    virtual ~Sphere() = default;

    virtual RayHit intersect(const Ray& ray) const override;
    virtual BoundingBox getWorldBounds() const override;
    virtual Vec3 sampleDirection(const Vec3& p, float u1, float u2, float* pdf = nullptr) const override;
    virtual float pdf(const Vec3& p, const Vec3& wi) const override;

    const Vec3 getCenter() const { return center_; }
    float getRadius() const { return radius_; }

private:
    Vec3 center_;
    float radius_;
    float radiusSq_;
    float invRadius_;
};

} // namespace pt

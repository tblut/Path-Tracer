#pragma once

#include "BoundingBox.h"
#include "Material.h"
#include "Ray.h"

namespace pt {

class Shape {
public:
    Shape(const Material& material_)
        : material(&material_)
    {
    }
    virtual ~Shape() = default;

    virtual RayHit intersect(const Ray& ray) const = 0;
    virtual BoundingBox getWorldBounds() const = 0;

    // Returns a uniformly sampled direction in world space from the point p to this shape
    virtual Vec3 sampleDirection(const Vec3& p, float u1, float u2, float* pdf = nullptr) const = 0;

    // Returns the PDF of the uniformly sampled direction wi from the point p (both in world space) 
    virtual float pdf(const Vec3& p, const Vec3& wi) const = 0;

    bool isLight() const {
        return material->getEmittance().r > 0.0f
            || material->getEmittance().g > 0.0f
            || material->getEmittance().b > 0.0f;
    }

    const Material* material;
};

} // namespace pt

#pragma once

#include "Ray.h"
#include "Material.h"

namespace pt {

class Material;

class Shape {
public:
    Shape(const Material& material_) : material(&material_) { }
    virtual ~Shape() = default;
    virtual float intersect(const Ray& ray) const = 0;
    virtual Vec3 normalAt(const Vec3& p) const = 0;

    // Returns a uniformly sampled direction from the point p to this shape
    virtual Vec3 sampleDirection(const Vec3& p, float u1, float u2, float* pdf = nullptr) const = 0;

    // Returns the PDF of uniformly sampled directions from the point p 
    virtual float pdf(const Vec3& p) const = 0;

    bool isLight() const {
        return material->getEmittance().r > 0.0f
            || material->getEmittance().g > 0.0f
            || material->getEmittance().b > 0.0f;
    }

    const Material* material;
};

} // namespace pt

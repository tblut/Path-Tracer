#pragma once

#include "Ray.h"
#include "Material.h"

namespace pt {

struct Material;

class Shape {
public:
    Shape(const Material& material_) : material(&material_) { }
    virtual ~Shape() = default;
    virtual RayHit intersect(const Ray& ray) const = 0;
    virtual Vec3 sampleDirection(const Vec3& p, float u1, float u2, float& pdf) const = 0;

    bool isLight() const {
        return material->emittance.r > 0.0f
            || material->emittance.g > 0.0f
            || material->emittance.b > 0.0f;
    }

    const Material* material;
};

} // namespace pt

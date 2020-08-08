#pragma once

#include "Ray.h"

namespace pt {

struct Material;

class Shape {
public:
    Shape(const Material& material_) : material(&material_) { }
    virtual ~Shape() = default;
    virtual RayHit intersect(const Ray& ray) const = 0;

    const Material* material;
};

} // namespace pt

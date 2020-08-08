#pragma once

#include "Shape.h"
#include "Vector3.h"

namespace pt {

class Plane : public Shape {
public:
    Plane(const Vec3& p0_, const Vec3& normal_, const Material& material_);
    virtual ~Plane() = default;

    virtual RayHit intersect(const Ray& ray) const override;

    Vec3 p0;
    Vec3 normal;
};

} // namespace pt

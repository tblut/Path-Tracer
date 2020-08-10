#pragma once

#include "Shape.h"
#include "MathUtils.h"
#include "Vector3.h"
#include "Ray.h"

namespace pt{

class Sphere : public Shape {
public:
    Sphere(const Vec3& center_, float radius_, const Material& material_);
    virtual ~Sphere() = default;

    virtual RayHit intersect(const Ray& ray) const override;
    virtual Vec3 sample(const Vec3& p, float u1, float u2, float& pdf) const override;

    Vec3 center;
    float radius;
};

} // namespace pt

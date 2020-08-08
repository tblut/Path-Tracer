#pragma once

#include "MathUtils.h"
#include "Vector3.h"
#include "Ray.h"

namespace pt{

struct Sphere {
    constexpr Sphere(const Vec3& center_, float radius_)
        : center(center_)
        , radius(radius_)
    {
    }

    Vec3 center;
    float radius;
};

inline RayHit intersect(const Ray& ray, const Sphere& sphere) {
    const float a = dot(ray.direction, ray.direction);
    const Vec3 oc = ray.origin - sphere.center;
    const float b = 2.0f * dot(ray.direction, oc);
    const float c = dot(oc, oc) - sphere.radius * sphere.radius;
    const float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) {
        return RayHit(-inf<float>, Vec3());
    }
    const float tmin = (-b - std::sqrt(discriminant)) / (2.0f * a);
    const Vec3 normal = (ray.at(tmin) - sphere.center) / sphere.radius;
    return RayHit(tmin, normal);
}

} // namespace pt

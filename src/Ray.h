#pragma once

#include "Vector3.h"

namespace pt {

class Shape;

struct Ray {
    Ray() = default;
    constexpr Ray(const Vec3& origin_, const Vec3& direction_)
        : origin(origin_)
        , direction(direction_)
    {
    }

    constexpr Vec3 at(float t) const {
        return origin + direction * t;
    }

    Vec3 origin;
    Vec3 direction;
};

struct RayHit {
    RayHit() = default;
    constexpr RayHit(float t_, const Vec3& normal_, const Shape* shape_)
        : t(t_)
        , normal(normal_)
        , shape(shape_)
    {
    }

    float t;
    Vec3 normal;
    const Shape* shape;
};

constexpr RayHit rayMiss = RayHit(-inf<float>, Vec3(), nullptr);

} // namespace pt

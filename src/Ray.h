#pragma once

#include "Vector3.h"

namespace pt {

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
    constexpr RayHit(float t_, const Vec3& normal_)
        : t(t_)
        , normal(normal_)
    {
    }

    float t;
    Vec3 normal;
};

} // namespace pt

#pragma once

#include "math/Vector3.h"

namespace pt {

struct Ray {
    Ray() = default;
    Ray(const Vec3& origin_, const Vec3& direction_)
        : origin(origin_)
        , direction(direction_)
    {
    }

    Vec3 at(float t) const {
        return origin + direction * t;
    }

    Vec3 origin;
    Vec3 direction;
};

struct RayHit {
    float t;
    Vec3 normal;
};

} // namespace pt

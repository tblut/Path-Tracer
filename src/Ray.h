#pragma once

#ifdef _MSC_VER
#pragma warning(disable: 4723) // Potential division by zero
#endif

#include "Vector3.h"

namespace pt {

class Shape;

struct Ray {
    Ray() = default;
    constexpr Ray(const Vec3& origin_, const Vec3& direction_)
        : origin(origin_)
        , direction(direction_)
        , invDirection(Vec3(1.0f) / direction_)
        , sign{ invDirection.x < 0.0f, invDirection.y < 0.0f, invDirection.z < 0.0f }
    {
    }

    constexpr Vec3 at(float t) const {
        return origin + direction * t;
    }

    Vec3 origin;
    Vec3 direction;
    Vec3 invDirection;
    int sign[3]; // If the ray direction signs are negative
};

struct RayHit {
    RayHit() = default;
    constexpr RayHit(float t_, const Vec3& normal_, const Shape* shape_)
        : t(t_)
        , normal(normal_)
        , shape(shape_)
    {
    }

    constexpr explicit operator bool() const {
        return t >= 0.0f;
    }

    float t;
    Vec3 normal;
    const Shape* shape;
};

constexpr RayHit rayMiss = RayHit(-inf<float>, Vec3(), nullptr);

} // namespace pt

#ifdef _MSC_VER
#pragma warning(default: 4723)
#endif

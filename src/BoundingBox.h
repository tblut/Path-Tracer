#pragma once

#include "MathUtils.h"
#include "Vector3.h"
#include "Ray.h"

namespace pt {

struct BoundingBox {
    BoundingBox() = default;
    constexpr BoundingBox(const Vec3& min_, const Vec3& max_)
        : min(min_)
        , max(max_)
    {
    }

    constexpr Vec3 getCenter() const { return (min + max) * 0.5f; }
    constexpr Vec3 getExtents() const { return (max - min) * 0.5f; }
    constexpr float getSurfaceArea() const {
        Vec3 size = max - min;
        return 2.0f * (size.x * size.y + size.x * size.z + size.y * size.z);
    }

    union {
        Vec3 p[2]; // 0=min, 1=max
        struct {
            Vec3 min;
            Vec3 max;
        };
    };
};

// See: An Efficient and Robust Ray–Box Intersection Algorithm (2005), Williams et al.
inline bool testIntersection(const Ray& ray, const BoundingBox& box) {
    float tmin = (box.p[ray.sign[0]].x - ray.origin.x) * ray.invDirection.x;
    float tmax = (box.p[1 - ray.sign[0]].x - ray.origin.x) * ray.invDirection.x;
    float tminY = (box.p[ray.sign[1]].y - ray.origin.y) * ray.invDirection.y;
    float tmaxY = (box.p[1 - ray.sign[1]].y - ray.origin.y) * ray.invDirection.y;
    if (tmin > tmaxY || tminY > tmax) {
        return false;
    }

    tmin = max(tmin, tminY);
    tmax = min(tmax, tmaxY);
    float tminZ = (box.p[ray.sign[2]].z - ray.origin.z) * ray.invDirection.z;
    float tmaxZ = (box.p[1 - ray.sign[2]].z - ray.origin.z) * ray.invDirection.z;
    if (tmin > tmaxZ || tminZ > tmax) {
        return false;
    }

    tmax = min(tmax, tmaxZ);
    return tmax >= 0.0f;
}

} // namespace pt

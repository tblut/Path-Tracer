#pragma once

#include "MathUtils.h"
#include "Vector3.h"
#include "Ray.h"

namespace pt {

struct BoundingBox {
    BoundingBox() = default;
    BoundingBox(const Vec3& min_, const Vec3& max_)
        : min(min_)
        , max(max_)
    {
    }

    Vec3 getCenter() const { return (min + max) * 0.5f; }
    Vec3 getExtents() const { return (max - min) * 0.5f; }

    Vec3 min;
    Vec3 max;
};

// See: https://tavianator.com/2015/ray_box_nan.html
inline bool testIntersection(const Ray& ray, const BoundingBox& box) {
    float t1 = (box.min[0] - ray.origin[0]) * ray.invDirection[0];
    float t2 = (box.max[0] - ray.origin[0]) * ray.invDirection[0];

    float tmin = min(t1, t2);
    float tmax = max(t1, t2);

    for (int i = 1; i < 3; i++) {
        t1 = (box.min[i] - ray.origin[i]) * ray.invDirection[i];
        t2 = (box.max[i] - ray.origin[i]) * ray.invDirection[i];

        tmin = max(tmin, min(t1, t2));
        tmax = min(tmax, max(t1, t2));
    }

    return tmax > max(tmin, 0.0f);
}

} // namespace pt

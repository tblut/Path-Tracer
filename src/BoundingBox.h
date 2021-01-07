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

    Vec3 min;
    Vec3 max;
};

inline bool testIntersection(const Vec3& rayOrigin,
        const Vec3& rayInvDirection, const BoundingBox& box) {
    Vec3 t0 = (box.min - rayOrigin) * rayInvDirection;
    Vec3 t1 = (box.max - rayOrigin) * rayInvDirection;
    float tmin = maxComponent(min(t0, t1));
    float tmax = minComponent(max(t0, t1));
    return (tmin <= tmax) & (tmax >= 0.0f); // Slightly better ASM on MSVC with & instead of &&
}

} // namespace pt

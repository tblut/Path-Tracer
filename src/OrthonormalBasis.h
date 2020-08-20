#pragma once

#include "Vector3.h"

#include <cmath>
#include <cassert>

namespace pt {

class OrthonormalBasis {
public:
    OrthonormalBasis(const Vec3& b1_, const Vec3& b2_, const Vec3& b3_)
        : b1(b1_), b2(b2_), b3(b3_)
    {
    }

    // See: https://graphics.pixar.com/library/OrthonormalB/paper.pdf
    OrthonormalBasis(const Vec3& normal) {
        assert(isNormalized(normal));
        const float sign = std::copysign(1.0f, normal.z);
        const float a = -1.0f / (sign + normal.z);
        const float b = normal.x * normal.y * a;
        b1 = Vec3(1.0f + sign * normal.x * normal.x * a, sign * b, -sign * normal.x);
        b2 = Vec3(b, sign + normal.y * normal.y * a, -normal.y);
        b3 = normal;
    }

    Vec3 localToWorld(const Vec3& v) const {
        return Vec3(
            v.x * b1.x + v.y * b2.x + v.z * b3.x,
            v.x * b1.y + v.y * b2.y + v.z * b3.y,
            v.x * b1.z + v.y * b2.z + v.z * b3.z
        );
    }

    Vec3 worldToLocal(const Vec3& v) const {
        return Vec3(dot(v, b1), dot(v, b2), dot(v, b3));
    }

    Vec3 b1, b2, b3;
};

} // namespace pt

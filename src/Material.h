#pragma once

#include "Vector3.h"

namespace pt {

struct Material {
    Vec3 baseColor;
    float roughness;
    float metalness;
    Vec3 emittance;
};

} // namespace pt

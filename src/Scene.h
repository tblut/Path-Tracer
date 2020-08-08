#pragma once

#include "Sphere.h"
#include "Ray.h"

#include <vector>

namespace pt {

class Scene {
public:
    void addSphere(const Vec3& center, float radius);
    RayHit intersect(const Ray& ray) const;

private:
    std::vector<Sphere> spheres_;
};

} // namespace pt

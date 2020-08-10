#pragma once

#include "Ray.h"

#include <vector>

namespace pt {

class Shape;
class Sphere;

class Scene {
public:
    RayHit intersect(const Ray& ray) const;
    void add(const Shape& shape);
    void compile();

    const std::vector<const Shape*>& getLights() const {
        return lights_;
    }

private:
    std::vector<const Shape*> shapes_;
    std::vector<const Shape*> lights_;
};

} // namespace pt

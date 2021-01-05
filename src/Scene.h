#pragma once

#include "Ray.h"
#include "BVH.h"

#include <vector>
#include <memory>

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
    size_t getNumLights() const { return lights_.size(); }

private:
    std::vector<const Shape*> shapes_;
    std::vector<const Shape*> lights_;
    std::unique_ptr<BVH> bvh_;
};

} // namespace pt

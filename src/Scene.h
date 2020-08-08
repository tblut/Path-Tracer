#pragma once

#include "Sphere.h"
#include "Ray.h"

#include <vector>

namespace pt {

class Shape;
class Sphere;

class Scene {
public:
    RayHit intersect(const Ray& ray) const;
    void add(const Shape& shape);

    template <typename T>
    void add(std::initializer_list<T> shapes) {
        for (auto&& shape : shapes) {
            add(shape);
        }
    }

private:
    std::vector<const Shape*> shapes_;
};

} // namespace pt

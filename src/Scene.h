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

    template <typename T>
    void add(std::initializer_list<T> shapes) {
        for (auto&& shape : shapes) {
            add(shape);
        }
    }

    const std::vector<const Shape*>& getShapes() const { return shapes_; }

private:
    std::vector<const Shape*> shapes_;
};

} // namespace pt

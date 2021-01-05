#include "Scene.h"
#include "MathUtils.h"
#include "Shape.h"

namespace pt {

RayHit Scene::intersect(const Ray& ray) const {
    return bvh_->intersect(ray);
}

void Scene::add(const Shape& shape) {
    shapes_.push_back(&shape);
}

void Scene::compile() {
    for (const Shape* shape : shapes_) {
        if (shape->isLight()) {
            lights_.push_back(shape);
        }
    }

    bvh_ = std::make_unique<BVH>(shapes_, 1);
}

} // namespace pt

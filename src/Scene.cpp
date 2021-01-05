#include "Scene.h"
#include "MathUtils.h"
#include "Shape.h"

namespace pt {

RayHit Scene::intersect(const Ray& ray) const {
    RayHit closestHit = rayMiss;
    /*for (const Shape* shape : shapes_) {
        if (!testIntersection(ray, shape->getWorldBounds())) {
            continue;
        }

        float t = shape->intersect(ray);
        if (t >= 0.0f && (closestHit.t < 0.0f || t < closestHit.t)) {
            closestHit.t = t;
            closestHit.shape = shape;
        }
    }*/

    auto [t, closestShape] = bvh_->intersect(ray);
    if (t >= 0.0f) {
        closestHit.t = t;
    }

    if (closestShape) {
        closestHit.shape = closestShape;
        closestHit.normal = closestHit.shape->normalAt(ray.at(closestHit.t));
    }

    return closestHit;
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

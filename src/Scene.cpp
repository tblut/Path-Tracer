#include "Scene.h"
#include "MathUtils.h"
#include "Shape.h"

namespace pt {

RayHit Scene::intersect(const Ray& ray) const {
    RayHit closestHit = rayMiss;
    for (const Shape* shape : shapes_) {
        RayHit hit = shape->intersect(ray);
        if (hit.t >= 0.0f && (closestHit.t < 0.0f || hit.t < closestHit.t)) {
            closestHit = hit;
        }
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
}

} // namespace pt

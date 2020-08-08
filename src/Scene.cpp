#include "Scene.h"
#include "MathUtils.h"

namespace pt {

void Scene::addSphere(const Vec3& center, float radius) {
    spheres_.emplace_back(center, radius);
}

RayHit Scene::intersect(const Ray& ray) const {
    RayHit closestHit(-inf<float>, Vec3());
    for (const auto& sphere : spheres_) {
        RayHit hit = pt::intersect(ray, sphere);
        if (hit.t >= 0.0f && (closestHit.t < 0.0f || hit.t < closestHit.t)) {
            closestHit = hit;
        }
    }

    return closestHit;
}

} // namespace pt

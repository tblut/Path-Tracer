#include "Plane.h"

namespace pt {

Plane::Plane(const Vec3& p0_, const Vec3& normal_, const Material& material_)
    : Shape(material_)
    , p0(p0_)
    , normal(normalize(normal_))
{
}

RayHit Plane::intersect(const Ray& ray) const {
    float denom = dot(ray.direction, normal);
    if (denom > -1e-6f) {
        return rayMiss;
    }

    float tmin = dot(p0 - ray.origin, normal) / denom;
    if (tmin < 0.0f) {
        return rayMiss;
    }

    return RayHit(tmin, normal, material);
}

} // namespace pt

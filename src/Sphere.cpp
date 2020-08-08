#include "Sphere.h"

namespace pt {

Sphere::Sphere(const Vec3& center_, float radius_, const Material& material)
    : Shape(material)
    , center(center_)
    , radius(radius_)
{
}

RayHit Sphere::intersect(const Ray & ray) const {
    const float a = dot(ray.direction, ray.direction);
    const Vec3 oc = ray.origin - center;
    const float b = 2.0f * dot(ray.direction, oc);
    const float c = dot(oc, oc) - radius * radius;
    const float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) {
        return miss;
    }

    const float tmin = (-b - std::sqrt(discriminant)) / (2.0f * a);
    const Vec3 normal = (ray.at(tmin) - center) / radius;
    return RayHit(tmin, normal, material);
}

} // namespace pt

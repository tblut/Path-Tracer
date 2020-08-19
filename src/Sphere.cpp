#include "Sphere.h"
#include "MathUtils.h"
#include "BRDF.h"

namespace pt {

Sphere::Sphere(const Vec3& center_, float radius_, const Material& material_)
    : Shape(material_)
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
        return rayMiss;
    }

    const float tmin = (-b - std::sqrt(discriminant)) / (2.0f * a);
    const Vec3 normal = (ray.at(tmin) - center) / radius;
    return RayHit(tmin, normal, this);
}

Vec3 Sphere::sampleDirection(const Vec3& p, float u1, float u2, float& pdf) const {
    Vec3 w = center - p;
    const float dist = length(w);
    const float cosThetaMax = std::sqrt(1.0f - (radius * radius) / (dist * dist));
    const float theta = std::acos(1.0f - u1 + u1 * cosThetaMax);
    const float phi = 2.0f * pi<float> * u2;
    const float sinTheta = std::sin(theta);
    const Vec3 dir(
        std::cos(phi) * sinTheta,
        std::sin(phi) * sinTheta,
        std::cos(theta)
    );
    pdf = 1.0f / (2.0f * pi<float> * (1.0f - cosThetaMax));

    w /= dist;
    Vec3 b1, b2;
    makeOrthonormalBasis(w, b1, b2);
    return localToWorld(dir, b1, b2, w);
}

} // namespace pt

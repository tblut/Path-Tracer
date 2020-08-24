#include "Sphere.h"
#include "MathUtils.h"
#include "OrthonormalBasis.h"

namespace pt {

Sphere::Sphere(const Vec3& center, float radius, const Material& material)
    : Shape(material)
    , center_(center)
    , radius_(radius)
{
}

RayHit Sphere::intersect(const Ray & ray) const {
    const float a = dot(ray.direction, ray.direction);
    const Vec3 oc = ray.origin - center_;
    const float b = 2.0f * dot(ray.direction, oc);
    const float c = dot(oc, oc) - radius_ * radius_;
    const float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) {
        return rayMiss;
    }

    const float tmin = (-b - std::sqrt(discriminant)) / (2.0f * a);
    const Vec3 normal = normalize(ray.at(tmin) - center_);
    return RayHit(tmin, normal, this);
}

Vec3 Sphere::sampleDirection(const Vec3& p, float u1, float u2, float* pdf) const {
    Vec3 w = center_ - p;
    const float distSq = lengthSq(w);
    const float cosThetaMax = std::sqrt(1.0f - (radius_ * radius_) / distSq);
    const float cosTheta = 1.0f - u1 + u1 * cosThetaMax;
    const float phi = 2.0f * pi<float> * u2;
    const float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
    Vec3 dir(
        sinTheta * std::cos(phi),
        sinTheta * std::sin(phi),
        cosTheta
    );
    if (pdf) {
        *pdf = 1.0f / (2.0f * pi<float> * (1.0f - cosThetaMax));
    }

    w /= std::sqrt(distSq);
    OrthonormalBasis basis(w);
    dir = basis.localToWorld(dir);
    assert(isNormalized(dir));
    return dir;
}

float Sphere::pdf(const Vec3& p) const {
    const float cosThetaMax = std::sqrt(1.0f - (radius_ * radius_) / lengthSq(center_ - p));
    return 1.0f / (2.0f * pi<float> * (1.0f - cosThetaMax));
}

} // namespace pt

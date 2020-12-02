#include "Sphere.h"
#include "MathUtils.h"
#include "OrthonormalBasis.h"

namespace pt {

Sphere::Sphere(const Vec3& center, float radius, const Material& material)
    : Shape(material)
    , center_(center)
    , radius_(radius)
    , radiusSq_(radius * radius)
{
}

float Sphere::intersect(const Ray& ray) const {
    const float a = dot(ray.direction, ray.direction);
    const Vec3 oc = ray.origin - center_;
    const float halfB = dot(ray.direction, oc);
    const float c = dot(oc, oc) - radiusSq_;
    const float discriminant = halfB * halfB - a * c;
    if (discriminant < 0.0f) {
        return -inf<float>;
    }

    const float tmin = (-halfB - std::sqrt(discriminant)) / a;
    return tmin;
}

Vec3 Sphere::normalAt(const Vec3& p) const {
    return normalize(p - center_);
}

Vec3 Sphere::sampleDirection(const Vec3& p, float u1, float u2, float* pdf) const {
    Vec3 w = center_ - p;
    const float distSq = lengthSq(w);
    const float cosThetaMax = std::sqrt(1.0f - radiusSq_ / distSq);
    const float cosTheta = 1.0f - u1 + u1 * cosThetaMax;
    const float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
    const float phi = 2.0f * pi<float> * u2;
    Vec3 dir(
        sinTheta * std::cos(phi),
        sinTheta * std::sin(phi),
        cosTheta
    );
    if (pdf) {
        *pdf = 1.0f / (2.0f * pi<float> * (1.0f - cosThetaMax));
    }

    w *= 1.0f / std::sqrt(distSq);
    OrthonormalBasis basis(w);
    dir = basis.localToWorld(dir);
    assert(isNormalized(dir));
    return dir;
}

float Sphere::pdf(const Vec3& p) const {
    const float cosThetaMax = std::sqrt(1.0f - radiusSq_ / lengthSq(center_ - p));
    return 1.0f / (2.0f * pi<float> * (1.0f - cosThetaMax));
}

} // namespace pt

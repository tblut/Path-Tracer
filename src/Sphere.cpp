#include "Sphere.h"
#include "MathUtils.h"
#include "OrthonormalBasis.h"

namespace pt {

Sphere::Sphere(const Vec3& center, float radius, const Material& material)
    : Shape(material)
    , center_(center)
    , radius_(radius)
    , radiusSq_(radius * radius)
    , invRadius_(1.0f / radius)
{
}

RayHit Sphere::intersect(const Ray& ray) const {
    float a = dot(ray.direction, ray.direction);
    Vec3 oc = ray.origin - center_;
    float halfB = dot(ray.direction, oc);
    float c = dot(oc, oc) - radiusSq_;
    float discriminant = halfB * halfB - a * c;
    if (discriminant < 0.0f) {
        return rayMiss;
    }

    float sqrtDiscr = std::sqrt(discriminant);
    float tmin = (-halfB - sqrtDiscr) / a;
    if (tmin < 0.0f) {
        tmin = (-halfB + sqrtDiscr) / a;
    }

    if (tmin < 0.0f || tmin >= ray.tmax) {
        return rayMiss;
    }

    Vec3 normal = (ray.at(tmin) - center_) * invRadius_;
    return RayHit(tmin, normal, this);
}


BoundingBox Sphere::getWorldBounds() const {
    Vec3 radiusVec(radius_);
    return BoundingBox(center_ - radiusVec, center_ + radiusVec);
}

Vec3 Sphere::sampleDirection(const Vec3& p, float u1, float u2, float* pdf) const {
    Vec3 w = center_ - p;
    float distSq = lengthSq(w);
    float sin2ThetaMax = radiusSq_ / distSq;
    float cosThetaMax = std::sqrt(max(0.0f, 1.0f - sin2ThetaMax));
    float cosTheta = 1.0f - u1 + u1 * cosThetaMax;
    float sinTheta = std::sqrt(max(0.0f, 1.0f - cosTheta * cosTheta));
    float phi = 2.0f * pi<float> * u2;
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

float Sphere::pdf(const Vec3& p, const Vec3& wi) const {
    float sin2ThetaMax = radiusSq_ / lengthSq(center_ - p);
    float cosThetaMax = std::sqrt(max(0.0f, 1.0f - sin2ThetaMax));
    return 1.0f / (2.0f * pi<float> * (1.0f - cosThetaMax));
}

} // namespace pt

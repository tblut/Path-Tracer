#include "Triangle.h"
#include "Vector2.h"

#include <cassert>

namespace {

using namespace pt;

Vec2 sampleUniformTriangle(float u1, float u2) {
    float sqrtU1 = std::sqrt(u1);
    return Vec2(1.0f - sqrtU1, u2 * sqrtU1);
}

// See: A Low-Distortion Map Between Triangle and Square (2019), Eric Heitz
/*Vec2 sampleUniformTriangle(float u1, float u2) {
    if (u2 > u1) {
        u1 *= 0.5f;
        u2 -= u1;
    }
    else {
        u2 *= 0.5f;
        u1 -= u2;
    }
    return Vec2(u1, u2);
}
*/
float triangleArea(const Vec3& p0, const Vec3& p1, const Vec3& p2) {
    return 0.5f * length(cross(p1 - p0, p2 - p0));
}

Vec3 triangleNormal(const Vec3& p0, const Vec3& p1, const Vec3& p2) {
    return normalize(cross(p1 - p0, p2 - p0));
}

Vec2 computeBarycentricCoords(const Vec3& p, const Vec3& p0, const Vec3& p1, const Vec3& p2) {
    Vec3 v0 = p1 - p0;
    Vec3 v1 = p2 - p0;
    Vec3 v2 = p - p0;
    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float invDenom = 1.0f / (d00 * d11 - d01 * d01);
    return Vec2(
        (d11 * d20 - d01 * d21) * invDenom,
        (d00 * d21 - d01 * d20) * invDenom
    );
}

} // namespace


namespace pt {

Triangle::Triangle(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Material& material)
    : Shape(material)
    , p0_(p0)
    , p1_(p1)
    , p2_(p2)
    , normal_(triangleNormal(p0, p1, p2))
    , area_(triangleArea(p0, p1, p2))
{
}

// See: Fast Minimum Storage Ray Triangle Intersection (1997), Möller and Trumbore
float Triangle::intersect(const Ray& ray) const {
    constexpr float eps = 1e-6f;

    Vec3 e1 = p1_ - p0_;
    Vec3 e2 = p2_ - p0_;
    Vec3 p = cross(ray.direction, e2);

    float determinant = dot(p, e1);
    if (abs(determinant) < eps) {
        return -inf<float>;
    }
    float invDeterminant = 1.0f / determinant;

    Vec3 s = ray.origin - p0_;
    float u = invDeterminant * dot(p, s);
    if (u < 0.0f || u > 1.0f) {
        return -inf<float>;
    }

    Vec3 q = cross(s, e1);
    float v = invDeterminant * dot(ray.direction, q);
    if (v < 0.0f || u + v > 1.0f) {
        return -inf<float>;
    }

    float t = invDeterminant * dot(e2, q);
    return t;
}

Vec3 Triangle::normalAt(const Vec3& p) const {
    return normal_;
}

Vec3 Triangle::sampleDirection(const Vec3& p, float u1, float u2, float* pdf) const {
    Vec2 uv = sampleUniformTriangle(u1, u2);
    Vec3 q = uv.x * p0_ + uv.y * p1_ + (1.0f - uv.x - uv.y) * p2_;
    Vec3 w = normalize(q - p);

    if (pdf) {
        // TODO: Make one-sidedness optional
        float cosThetaI = dot(-w, normal_);
        if (cosThetaI <= 0.0f) {
            *pdf = 0.0f;
        }
        else {
            // pdf = 1/A * |T|^-1 = 1/A * r^2/(cos(theta)*A) = r^2/cos(theta)
            *pdf = lengthSq(q - p) / cosThetaI;
        }
    }

    return w;
}

float Triangle::pdf(const Vec3& p, const Vec3& wi) const {
    // TODO: Make one-sidedness optional
    float cosThetaI = dot(-wi, normal_);
    if (cosThetaI <= 0.0f) {
        return 0.0f;
    }

    Ray ray(p, wi);
    float t = intersect(ray);
    if (t < 0.0f) {
        return 0.0f;
    }

    float distSq = lengthSq(ray.at(t) - p);
    return distSq / (cosThetaI * area_);
}

} // namespace pt

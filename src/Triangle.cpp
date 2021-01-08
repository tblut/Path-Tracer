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

Triangle::Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Material& material)
    : Shape(material)
    , vertices_{ v0, v1, v2 }
    , area_(triangleArea(v0, v1, v2))
{
    Vec3 normal = triangleNormal(v0, v1, v2);
    normals_[0] = normal;
    normals_[1] = normal;
    normals_[2] = normal;
}

Triangle::Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2,
        const Vec3& n0, const Vec3& n1, const Vec3& n2, const Material& material)
    : Shape(material)
    , vertices_{ v0, v1, v2 }
    , normals_{ n0, n1, n2 }
    , area_(triangleArea(v0, v1, v2))
{
}

// See: Fast Minimum Storage Ray Triangle Intersection (1997), Möller and Trumbore
// TODO: Use the method from Woop et al. (2013) instead to get watertight triangle intersection
RayHit Triangle::intersect(const Ray& ray) const {
    constexpr float eps = 1e-6f;

    Vec3 e1 = vertices_[1] - vertices_[0];
    Vec3 e2 = vertices_[2] - vertices_[0];
    Vec3 p = cross(ray.direction, e2);

    float determinant = dot(p, e1);
    if (abs(determinant) < eps) {
        return rayMiss;
    }
    float invDeterminant = 1.0f / determinant;

    Vec3 s = ray.origin - vertices_[0];
    float u = invDeterminant * dot(p, s);
    if (u < 0.0f || u > 1.0f) {
        return rayMiss;
    }

    Vec3 q = cross(s, e1);
    float v = invDeterminant * dot(ray.direction, q);
    if (v < 0.0f || u + v > 1.0f) {
        return rayMiss;
    }

    float tmin = invDeterminant * dot(e2, q);
    if (tmin < 0.0f || tmin >= ray.tmax) {
        return rayMiss;
    }

    Vec3 normal = normalize((1.0f - u - v) * normals_[0] + u * normals_[1] + v * normals_[2]);
    return RayHit(tmin, normal, this);
}

BoundingBox Triangle::getWorldBounds() const {
    // Increase the size by a very small epsilon so that perfectly
    // flat dimensions are avoided which lead to false positives
    // in the ray/box intersection method.
    constexpr Vec3 eps(std::numeric_limits<float>::epsilon());
    return BoundingBox(
        min(vertices_[0], min(vertices_[1], vertices_[2])) - eps,
        max(vertices_[0], max(vertices_[1], vertices_[2])) + eps
    );
}

Vec3 Triangle::sampleDirection(const Vec3& p, float u1, float u2, float* pdf) const {
    Vec2 uv = sampleUniformTriangle(u1, u2);
    float w = (1.0f - uv.x - uv.y);
    Vec3 q = uv.x * vertices_[0] + uv.y * vertices_[1] + w * vertices_[2];
    Vec3 dir = normalize(q - p);

    if (pdf) {
        // TODO: Make one-sidedness optional
        Vec3 normal = normalize(uv.x * normals_[0] + uv.y * normals_[1] + w * normals_[2]);
        float cosThetaI = dot(-dir, normal);
        if (cosThetaI <= 0.0f) {
            *pdf = 0.0f;
        }
        else {
            // pdf = 1/A * |T|^-1 = 1/A * r^2/(cos(theta)*A) = r^2/cos(theta)
            *pdf = lengthSq(q - p) / cosThetaI;
        }
    }

    return dir;
}

float Triangle::pdf(const Vec3& p, const Vec3& wi) const {
    Ray ray(p, wi);
    RayHit hit = intersect(ray);
    if (hit.t < 0.0f) {
        return 0.0f;
    }

    // TODO: Make one-sidedness optional
    float cosThetaI = dot(-wi, hit.normal);
    if (cosThetaI <= 0.0f) {
        return 0.0f;
    }

    float distSq = lengthSq(ray.at(hit.t) - p);
    return distSq / (cosThetaI * area_);
}

} // namespace pt

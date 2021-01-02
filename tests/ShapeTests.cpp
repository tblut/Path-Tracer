#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "TestHelpers.h"
#include "BoundingBox.h"
#include "Sphere.h"
#include "Triangle.h"
#include "RandomSeries.h"
#include "BSDF.h"

#include <vector>

TEST_CASE("BoundingBox") {
    pt::BoundingBox bounds(pt::Vec3(-1.0f), pt::Vec3(1.0f));

    SECTION("getCenter") {
        REQUIRE(bounds.getCenter() == pt::ApproxVec3(0.0f, 0.0f, 0.0f));
    }

    SECTION("getExtents") {
        REQUIRE(bounds.getExtents() == pt::ApproxVec3(1.0f, 1.0f, 1.0f));
    }

    SECTION("Ray Outside Hit") {
        pt::Vec3 rayOrigin(0.0f, 0.0f, -2.0f);
        pt::Vec3 rayDirection = pt::normalize(bounds.getCenter() - rayOrigin);
        REQUIRE(pt::testIntersection(pt::Ray(rayOrigin, rayDirection), bounds));
    }

    SECTION("Ray Inside Hit") {
        pt::Vec3 rayOrigin(0.0f, 0.0f, -0.5f);
        pt::Vec3 rayDirection = pt::normalize(bounds.getCenter() - rayOrigin);
        REQUIRE(pt::testIntersection(pt::Ray(rayOrigin, rayDirection), bounds));
    }

    SECTION("Ray Infront Miss") {
        pt::Vec3 rayOrigin(0.0f, -2.0f, -2.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, -1.0f);
        REQUIRE_FALSE(pt::testIntersection(pt::Ray(rayOrigin, rayDirection), bounds));
    }

    SECTION("Ray Behind Miss") {
        pt::Vec3 rayOrigin(0.0f, -2.0f, 2.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, -1.0f);
        REQUIRE_FALSE(pt::testIntersection(pt::Ray(rayOrigin, rayDirection), bounds));
    }
}


TEST_CASE("Sphere") {
    pt::Material dummyMat(pt::Vec3(), 0.0f, 0.0f);
    pt::Vec3 sphereCenter(1.0f, 1.0f, 1.0f);
    float sphereRadius = 2.0f;
    pt::Sphere sphere(sphereCenter, sphereRadius, dummyMat);

    SECTION("Ray Outside Hit") {
        pt::Vec3 rayOrigin(1.0f, 1.0f, -2.0f);
        pt::Vec3 rayDirection = pt::normalize(sphereCenter - rayOrigin);
        float distance = pt::length(sphereCenter - rayOrigin) - sphereRadius;
        float t = sphere.intersect(pt::Ray(rayOrigin, rayDirection));
        REQUIRE(t == pt::Approx(distance));
    }

    SECTION("Ray Inside Hit") {
        pt::Vec3 rayOrigin = sphereCenter;
        pt::Vec3 rayDirection(0.0f, 0.0f, 1.0f);
        float t = sphere.intersect(pt::Ray(rayOrigin, rayDirection));
        REQUIRE(t == pt::Approx(sphereRadius));
    }

    SECTION("Ray Miss") {
        pt::Vec3 rayOrigin(-3.0f, 1.0f, -2.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, 1.0f);
        float t = sphere.intersect(pt::Ray(rayOrigin, rayDirection));
        REQUIRE(t < 0.0f);
    }

    SECTION("Ray Behind Sphere Miss") {
        pt::Vec3 rayOrigin(1.0f, 1.0f, 4.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, 1.0f);
        float t = sphere.intersect(pt::Ray(rayOrigin, rayDirection));
        REQUIRE(t < 0.0f);
    }

    SECTION("Normal Computation") {
        pt::Vec3 normal = sphere.normalAt(sphereCenter + pt::Vec3(sphereRadius, 0.0f, 0.0f));
        REQUIRE(normal == pt::ApproxVec3(1.0f, 0.0f, 0.0f));
    }

    SECTION("Direction Sampling") {
        pt::RandomSeries rng;
        pt::Vec3 p(0.0f, -1.0f, 0.0f);

        for (int i = 0; i < 1000000; i++) {
            float pdf;
            pt::Vec3 dir = sphere.sampleDirection(p,
                rng.uniformFloat(), rng.uniformFloat(), &pdf);
            REQUIRE(pdf >= 0.0f);

            float distance = pt::length(sphereCenter - p) - sphereRadius;
            float t = sphere.intersect(pt::Ray(p, dir));
            REQUIRE(t >= distance - pt::testEps<float>);
        }
    }
}


TEST_CASE("Triangle") {
    pt::Material dummyMat(pt::Vec3(), 0.0f, 0.0f);
    pt::Triangle triangle(
        pt::Vec3(-1.0f, -1.0f, 0.0f),
        pt::Vec3(1.0f, -1.0f, 0.0f),
        pt::Vec3(0.0f, 1.0f, 0.0f),
        dummyMat);

    SECTION("Ray Front Hit") {
        pt::Vec3 rayOrigin(0.0f, 0.0f, 2.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, -1.0f);
        float distance = pt::length(rayOrigin);
        float t = triangle.intersect(pt::Ray(rayOrigin, rayDirection));
        REQUIRE(t == pt::Approx(distance));
    }

    SECTION("Ray Back Hit") {
        pt::Vec3 rayOrigin(0.0f, 0.0f, -2.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, 1.0f);
        float distance = pt::length(rayOrigin);
        float t = triangle.intersect(pt::Ray(rayOrigin, rayDirection));
        REQUIRE(t == pt::Approx(distance));
    }

    SECTION("Ray Miss") {
        pt::Vec3 rayOrigin(1.0f, 1.0f, 2.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, -1.0f);
        float t = triangle.intersect(pt::Ray(rayOrigin, rayDirection));
        REQUIRE(t < 0.0f);
    }

    SECTION("Ray Behind Miss") {
        pt::Vec3 rayOrigin(1.0f, 1.0f, -2.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, -1.0f);
        float t = triangle.intersect(pt::Ray(rayOrigin, rayDirection));
        REQUIRE(t < 0.0f);
    }

    SECTION("Normal Computation") {
        pt::Vec3 normal = triangle.normalAt(pt::Vec3(0.0f));
        REQUIRE(normal == pt::ApproxVec3(0.0f, 0.0f, 1.0f));
    }
}


TEST_CASE("Triangle Watertight") {
    constexpr float radius = 100.0f;
    constexpr size_t numSlices = 128;

    std::vector<pt::Vec3> vertices;
    vertices.reserve(numSlices + 1);
    vertices.emplace_back(0.0f, 0.0f, 0.0f);
    for (size_t slice = 0; slice < numSlices; slice++) {
        float phi = 2.0f * pt::pi<float> / numSlices * slice;
        vertices.emplace_back(std::cos(phi) * radius, std::sin(phi) * radius, 0.0f);
    }

    pt::Material dummyMat(pt::Vec3(), 0.0f, 0.0f);
    std::vector<pt::Triangle> triangles;
    for (size_t slice = 0; slice < numSlices; slice++) {
        pt::Vec3 p0 = vertices[0];
        pt::Vec3 p1 = vertices[1 + slice];
        pt::Vec3 p2 = vertices[1 + (slice + 1) % numSlices];
        triangles.emplace_back(p0, p1, p2, dummyMat);
    }

    pt::RandomSeries rng;
    for (size_t i = 0; i < 1000000; i++) {
        pt::Vec3 rayOrigin = pt::sampleCosineHemisphere(rng.uniformFloat(), rng.uniformFloat());
        rayOrigin *= 1.0f + rng.uniformFloat() * radius * 2.0f;

        pt::Vec3 vertex = vertices[static_cast<size_t>(rng.uniformFloat() * vertices.size())];
        pt::Ray ray(rayOrigin, pt::normalize(vertex - rayOrigin));

        size_t numIntersections = 0;
        for (const auto& triangle : triangles) {
            if (triangle.intersect(ray) >= 0.0f) {
                numIntersections++;
            }
        }
        REQUIRE(numIntersections >= 1);
    }
}

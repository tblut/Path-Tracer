#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "TestHelpers.h"
#include "Sphere.h"
#include "Triangle.h"
#include "RandomSeries.h"

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

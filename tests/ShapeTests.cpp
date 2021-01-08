#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "TestHelpers.h"
#include "BoundingBox.h"
#include "Sphere.h"
#include "Triangle.h"
#include "BVH.h"
#include "RandomSeries.h"
#include "BSDF.h"

#include <vector>

TEST_CASE("BoundingBox") {
    pt::RandomSeries rng;
    pt::Vec3 extents(1.0f);
    pt::Vec3 flatExtents(1.0f, 0.0f, 1.0f);
    pt::BoundingBox bounds(-extents, extents);
    pt::BoundingBox boundsFlat(-flatExtents, flatExtents);

    // The triangle bounds method adds epsilon to the min and max, so
    // emulate that here as well
    constexpr pt::Vec3 eps(std::numeric_limits<float>::epsilon());
    bounds.min -= eps;
    bounds.max += eps;
    boundsFlat.min -= eps;
    boundsFlat.max += eps;

    SECTION("Center") {
        REQUIRE(bounds.getCenter() == pt::ApproxVec3(0.0f, 0.0f, 0.0f));
        REQUIRE(boundsFlat.getCenter() == pt::ApproxVec3(0.0f, 0.0f, 0.0f));
    }

    SECTION("Extents") {
        REQUIRE(bounds.getExtents() == pt::ApproxVec3(1.0f, 1.0f, 1.0f));
        REQUIRE(boundsFlat.getExtents() == pt::ApproxVec3(1.0f, 0.0f, 1.0f));
    }

    SECTION("Surface Area") {
        REQUIRE(bounds.getSurfaceArea() == pt::Approx(24.0f));
        REQUIRE(boundsFlat.getSurfaceArea() == pt::Approx(8.0f));
    }

    SECTION("Ray Outside Hit") {
        for (const pt::BoundingBox& box : { bounds, boundsFlat }) {
            for (int i = 0; i < 1000000; i++) {
                pt::Vec3 rayOrigin = pt::sampleUniformSphere(rng.uniformFloat(), rng.uniformFloat());
                rayOrigin *= 1.0f + rng.uniformFloat() * 10000.0f;

                pt::Vec3 target = pt::sampleUniformSphere(rng.uniformFloat(), rng.uniformFloat());
                target *= box.getExtents() * rng.uniformFloat();

                pt::Vec3 rayDirection = pt::normalize(target - rayOrigin);
                REQUIRE(pt::testIntersection(pt::Ray(rayOrigin, rayDirection), pt::Vec3(1.0f) / rayDirection, box));
            }
        }
    }

    SECTION("Ray Outside Parallel Edge Hit") {
        for (const pt::BoundingBox* box : { &bounds, &boundsFlat }) {
            pt::Vec3 extent = (box == &bounds) ? extents : flatExtents;

            for (int i = 0; i < 1000000; i++) {
                bool flip = rng.uniformFloat() < 0.5f;
                int axis = static_cast<int>(rng.uniformFloat() * 3.0f);
                pt::Vec3 normal(0.0f);
                normal[axis] = flip ? -1.0f : 1.0f;

                pt::Vec3 rayDirection = -normal;
                pt::Vec3 rayOrigin = box->getCenter();
                rayOrigin += rng.uniformFloat() * 10000.0f * normal;

                float u = rng.uniformFloat();
                int slideAxis = (u < 0.5f) ? (axis + 1) % 3 : (axis + 2) % 3;
                int sideAxis = (u < 0.5f) ? (axis + 2) % 3 : (axis + 1) % 3;
                pt::Vec3 slideDir(0.0f);
                slideDir[slideAxis] = 1.0f;
                float slideAmount = pt::remap(rng.uniformFloat(), 0.0f, pt::oneMinusEpsilon<float>, -0.5f, 0.5f);
                rayOrigin += extent * slideDir * slideAmount;
                rayOrigin[sideAxis] = (rng.uniformFloat() < 0.5f) ? extent[sideAxis] : -extent[sideAxis];

                REQUIRE(pt::testIntersection(pt::Ray(rayOrigin, rayDirection), pt::Vec3(1.0f) / rayDirection, *box));
            }
        }
    }

    SECTION("Ray Inside Hit") {
        for (const pt::BoundingBox& box : { bounds, boundsFlat }) {
            for (int i = 0; i < 1000000; i++) {
                pt::Vec3 rayDirection = pt::sampleUniformSphere(rng.uniformFloat(), rng.uniformFloat());

                pt::Vec3 rayOrigin = pt::sampleUniformSphere(rng.uniformFloat(), rng.uniformFloat());
                rayOrigin *= box.getExtents() * rng.uniformFloat();

                REQUIRE(pt::testIntersection(pt::Ray(rayOrigin, rayDirection), pt::Vec3(1.0f) / rayDirection, box));
            }
        }
    }

    SECTION("Ray Miss") {
        for (bool front : { true, false }) {
            for (int i = 0; i < 6; i++) {
                int dirAxis = i / 2;
                pt::Vec3 rayOrigin(0.0f);
                rayOrigin[dirAxis] = (i % 2) == 0 ? 2.0f : -2.0f;

                pt::Vec3 rayDirection(0.0f);
                rayDirection[dirAxis] = (i % 2) == 0 ? -1.0f : 1.0f;
                if (!front) {
                    rayDirection[dirAxis] *= -1.0f;
                }

                for (int k = 0; k < 4; k++) {
                    int offsetAxis = (dirAxis + 1 + k / 2) % 3;
                    rayOrigin[offsetAxis] = (k % 2) == 0 ? -4.0f : 4.0f;

                    REQUIRE_FALSE(pt::testIntersection(pt::Ray(rayOrigin, rayDirection), pt::Vec3(1.0f) / rayDirection, bounds));
                    REQUIRE_FALSE(pt::testIntersection(pt::Ray(rayOrigin, rayDirection), pt::Vec3(1.0f) / rayDirection, boundsFlat));
                }
            }
        }
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
        float t = sphere.intersect(pt::Ray(rayOrigin, rayDirection)).t;
        REQUIRE(t == pt::Approx(distance));
    }

    SECTION("Ray Inside Hit") {
        pt::Vec3 rayOrigin = sphereCenter;
        pt::Vec3 rayDirection(0.0f, 0.0f, 1.0f);
        float t = sphere.intersect(pt::Ray(rayOrigin, rayDirection)).t;
        REQUIRE(t == pt::Approx(sphereRadius));
    }

    SECTION("Ray Miss") {
        pt::Vec3 rayOrigin(-3.0f, 1.0f, -2.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, 1.0f);
        REQUIRE_FALSE(sphere.intersect(pt::Ray(rayOrigin, rayDirection)));
    }

    SECTION("Ray Behind Sphere Miss") {
        pt::Vec3 rayOrigin(1.0f, 1.0f, 4.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, 1.0f);
        REQUIRE_FALSE(sphere.intersect(pt::Ray(rayOrigin, rayDirection)));
    }

    SECTION("Normal Computation") {
        pt::Ray ray(sphereCenter + pt::Vec3(4.0f, 0.0f, 0.0f), pt::Vec3(-1.0f, 0.0f, 0.0f));
        pt::RayHit hit = sphere.intersect(ray);
        REQUIRE(hit.normal == pt::ApproxVec3(1.0f, 0.0f, 0.0f));
    }

    SECTION("World Bounds") {
        pt::BoundingBox box = sphere.getWorldBounds();
        REQUIRE(box.min == pt::ApproxVec3(-1.0f, -1.0f, -1.0f));
        REQUIRE(box.max == pt::ApproxVec3(3.0f, 3.0f, 3.0f));
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
            float t = sphere.intersect(pt::Ray(p, dir)).t;
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
        dummyMat
    );

    SECTION("Ray Front Hit") {
        pt::Vec3 rayOrigin(0.0f, 0.0f, 2.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, -1.0f);
        float distance = pt::length(rayOrigin);
        float t = triangle.intersect(pt::Ray(rayOrigin, rayDirection)).t;
        REQUIRE(t == pt::Approx(distance));
    }

    SECTION("Ray Back Hit") {
        pt::Vec3 rayOrigin(0.0f, 0.0f, -2.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, 1.0f);
        float distance = pt::length(rayOrigin);
        float t = triangle.intersect(pt::Ray(rayOrigin, rayDirection)).t;
        REQUIRE(t == pt::Approx(distance));
    }

    SECTION("Ray Miss") {
        pt::Vec3 rayOrigin(1.0f, 1.0f, 2.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, -1.0f);
        REQUIRE_FALSE(triangle.intersect(pt::Ray(rayOrigin, rayDirection)));
    }

    SECTION("Ray Behind Miss") {
        pt::Vec3 rayOrigin(1.0f, 1.0f, -2.0f);
        pt::Vec3 rayDirection(0.0f, 0.0f, -1.0f);
        REQUIRE_FALSE(triangle.intersect(pt::Ray(rayOrigin, rayDirection)));
    }

    SECTION("Normal Computation Flat") {
        pt::Ray ray(pt::Vec3(0.0f, 0.0f, 3.0f), pt::Vec3(0.0f, 0.0f, -1.0f));
        pt::RayHit hit = triangle.intersect(ray);
        REQUIRE(hit.normal == pt::ApproxVec3(0.0f, 0.0f, 1.0f));
    }

    SECTION("Normal Computation Smooth") {
        pt::Triangle smoothTriangle(
            pt::Vec3(-1.0f, -1.0f, 0.0f),
            pt::Vec3(1.0f, -1.0f, 0.0f),
            pt::Vec3(0.0f, 1.0f, 0.0f),
            pt::normalize(pt::Vec3(-1.0f, -1.0f, 1.0f)),
            pt::normalize(pt::Vec3(1.0f, -1.0f, 1.0f)),
            pt::normalize(pt::Vec3(0.0f, 1.0f, 1.0f)),
            dummyMat
        );

        pt::Vec3 center = (smoothTriangle.getVertex(0) + smoothTriangle.getVertex(1)
            + smoothTriangle.getVertex(2)) / 3.0f;
        pt::Vec3 origin(0.0f, 0.0f, 3.0f);
        pt::Ray ray(origin, pt::normalize(center - origin));
        pt::RayHit hit = smoothTriangle.intersect(ray);

        pt::Vec3 expectedNormal = pt::normalize(smoothTriangle.getNormal(0)
            + smoothTriangle.getNormal(1) + smoothTriangle.getNormal(2));
        REQUIRE(hit.normal == pt::ApproxVec3(expectedNormal.x, expectedNormal.y, expectedNormal.z));
    }

    SECTION("World Bounds") {
        pt::BoundingBox box = triangle.getWorldBounds();
        REQUIRE(box.min == pt::ApproxVec3(-1.0f, -1.0f, 0.0f));
        REQUIRE(box.max == pt::ApproxVec3(1.0f, 1.0f, 0.0f));
    }

    /*SECTION("Watertight") {
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
    }*/
}


TEST_CASE("Bounding Volume Hierarchy") {
    pt::Material dummyMat(pt::Vec3(), 0.0f, 0.0f);
    std::vector<pt::Sphere> spheres;
    spheres.emplace_back(pt::Vec3(-10.0f, 0.0f, 0.0f), 1.0f, dummyMat);
    spheres.emplace_back(pt::Vec3(-2.0f, 0.0f, 0.0f), 0.9f, dummyMat);
    spheres.emplace_back(pt::Vec3(-1.0f, 0.0f, 0.0f), 1.0f, dummyMat);
    spheres.emplace_back(pt::Vec3(0.0f, 0.0f, 0.0f), 1.3f, dummyMat);
    spheres.emplace_back(pt::Vec3(1.0f, 0.0f, 0.0f), 1.2f, dummyMat);
    spheres.emplace_back(pt::Vec3(2.0f, 0.0f, 0.0f), 1.0f, dummyMat);
    spheres.emplace_back(pt::Vec3(20.0f, 0.0f, 0.0f), 1.0f, dummyMat);

    std::vector<const pt::Shape*> shapes;
    for (const auto& sphere : spheres) {
        shapes.push_back(&sphere);
    }

    SECTION("Total Number of Shapes") {
        for (int maxShapesPerLeaf = 1; maxShapesPerLeaf < 3; maxShapesPerLeaf++) {
            pt::BVH bvh(shapes, maxShapesPerLeaf);
            size_t numShapes = 0;
            bvh.traverse([&](const pt::BVH::LinearNode& node) {
                numShapes += node.numShapes;
                return true;
            });
            REQUIRE(numShapes == spheres.size());
        }
    }

    SECTION("Number of Shapes in Leafs") {
        for (int maxShapesPerLeaf = 1; maxShapesPerLeaf < 3; maxShapesPerLeaf++) {
            pt::BVH bvh(shapes, maxShapesPerLeaf);
            bvh.traverse([&](const pt::BVH::LinearNode& node) {
                if (node.isLeaf()) {
                    REQUIRE(node.numShapes <= maxShapesPerLeaf);
                }
                else {
                    REQUIRE(node.numShapes == 0);
                }
                return true;
            });
        }
    }

    SECTION("Direct Intersection") {
        for (int maxShapesPerLeaf = 1; maxShapesPerLeaf < 3; maxShapesPerLeaf++) {
            pt::BVH bvh(shapes, maxShapesPerLeaf);
            for (const auto& sphere : spheres) {
                pt::Vec3 target = sphere.getCenter();
                pt::Vec3 origin(target.x, target.y, 5.0f);
                pt::Ray ray(origin, pt::normalize(target - origin));

                pt::RayHit hit = bvh.intersect(ray);
                REQUIRE(hit.shape == &sphere);
            }
        }
    }

    SECTION("Closest Intersection") {
        for (int maxShapesPerLeaf = 1; maxShapesPerLeaf < 3; maxShapesPerLeaf++) {
            pt::BVH bvh(shapes, maxShapesPerLeaf);
            for (const auto& sphere : spheres) {
                pt::Vec3 target = sphere.getCenter();
                pt::Vec3 origin(22.0f, target.y, target.z);
                pt::Ray ray(origin, pt::normalize(target - origin));

                pt::RayHit hit = bvh.intersect(ray);
                REQUIRE(hit.shape == &spheres.back());
            }
        }
    }
}

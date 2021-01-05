#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "TestHelpers.h"
#include "MathUtils.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

// Functions
TEST_CASE("min") {
    CHECK(pt::min(-1.0f, 1.0f) == pt::Approx(-1.0f));
    CHECK(pt::min(pt::Vec2(1.0f, 2.0f), pt::Vec2(2.0f, 1.0f)) == pt::ApproxVec2(1.0f, 1.0f));
    CHECK(pt::min(pt::Vec3(1.0f, 3.0f, 2.0f), pt::Vec3(3.0f, 2.0f, 1.0f)) == pt::ApproxVec3(1.0f, 2.0f, 1.0f));
    CHECK(pt::min(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f), pt::Vec4(4.0f, 3.0f, 2.0f, 1.0f)) == pt::ApproxVec4(1.0f, 2.0f, 2.0f, 1.0f));
}
TEST_CASE("max") {
    CHECK(pt::max(-1.0f, 1.0f) == pt::Approx(1.0f));
    CHECK(pt::max(pt::Vec2(1.0f, 2.0f), pt::Vec2(2.0f, 1.0f)) == pt::ApproxVec2(2.0f, 2.0f));
    CHECK(pt::max(pt::Vec3(1.0f, 3.0f, 2.0f), pt::Vec3(3.0f, 2.0f, 1.0f)) == pt::ApproxVec3(3.0f, 3.0f, 2.0f));
    CHECK(pt::max(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f), pt::Vec4(4.0f, 3.0f, 2.0f, 1.0f)) == pt::ApproxVec4(4.0f, 3.0f, 3.0f, 4.0f));
}

TEST_CASE("minComponent") {
    CHECK(pt::minComponent(pt::Vec2(1.0f, 2.0f)) == pt::Approx(1.0f));
    CHECK(pt::minComponent(pt::Vec2(2.0f, 1.0f)) == pt::Approx(1.0f));

    CHECK(pt::minComponent(pt::Vec3(1.0f, 2.0f, 3.0f)) == pt::Approx(1.0f));
    CHECK(pt::minComponent(pt::Vec3(1.0f, 3.0f, 2.0f)) == pt::Approx(1.0f));
    CHECK(pt::minComponent(pt::Vec3(3.0f, 2.0f, 1.0f)) == pt::Approx(1.0f));

    CHECK(pt::minComponent(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == pt::Approx(1.0f));
    CHECK(pt::minComponent(pt::Vec4(1.0f, 2.0f, 4.0f, 3.0f)) == pt::Approx(1.0f));
    CHECK(pt::minComponent(pt::Vec4(1.0f, 4.0f, 3.0f, 2.0f)) == pt::Approx(1.0f));
    CHECK(pt::minComponent(pt::Vec4(4.0f, 2.0f, 3.0f, 1.0f)) == pt::Approx(1.0f));
}
TEST_CASE("maxComponent") {
    CHECK(pt::maxComponent(pt::Vec2(1.0f, 2.0f)) == pt::Approx(2.0f));
    CHECK(pt::maxComponent(pt::Vec2(2.0f, 1.0f)) == pt::Approx(2.0f));

    CHECK(pt::maxComponent(pt::Vec3(1.0f, 2.0f, 3.0f)) == pt::Approx(3.0f));
    CHECK(pt::maxComponent(pt::Vec3(1.0f, 3.0f, 2.0f)) == pt::Approx(3.0f));
    CHECK(pt::maxComponent(pt::Vec3(3.0f, 2.0f, 1.0f)) == pt::Approx(3.0f));

    CHECK(pt::maxComponent(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == pt::Approx(4.0f));
    CHECK(pt::maxComponent(pt::Vec4(1.0f, 2.0f, 4.0f, 3.0f)) == pt::Approx(4.0f));
    CHECK(pt::maxComponent(pt::Vec4(1.0f, 4.0f, 3.0f, 2.0f)) == pt::Approx(4.0f));
    CHECK(pt::maxComponent(pt::Vec4(4.0f, 2.0f, 3.0f, 1.0f)) == pt::Approx(4.0f));
}

TEST_CASE("minDimension") {
    CHECK(pt::minDimension(pt::Vec2(1.0f, 2.0f)) == 0);
    CHECK(pt::minDimension(pt::Vec2(2.0f, 1.0f)) == 1);

    CHECK(pt::minDimension(pt::Vec3(1.0f, 2.0f, 3.0f)) == 0);
    CHECK(pt::minDimension(pt::Vec3(3.0f, 1.0f, 2.0f)) == 1);
    CHECK(pt::minDimension(pt::Vec3(2.0f, 3.0f, 1.0f)) == 2);

    CHECK(pt::minDimension(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == 0);
    CHECK(pt::minDimension(pt::Vec4(4.0f, 1.0f, 2.0f, 3.0f)) == 1);
    CHECK(pt::minDimension(pt::Vec4(3.0f, 4.0f, 1.0f, 2.0f)) == 2);
    CHECK(pt::minDimension(pt::Vec4(2.0f, 3.0f, 4.0f, 1.0f)) == 3);
}
TEST_CASE("maxDimension") {
    CHECK(pt::maxDimension(pt::Vec2(1.0f, 2.0f)) == 1);
    CHECK(pt::maxDimension(pt::Vec2(2.0f, 1.0f)) == 0);

    CHECK(pt::maxDimension(pt::Vec3(1.0f, 2.0f, 3.0f)) == 2);
    CHECK(pt::maxDimension(pt::Vec3(3.0f, 1.0f, 2.0f)) == 0);
    CHECK(pt::maxDimension(pt::Vec3(1.0f, 3.0f, 1.0f)) == 1);

    CHECK(pt::maxDimension(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == 3);
    CHECK(pt::maxDimension(pt::Vec4(4.0f, 1.0f, 2.0f, 3.0f)) == 0);
    CHECK(pt::maxDimension(pt::Vec4(3.0f, 4.0f, 1.0f, 2.0f)) == 1);
    CHECK(pt::maxDimension(pt::Vec4(2.0f, 3.0f, 4.0f, 1.0f)) == 2);
}

TEST_CASE("clamp") {
    CHECK(pt::clamp(-1.0f, 1.5f, 1.75f) == pt::Approx(1.5f));
    CHECK(pt::clamp(1.6f, 1.5f, 1.75f) == pt::Approx(1.6f));
    CHECK(pt::clamp(3.0f, 1.5f, 1.75f) == pt::Approx(1.75f));

    CHECK(pt::clamp(pt::Vec2(-1.0f, -2.0f), 1.5f, 1.75f) == pt::ApproxVec2(1.5f, 1.5f));
    CHECK(pt::clamp(pt::Vec2(1.0f, 0.9f), 1.5f, 1.75f) == pt::ApproxVec2(1.5f, 1.5f));
    CHECK(pt::clamp(pt::Vec2(1.6f, 1.7f), 1.5f, 1.75f) == pt::ApproxVec2(1.6f, 1.7f));
    CHECK(pt::clamp(pt::Vec2(2.0f, 3.0f), 1.5f, 1.75f) == pt::ApproxVec2(1.75f, 1.75f));

    CHECK(pt::clamp(pt::Vec3(-1.0f, -2.0f, -3.0f), 1.5f, 1.75f) == pt::ApproxVec3(1.5f, 1.5f, 1.5f));
    CHECK(pt::clamp(pt::Vec3(1.0f, 1.0f, 1.0f), 1.5f, 1.75f) == pt::ApproxVec3(1.5f, 1.5f, 1.5f));
    CHECK(pt::clamp(pt::Vec3(1.6f, 1.7f, 1.72f), 1.5f, 1.75f) == pt::ApproxVec3(1.6f, 1.7f, 1.72f));
    CHECK(pt::clamp(pt::Vec3(2.0f, 3.0f, 4.0f), 1.5f, 1.75f) == pt::ApproxVec3(1.75f, 1.75f, 1.75f));

    CHECK(pt::clamp(pt::Vec4(-1.0f, -1.0f, -1.0f, -1.0f), 1.5f, 1.75f) == pt::ApproxVec4(1.5f, 1.5f, 1.5f, 1.5f));
    CHECK(pt::clamp(pt::Vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.5f, 1.75f) == pt::ApproxVec4(1.5f, 1.5f, 1.5f, 1.5f));
    CHECK(pt::clamp(pt::Vec4(1.6f, 1.7f, 1.72f, 1.74f), 1.5f, 1.75f) == pt::ApproxVec4(1.6f, 1.7f, 1.72f, 1.74f));
    CHECK(pt::clamp(pt::Vec4(2.0f, 3.0f, 4.0f, 5.0f), 1.5f, 1.75f) == pt::ApproxVec4(1.75f, 1.75f, 1.75f, 1.75f));
}

TEST_CASE("saturate") {
    CHECK(pt::saturate(-1.0f) == pt::Approx(0.0f));
    CHECK(pt::saturate(0.6f) == pt::Approx(0.6f));
    CHECK(pt::saturate(2.0f) == pt::Approx(1.0f));

    CHECK(pt::saturate(pt::Vec2(-1.0f, -2.0f)) == pt::ApproxVec2(0.0f, 0.0f));
    CHECK(pt::saturate(pt::Vec2(0.6f, 0.7f)) == pt::ApproxVec2(0.6f, 0.7f));
    CHECK(pt::saturate(pt::Vec2(1.0f, 2.0f)) == pt::ApproxVec2(1.0f, 1.0f));

    CHECK(pt::saturate(pt::Vec3(-1.0f, -2.0f, -3.0f)) == pt::ApproxVec3(0.0f, 0.0f, 0.0f));
    CHECK(pt::saturate(pt::Vec3(0.6f, 0.7f, 0.8f)) == pt::ApproxVec3(0.6f, 0.7f, 0.8f));
    CHECK(pt::saturate(pt::Vec3(1.0f, 2.0f, 3.0f)) == pt::ApproxVec3(1.0f, 1.0f, 1.0f));

    CHECK(pt::saturate(pt::Vec4(-1.0f, -2.0f, -3.0f, -4.0f)) == pt::ApproxVec4(0.0f, 0.0f, 0.0f, 0.0f));
    CHECK(pt::saturate(pt::Vec4(0.6f, 0.7f, 0.8f, 0.9f)) == pt::ApproxVec4(0.6f, 0.7f, 0.8f, 0.9f));
    CHECK(pt::saturate(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == pt::ApproxVec4(1.0f, 1.0f, 1.0f, 1.0f));
}

TEST_CASE("abs") {
    CHECK(pt::abs(-1.0f) == pt::Approx(1.0f));
    CHECK(pt::abs(1.0f) == pt::Approx(1.0f));

    CHECK(pt::abs(pt::Vec2(1.0f, 2.0f)) == pt::ApproxVec2(1.0f, 2.0f));
    CHECK(pt::abs(pt::Vec2(-1.0f, -2.0f)) == pt::ApproxVec2(1.0f, 2.0f));

    CHECK(pt::abs(pt::Vec3(1.0f, 2.0f, 3.0f)) == pt::ApproxVec3(1.0f, 2.0f, 3.0f));
    CHECK(pt::abs(pt::Vec3(-1.0f, -2.0f, -3.0f)) == pt::ApproxVec3(1.0f, 2.0f, 3.0f));

    CHECK(pt::abs(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == pt::ApproxVec4(1.0f, 2.0f, 3.0f, 4.0f));
    CHECK(pt::abs(pt::Vec4(-1.0f, -2.0f, -3.0f, -4.0f)) == pt::ApproxVec4(1.0f, 2.0f, 3.0f, 4.0f));
}

TEST_CASE("floor") {
    CHECK(pt::floor(-1.0f) == pt::Approx(-1.0f));
    CHECK(pt::floor(1.0f) == pt::Approx(1.0f));
    CHECK(pt::floor(-1.999f) == pt::Approx(-2.0f));
    CHECK(pt::floor(2.999f) == pt::Approx(2.0f));

    CHECK(pt::floor(pt::Vec2(-1.0f, -2.0f)) == pt::ApproxVec2(-1.0f, -2.0f));
    CHECK(pt::floor(pt::Vec2(1.0f, 2.0f)) == pt::ApproxVec2(1.0f, 2.0f));
    CHECK(pt::floor(pt::Vec2(-1.4f, -2.999f)) == pt::ApproxVec2(-2.0f, -3.0f));
    CHECK(pt::floor(pt::Vec2(1.4f, 2.999f)) == pt::ApproxVec2(1.0f, 2.0f));

    CHECK(pt::floor(pt::Vec3(-1.0f, -2.0f, -3.0f)) == pt::ApproxVec3(-1.0f, -2.0f, -3.0f));
    CHECK(pt::floor(pt::Vec3(1.0f, 2.0f, 3.0f)) == pt::ApproxVec3(1.0f, 2.0f, 3.0f));
    CHECK(pt::floor(pt::Vec3(-1.4f, -2.999f, -3.645f)) == pt::ApproxVec3(-2.0f, -3.0f, -4.0f));
    CHECK(pt::floor(pt::Vec3(1.4f, 2.999f, 3.645f)) == pt::ApproxVec3(1.0f, 2.0f, 3.0f));

    CHECK(pt::floor(pt::Vec4(-1.0f, -2.0f, -3.0f, -4.0f)) == pt::ApproxVec4(-1.0f, -2.0f, -3.0f, -4.0f));
    CHECK(pt::floor(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == pt::ApproxVec4(1.0f, 2.0f, 3.0f, 4.0f));
    CHECK(pt::floor(pt::Vec4(-1.4f, -2.999f, -3.645f, -4.031f)) == pt::ApproxVec4(-2.0f, -3.0f, -4.0f, -5.0f));
    CHECK(pt::floor(pt::Vec4(1.4f, 2.999f, 3.645f, 4.031f)) == pt::ApproxVec4(1.0f, 2.0f, 3.0f, 4.0f));
}

TEST_CASE("ceil") {
    CHECK(pt::ceil(-1.0f) == pt::Approx(-1.0f));
    CHECK(pt::ceil(1.0f) == pt::Approx(1.0f));
    CHECK(pt::ceil(-1.999f) == pt::Approx(-1.0f));
    CHECK(pt::ceil(2.999f) == pt::Approx(3.0f));

    CHECK(pt::ceil(pt::Vec2(-1.0f, -2.0f)) == pt::ApproxVec2(-1.0f, -2.0f));
    CHECK(pt::ceil(pt::Vec2(1.0f, 2.0f)) == pt::ApproxVec2(1.0f, 2.0f));
    CHECK(pt::ceil(pt::Vec2(-1.4f, -2.999f)) == pt::ApproxVec2(-1.0f, -2.0f));
    CHECK(pt::ceil(pt::Vec2(1.4f, 2.999f)) == pt::ApproxVec2(2.0f, 3.0f));

    CHECK(pt::ceil(pt::Vec3(-1.0f, -2.0f, -3.0f)) == pt::ApproxVec3(-1.0f, -2.0f, -3.0f));
    CHECK(pt::ceil(pt::Vec3(1.0f, 2.0f, 3.0f)) == pt::ApproxVec3(1.0f, 2.0f, 3.0f));
    CHECK(pt::ceil(pt::Vec3(-1.4f, -2.999f, -3.645f)) == pt::ApproxVec3(-1.0f, -2.0f, -3.0f));
    CHECK(pt::ceil(pt::Vec3(1.4f, 2.999f, 3.645f)) == pt::ApproxVec3(2.0f, 3.0f, 4.0f));

    CHECK(pt::ceil(pt::Vec4(-1.0f, -2.0f, -3.0f, -4.0f)) == pt::ApproxVec4(-1.0f, -2.0f, -3.0f, -4.0f));
    CHECK(pt::ceil(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == pt::ApproxVec4(1.0f, 2.0f, 3.0f, 4.0f));
    CHECK(pt::ceil(pt::Vec4(-1.4f, -2.999f, -3.645f, -4.031f)) == pt::ApproxVec4(-1.0f, -2.0f, -3.0f, -4.0f));
    CHECK(pt::ceil(pt::Vec4(1.4f, 2.999f, 3.645f, 4.031f)) == pt::ApproxVec4(2.0f, 3.0f, 4.0f, 5.0f));
}

TEST_CASE("lerp") {
    CHECK(pt::lerp(1.0f, 3.0f, 0.5f) == pt::Approx(2.0f));
    CHECK(pt::lerp(pt::Vec2(1.0f, 2.0f), pt::Vec2(2.0f, 4.0f), 0.5f) == pt::ApproxVec2(1.5f, 3.0f));
    CHECK(pt::lerp(pt::Vec3(1.0f, 2.0f, 3.0f), pt::Vec3(2.0f, 4.0f, 6.0f), 0.5f) == pt::ApproxVec3(1.5f, 3.0f, 4.5f));
    CHECK(pt::lerp(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f), pt::Vec4(2.0f, 4.0f, 6.0f, 8.0f), 0.5f) == pt::ApproxVec4(1.5f, 3.0f, 4.5f, 6.0f));
}

TEST_CASE("remap") {
    CHECK(pt::remap(0.0f, 0.0f, 1.0f, -1.0f, 1.0f) == pt::Approx(-1.0f));
    CHECK(pt::remap(0.0f, -1.0f, 1.0f, 0.0f, 1.0f) == pt::Approx(0.5f));

    float remapped = pt::remap(
        pt::oneMinusEpsilon<float>,
        -1.0f, pt::oneMinusEpsilon<float>,
        0.0f, pt::oneMinusEpsilon<float>);
    CHECK(remapped == pt::Approx(pt::oneMinusEpsilon<float>));
}

TEST_CASE("degrees") {
    CHECK(pt::degrees(0.0f) == pt::Approx(0.0f));
    CHECK(pt::degrees(pt::pi<float>) == pt::Approx(180.0f));
}

TEST_CASE("radians") {
    CHECK(pt::radians(0.0f) == pt::Approx(0.0f));
    CHECK(pt::radians(180.0f) == pt::Approx(pt::pi<float>));
}


TEST_CASE("Vector Constructors and Operators") {
    auto a2 = pt::Vec2(1.0f, 2.0f);
    auto b2 = pt::Vec2(3.0f, 5.0f);

    auto a3 = pt::Vec3(1.0f, 2.0f, 3.0f);
    auto b3 = pt::Vec3(3.0f, 5.0f, 7.0f);

    auto a4 = pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f);
    auto b4 = pt::Vec4(5.0f, 7.0f, 9.0f, 11.0f);

    SECTION("data members") {
        auto v2 = pt::Vec2(4.0f);
        auto v3 = pt::Vec3(4.0f);
        auto v4 = pt::Vec4(4.0f);

        CHECK(v2 == pt::ApproxVec2(4.0f, 4.0f));
        CHECK(a2 == pt::ApproxVec2(1.0f, 2.0f));
        CHECK(a2 == pt::ApproxVec2(a2.r, a2.g));
        CHECK(a2 == pt::ApproxVec2(a2.data[0], a2.data[1]));
        CHECK(a2 == pt::ApproxVec2(a2[0], a2[1]));

        CHECK(v3 == pt::ApproxVec3(4.0f, 4.0f, 4.0f));
        CHECK(a3 == pt::ApproxVec3(1.0f, 2.0f, 3.0f));
        CHECK(a3 == pt::ApproxVec3(a3.r, a3.g, a3.b));
        CHECK(a3 == pt::ApproxVec3(a3.data[0], a3.data[1], a3.data[2]));
        CHECK(a3 == pt::ApproxVec3(a3[0], a3[1], a3[2]));

        CHECK(v4 == pt::ApproxVec4(4.0f, 4.0f, 4.0f, 4.0f));
        CHECK(a4 == pt::ApproxVec4(1.0f, 2.0f, 3.0f, 4.0f));
        CHECK(a4 == pt::ApproxVec4(a4.r, a4.g, a4.b, a4.a));
        CHECK(a4 == pt::ApproxVec4(a4.data[0], a4.data[1], a4.data[2], a4.data[3]));
        CHECK(a4 == pt::ApproxVec4(a4[0], a4[1], a4[2], a4[3]));
    }

    SECTION("fromSpherical") {
        CHECK(pt::Vec2::fromSpherical(0.0f) == pt::ApproxVec2(1.0f, 0.0f));
        CHECK(pt::Vec2::fromSpherical(pt::pi<float> / 2.0f) == pt::ApproxVec2(0.0f, 1.0f));
        CHECK(pt::Vec3::fromSpherical(0.0f, 0.0f) == pt::ApproxVec3(0.0f, 0.0f, 1.0f));
        CHECK(pt::Vec3::fromSpherical(pt::pi<float> / 2.0f, 0.0f) == pt::ApproxVec3(1.0f, 0.0f, 0.0f));
        CHECK(pt::Vec3::fromSpherical(pt::pi<float> / 2.0f, pt::pi<float> / 2.0f) == pt::ApproxVec3(0.0f, 1.0f, 0.0f));
    }

    SECTION("vec += vec") {
        pt::Vec2& c2 = (a2 += b2);
        CHECK(a2 == pt::ApproxVec2(4.0f, 7.0f));
        CHECK(&a2 == &c2);

        pt::Vec3& c3 = (a3 += b3);
        CHECK(a3 == pt::ApproxVec3(4.0f, 7.0f, 10.0f));
        CHECK(&a3 == &c3);

        pt::Vec4& c4 = (a4 += b4);
        CHECK(a4 == pt::ApproxVec4(6.0f, 9.0f, 12.0f, 15.0f));
        CHECK(&a4 == &c4);
    }
    SECTION("vec -= vec") {
        pt::Vec2& c2 = (a2 -= b2);
        CHECK(a2 == pt::ApproxVec2(-2.0f, -3.0f));
        CHECK(&a2 == &c2);

        pt::Vec3& c3 = (a3 -= b3);
        CHECK(a3 == pt::ApproxVec3(-2.0f, -3.0f, -4.0f));
        CHECK(&a3 == &c3);

        pt::Vec4& c4 = (a4 -= b4);
        CHECK(a4 == pt::ApproxVec4(-4.0f, -5.0f, -6.0f, -7.0f));
        CHECK(&a4 == &c4);
    }
    SECTION("vec *= vec") {
        pt::Vec2& c2 = (a2 *= b2);
        CHECK(a2 == pt::ApproxVec2(3.0f, 10.0f));
        CHECK(&a2 == &c2);

        pt::Vec3& c3 = (a3 *= b3);
        CHECK(a3 == pt::ApproxVec3(3.0f, 10.0f, 21.0f));
        CHECK(&a3 == &c3);

        pt::Vec4& c4 = (a4 *= b4);
        CHECK(a4 == pt::ApproxVec4(5.0f, 14.0f, 27.0f, 44.0f));
        CHECK(&a4 == &c4);
    }
    SECTION("vec *= scalar") {
        pt::Vec2& c2 = (a2 *= 3.0f);
        CHECK(a2 == pt::ApproxVec2(3.0f, 6.0f));
        CHECK(&a2 == &c2);

        pt::Vec3& c3 = (a3 *= 3.0f);
        CHECK(a3 == pt::ApproxVec3(3.0f, 6.0f, 9.0f));
        CHECK(&a3 == &c3);

        pt::Vec4& c4 = (a4 *= 3.0f);
        CHECK(a4 == pt::ApproxVec4(3.0f, 6.0f, 9.0f, 12.0f));
        CHECK(&a4 == &c4);
    }
    SECTION("vec /= vec") {
        pt::Vec2& c2 = (a2 /= b2);
        CHECK(a2 == pt::ApproxVec2(0.33333333333f, 0.4f));
        CHECK(&a2 == &c2);

        pt::Vec3& c3 = (a3 /= b3);
        CHECK(a3 == pt::ApproxVec3(0.33333333333f, 0.4f, 0.42857142857f));
        CHECK(&a3 == &c3);

        pt::Vec4& c4 = (a4 /= b4);
        CHECK(a4 == pt::ApproxVec4(0.2f, 0.28571428571f, 0.33333333333f, 0.36363636363f));
        CHECK(&a4 == &c4);
    }
    SECTION("vec /= scalar") {
        pt::Vec2& c2 = (a2 /= 3.0f);
        CHECK(a2 == pt::ApproxVec2(0.33333333333f, 0.66666666666f));
        CHECK(&a2 == &c2);

        pt::Vec3& c3 = (a3 /= 3.0f);
        CHECK(a3 == pt::ApproxVec3(0.33333333333f, 0.66666666666f, 1.0f));
        CHECK(&a3 == &c3);

        pt::Vec4& c4 = (a4 /= 3.0f);
        CHECK(a4 == pt::ApproxVec4(0.33333333333f, 0.66666666666f, 1.0f, 1.33333333333f));
        CHECK(&a4 == &c4);
    }

    SECTION("vec + vec") {
        CHECK(a2 + b2 == pt::ApproxVec2(4.0f, 7.0f));
        CHECK(a3 + b3 == pt::ApproxVec3(4.0f, 7.0f, 10.0f));
        CHECK(a4 + b4 == pt::ApproxVec4(6.0f, 9.0f, 12.0f, 15.0f));
    }
    SECTION("-vec") {
        CHECK(-a2 == pt::ApproxVec2(-1.0f, -2.0f));
        CHECK(-a3 == pt::ApproxVec3(-1.0f, -2.0f, -3.0f));
        CHECK(-a4 == pt::ApproxVec4(-1.0f, -2.0f, -3.0f, -4.0f));
    }
    SECTION("vec - vec") {
        CHECK(a2 - b2 == pt::ApproxVec2(-2.0f, -3.0f));
        CHECK(a3 - b3 == pt::ApproxVec3(-2.0f, -3.0f, -4.0f));
        CHECK(a4 - b4 == pt::ApproxVec4(-4.0f, -5.0f, -6.0f, -7.0f));
    }
    SECTION("vec * vec") {
        CHECK(a2 * b2 == pt::ApproxVec2(3.0f, 10.0f));
        CHECK(a3 * b3 == pt::ApproxVec3(3.0f, 10.0f, 21.0f));
        CHECK(a4 * b4 == pt::ApproxVec4(5.0f, 14.0f, 27.0f, 44.0f));
    }
    SECTION("scalar * vec") {
        CHECK(3.0f * a2 == pt::ApproxVec2(3.0f, 6.0f));
        CHECK(3.0f * a3 == pt::ApproxVec3(3.0f, 6.0f, 9.0f));
        CHECK(3.0f * a4 == pt::ApproxVec4(3.0f, 6.0f, 9.0f, 12.0f));
    }
    SECTION("vec * scalar") {
        CHECK(a2 * 3.0f == pt::ApproxVec2(3.0f, 6.0f));
        CHECK(a3 * 3.0f == pt::ApproxVec3(3.0f, 6.0f, 9.0f));
        CHECK(a4 * 3.0f == pt::ApproxVec4(3.0f, 6.0f, 9.0f, 12.0f));
    }
    SECTION("vec / vec") {
        CHECK(a2 / b2 == pt::ApproxVec2(0.33333333333f, 0.4f));
        CHECK(a3 / b3 == pt::ApproxVec3(0.33333333333f, 0.4f, 0.42857142857f));
        CHECK(a4 / b4 == pt::ApproxVec4(0.2f, 0.28571428571f, 0.33333333333f, 0.36363636363f));
    }
    SECTION("vec / scalar") {
        CHECK(a2 / 3.0f == pt::ApproxVec2(0.33333333333f, 0.66666666666f));
        CHECK(a3 / 3.0f == pt::ApproxVec3(0.33333333333f, 0.66666666666f, 1.0f));
        CHECK(a4 / 3.0f == pt::ApproxVec4(0.33333333333f, 0.66666666666f, 1.0f, 1.33333333333f));
    }
}


// Vector functions
TEST_CASE("dot") {
    CHECK(pt::dot(pt::Vec2(2.0f, 3.0f), pt::Vec2(3.0f, 1.0f)) == pt::Approx(9.0f));
    CHECK(pt::dot(pt::Vec3(2.0f, 3.0f, 4.0f), pt::Vec3(5.0f, 6.0f, 7.0f)) == pt::Approx(56.0f));
    CHECK(pt::dot(pt::Vec4(2.0f, 3.0f, 4.0f, 5.0f), pt::Vec4(5.0f, 6.0f, 7.0f, 8.0f)) == pt::Approx(96.0f));
}

TEST_CASE("lengthSq") {
    CHECK(pt::lengthSq(pt::Vec2(2.0f, 3.0f)) == pt::Approx(13.0f));
    CHECK(pt::lengthSq(pt::Vec3(2.0f, 3.0f, 4.0f)) == pt::Approx(29.0f));
    CHECK(pt::lengthSq(pt::Vec4(2.0f, 3.0f, 4.0f, 5.0f)) == pt::Approx(54.0f));
}

TEST_CASE("length") {
    CHECK(pt::length(pt::Vec2(2.0f, 3.0f)) == pt::Approx(3.6055512754639892931192212674705f));
    CHECK(pt::length(pt::Vec3(2.0f, 3.0f, 4.0f)) == pt::Approx(5.3851648071345040312507104915403f));
    CHECK(pt::length(pt::Vec4(2.0f, 3.0f, 4.0f, 5.0f)) == pt::Approx(7.3484692283495342945918522241177f));
}

TEST_CASE("normalize") {
    CHECK(pt::normalize(pt::Vec2(2.0f, 3.0f)) == pt::ApproxVec2(0.55470019622f, 0.83205029433f));
    CHECK(pt::normalize(pt::Vec3(2.0f, 3.0f, 4.0f)) == pt::ApproxVec3(0.37139067635f, 0.55708601453f, 0.7427813527f));
    CHECK(pt::normalize(pt::Vec4(2.0f, 3.0f, 4.0f, 5.0f)) == pt::ApproxVec4(0.27216552697f, 0.40824829046f, 0.54433105395f, 0.68041381744f));
}

TEST_CASE("isNormalized") {
    CHECK(pt::isNormalized(pt::Vec2(0.55470019622f, 0.83205029433f)));
    CHECK_FALSE(pt::isNormalized(pt::Vec2(1.55470019622f, 0.83205029433f)));

    CHECK(pt::isNormalized(pt::Vec3(0.37139067635f, 0.55708601453f, 0.7427813527f)));
    CHECK_FALSE(pt::isNormalized(pt::Vec3(1.37139067635f, 0.55708601453f, 0.7427813527f)));

    CHECK(pt::isNormalized(pt::Vec4(0.27216552697f, 0.40824829046f, 0.54433105395f, 0.68041381744f)));
    CHECK_FALSE(pt::isNormalized(pt::Vec4(1.27216552697f, 0.40824829046f, 0.54433105395f, 0.68041381744f)));
}

TEST_CASE("cross") {
    auto vecA = pt::Vec3(1.0f, 0.0f, 0.0f);
    auto vecB = pt::Vec3(0.0f, 0.0f, 1.0f);
    auto vecC = pt::cross(vecB, vecA);

    CHECK(vecC == pt::ApproxVec3(0.0f, 1.0f, 0.0f));
    CHECK(pt::dot(vecA, vecC) == pt::Approx(0.0f));
    CHECK(pt::dot(vecB, vecC) == pt::Approx(0.0f));
    CHECK(pt::isNormalized(vecC));
}

TEST_CASE("reflect") {
    auto normal = pt::Vec3(0.0f, 1.0f, 0.0f);
    auto wo = pt::Vec3(-1.0f, 1.0f, 0.0f);
    auto wi = pt::reflect(wo, normal);
    CHECK(wi == pt::ApproxVec3(1.0f, 1.0f, 0.0f));
    CHECK(pt::dot(wi, normal) == pt::Approx(pt::dot(wo, normal)));

    wo = pt::normalize(wo);
    wi = pt::reflect(wo, normal);
    CHECK(wi == pt::ApproxVec3(0.7071067811865f, 0.7071067811865f, 0.0f));
    CHECK(pt::dot(wi, normal) == pt::Approx(pt::dot(wo, normal)));
    CHECK(pt::isNormalized(wi));
}

TEST_CASE("refract") {
    auto normal = pt::Vec3(0.0f, 1.0f, 0.0f);
    auto wi = pt::normalize(pt::Vec3(-1.0f, 1.0f, 0.0f));

    float etaI = 1.333f; // Water
    float etaT = 1.0f; // Air
    float eta = etaI / etaT; // Water to Air

    SECTION("Refraction Air To Air") {
        pt::Vec3 wt;
        bool tir = !pt::refract(wi, normal, 1.0f, wt);
        CHECK_FALSE(tir);
        CHECK(wt == pt::ApproxVec3(-wi.x, -wi.y, wi.z));
    }

    SECTION("Refraction From Water to Air") {
        pt::Vec3 wt;
        bool tir = !pt::refract(wi, normal, eta, wt);
        CHECK_FALSE(tir);
        CHECK(pt::dot(wt, normal) < 0.0f);

        float angleOfIncidence = std::acos(pt::abs(pt::dot(wi, normal)));
        float expectedAngleOfIncidence = static_cast<float>(pt::radians(45.0));
        CHECK(angleOfIncidence == pt::Approx(expectedAngleOfIncidence).epsilon(1e-5f));

        float angleOfRefraction = std::acos(pt::abs(pt::dot(wt, normal)));
        float expectedAngleOfRefraction = static_cast<float>(pt::radians(70.4883056));
        CHECK(angleOfRefraction == pt::Approx(expectedAngleOfRefraction).epsilon(1e-5f));
    }

    SECTION("Total Internal Reflection From Water to Air") {
        float expectedCriticalAngle = static_cast<float>(pt::radians(90.0 - 48.6066264));
        wi.x = -std::cos(expectedCriticalAngle + pt::testEps<float>);
        wi.y = std::sin(expectedCriticalAngle + pt::testEps<float>);

        pt::Vec3 wt;
        bool tir = !pt::refract(wi, normal, eta, wt);
        CHECK_FALSE(tir);

        wi.x = -std::cos(expectedCriticalAngle - pt::testEps<float>);
        wi.y = std::sin(expectedCriticalAngle - pt::testEps<float>);
        tir = !pt::refract(wi, normal, eta, wt);
        CHECK(tir);
    }
}


TEST_CASE("Matrix Operators & Functions") {
    auto a = pt::Mat4(
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f);
    auto b = pt::Mat4(
        17.0f, 18.0f, 19.0f, 20.0f,
        21.0f, 22.0f, 23.0f, 24.0f,
        25.0f, 26.0f, 27.0f, 28.0f,
        29.0f, 30.0f, 31.0f, 32.0f);

    SECTION("constructors") {
        auto diag = pt::Mat4(4.0f);
        auto rows = pt::Mat4::fromRows(
            pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f),
            pt::Vec4(5.0f, 6.0f, 7.0f, 8.0f),
            pt::Vec4(9.0f, 10.0f, 11.0f, 12.0f),
            pt::Vec4(13.0f, 14.0f, 15.0f, 16.0f));
        auto cols = pt::Mat4::fromColumns(
            pt::Vec4(1.0f, 5.0f, 9.0f, 13.0f),
            pt::Vec4(2.0f, 6.0f, 10.0f, 14.0f),
            pt::Vec4(3.0f, 7.0f, 11.0f, 15.0f),
            pt::Vec4(4.0f, 8.0f, 12.0f, 16.0f));

        CHECK(a._11 == pt::Approx(1.0f));
        CHECK(a._12 == pt::Approx(2.0f));
        CHECK(a._13 == pt::Approx(3.0f));
        CHECK(a._14 == pt::Approx(4.0f));
        CHECK(a._21 == pt::Approx(5.0f));
        CHECK(a._22 == pt::Approx(6.0f));
        CHECK(a._23 == pt::Approx(7.0f));
        CHECK(a._24 == pt::Approx(8.0f));
        CHECK(a._31 == pt::Approx(9.0f));
        CHECK(a._32 == pt::Approx(10.0f));
        CHECK(a._33 == pt::Approx(11.0f));
        CHECK(a._34 == pt::Approx(12.0f));
        CHECK(a._41 == pt::Approx(13.0f));
        CHECK(a._42 == pt::Approx(14.0f));
        CHECK(a._43 == pt::Approx(15.0f));
        CHECK(a._44 == pt::Approx(16.0f));

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                CHECK(a.data[i][j] == pt::Approx(static_cast<float>(j + i * 4 + 1)));
                CHECK(a.data[i][j] == pt::Approx(a[i][j]));
                CHECK(a.rows[i][j] == pt::Approx(a[i][j]));
                CHECK(a[i][j] == pt::Approx(rows[i][j]));
                CHECK(a[i][j] == pt::Approx(cols[i][j]));

                if (i == j) {
                    CHECK(diag[i][j] == pt::Approx(4.0f));
                }
                else {
                    CHECK(diag[i][j] == pt::Approx(0.0f));
                }
            }
        }
    }

    SECTION("mat * mat, mat *= mat") {
        auto expected = pt::ApproxMat4(
            250.0f, 260.0f, 270.0f, 280.0f,
            618.0f, 644.0f, 670.0f, 696.f,
            986.0f, 1028.0f, 1070.0f, 1112.0f,
            1354.0f, 1412.0f, 1470.0f, 1528.0f);
        CHECK(a * b == expected);

        pt::Mat4& c = (a *= b);
        CHECK(a == expected);
        CHECK(&a == &c);
    }

    SECTION("mat * scalar, scalar * mat, mat *= scalar") {
        auto expected = pt::ApproxMat4(
            2.0f, 4.0f, 6.0f, 8.0f,
            10.0f, 12.0f, 14.0f, 16.f,
            18.0f, 20.0f, 22.0f, 24.0f,
            26.0f, 28.0f, 30.0f, 32.0f);
        CHECK(a * 2.0f == expected);
        CHECK(2.0f * a == expected);

        pt::Mat4& c = (a *= 2.0f);
        CHECK(a == expected);
        CHECK(&a == &c);
    }

    SECTION("transpose") {
        auto expected = pt::ApproxMat4(
            1.0f, 5.0f, 9.0f, 13.0f,
            2.0f, 6.0f, 10.0f, 14.0f,
            3.0f, 7.0f, 11.0f, 15.0f,
            4.0f, 8.0f, 12.0f, 16.0f);
        REQUIRE(pt::transpose(a) == expected);
    }

    SECTION("transformPoint3x4") {
        auto expected = pt::ApproxVec3(138.0f, 394.0f, 650.0f);
        REQUIRE(pt::transformPoint3x4(a, pt::Vec3(17.0f, 21.0f, 25.0f)) == expected);
    }

    SECTION("transformVector3x4") {
        auto expected = pt::ApproxVec3(134.0f, 386.0f, 638.0f);
        REQUIRE(pt::transformVector3x4(a, pt::Vec3(17.0f, 21.0f, 25.0f)) == expected);
    }

    SECTION("lookAt") {
        auto expected = pt::ApproxMat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
        auto result = pt::lookAt(
            pt::Vec3(0.0f, 0.0f, 0.0f),
            pt::Vec3(0.0f, 0.0f, -1.0f),
            pt::Vec3(0.0f, 1.0f, 0.0f));
        REQUIRE(result == expected);
    }

    SECTION("translation") {
        auto expected = pt::ApproxMat4(
            1.0f, 0.0f, 0.0f, 3.0f,
            0.0f, 1.0f, 0.0f, 4.0f,
            0.0f, 0.0f, 1.0f, 5.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
        auto result = pt::translation(pt::Vec3(3.0f, 4.0f, 5.0f));
        REQUIRE(result == expected);

        auto transformed = pt::transformPoint3x4(result, pt::Vec3(1.0f, 2.0f, 3.0f));
        REQUIRE(transformed == pt::ApproxVec3(4.0f, 6.0f, 8.0f));

        transformed = pt::transformVector3x4(result, pt::Vec3(1.0f, 2.0f, 3.0f));
        REQUIRE(transformed == pt::ApproxVec3(1.0f, 2.0f, 3.0f));
    }

    SECTION("scaling") {
        auto expected = pt::ApproxMat4(
            3.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 4.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 5.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
        auto result = pt::scaling(pt::Vec3(3.0f, 4.0f, 5.0f));
        REQUIRE(result == expected);

        auto transformed = pt::transformPoint3x4(result, pt::Vec3(1.0f, 2.0f, 3.0f));
        REQUIRE(transformed == pt::ApproxVec3(3.0f, 8.0f, 15.0f));

        transformed = pt::transformVector3x4(result, pt::Vec3(1.0f, 2.0f, 3.0f));
        REQUIRE(transformed == pt::ApproxVec3(3.0f, 8.0f, 15.0f));
    }

    SECTION("rotationX") {
        auto expected = pt::ApproxMat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
        auto result = pt::rotationX(pt::pi<float> * 0.5f);
        REQUIRE(result == expected);

        auto transformed = pt::transformPoint3x4(result, pt::Vec3(0.0f, 1.0f, 0.0f));
        REQUIRE(transformed == pt::ApproxVec3(0.0f, 0.0f, 1.0f));

        transformed = pt::transformVector3x4(result, pt::Vec3(0.0f, 1.0f, 0.0f));
        REQUIRE(transformed == pt::ApproxVec3(0.0f, 0.0f, 1.0f));
    }

    SECTION("rotationY") {
        auto expected = pt::ApproxMat4(
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
        auto result = pt::rotationY(pt::pi<float> * 0.5f);
        REQUIRE(result == expected);

        auto transformed = pt::transformPoint3x4(result, pt::Vec3(1.0f, 0.0f, 0.0f));
        REQUIRE(transformed == pt::ApproxVec3(0.0f, 0.0f, -1.0f));

        transformed = pt::transformVector3x4(result, pt::Vec3(1.0f, 0.0f, 0.0f));
        REQUIRE(transformed == pt::ApproxVec3(0.0f, 0.0f, -1.0f));
    }

    SECTION("rotationZ") {
        auto expected = pt::ApproxMat4(
            0.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
        auto result = pt::rotationZ(pt::pi<float> * 0.5f);
        REQUIRE(result == expected);

        auto transformed = pt::transformPoint3x4(result, pt::Vec3(1.0f, 0.0f, 0.0f));
        REQUIRE(transformed == pt::ApproxVec3(0.0f, 1.0f, 0.0f));

        transformed = pt::transformVector3x4(result, pt::Vec3(1.0f, 0.0f, 0.0f));
        REQUIRE(transformed == pt::ApproxVec3(0.0f, 1.0f, 0.0f));
    }
}

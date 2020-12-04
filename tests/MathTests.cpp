#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "MathUtils.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

constexpr double testEps = 0.000001;

template <typename T>
constexpr bool approxEquals(const pt::Vector2<T>& a,
        const pt::Vector2<T>& b, T eps = static_cast<T>(testEps)) {
    return abs(a.x - b.x) < eps
        && abs(a.y - b.y) < eps;
}

template <typename T>
constexpr bool approxEquals(const pt::Vector3<T>& a,
    const pt::Vector3<T>& b, T eps = static_cast<T>(testEps)) {
    return abs(a.x - b.x) < eps
        && abs(a.y - b.y) < eps
        && abs(a.z - b.z) < eps;
}

template <typename T>
constexpr bool approxEquals(const pt::Vector4<T>& a,
    const pt::Vector4<T>& b, T eps = static_cast<T>(testEps)) {
    return abs(a.x - b.x) < eps
        && abs(a.y - b.y) < eps
        && abs(a.z - b.z) < eps
        && abs(a.w - b.w) < eps;
}


TEST_CASE("Vector2") {
    auto a = pt::Vec2(1.0f, 2.0f);
    auto b = pt::Vec2(2.0f, 1.0f);

    REQUIRE((a.x == 1.0f && a.y == 2.0f));
    REQUIRE((a.x == a.r && a.y == a.g));
    REQUIRE((a.x == a.data[0] && a.y == a.data[1]));
    REQUIRE((a.x == a[0] && a.y == a[1]));

    SECTION("vec + vec") {
        REQUIRE(approxEquals(a + b, pt::Vec2(3.0f, 3.0f)));
    }
    SECTION("-vec") {
        REQUIRE(approxEquals(-a, pt::Vec2(-1.0f, -2.0f)));
    }
    SECTION("vec - vec") {
        REQUIRE(approxEquals(a - b, pt::Vec2(-1.0f, 1.0f)));
    }
    SECTION("vec * vec") {
        REQUIRE(approxEquals(a * b, pt::Vec2(2.0f, 2.0f)));
    }
    SECTION("scalar * vec") {
        REQUIRE(approxEquals(3.0f * a, pt::Vec2(3.0f, 6.0f)));
    }
    SECTION("vec * scalar") {
        REQUIRE(approxEquals(a * 3.0f, pt::Vec2(3.0f, 6.0f)));
    }
    SECTION("vec / vec") {
        REQUIRE(approxEquals(a / b, pt::Vec2(0.5f, 2.0f)));
    }
    SECTION("vec / scalar") {
        REQUIRE(approxEquals(a / 2.0f, pt::Vec2(0.5f, 1.0f)));
    }
}

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "MathUtils.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

namespace pt {
    template <typename T> constexpr T testEps = static_cast<T>(0.0000001);

    template <typename T>
    struct Approx {
        explicit constexpr Approx(T value_) : value(value_) { }

        friend constexpr bool operator==(T lhs, const Approx& rhs) {
            return std::abs(lhs - rhs.value) < testEps<T>;
        }

        friend constexpr bool operator!=(T lhs, const Approx& rhs) {
            return !(lhs == rhs);
        }

        T value;
    };

    template <typename T>
    struct ApproxVec2 {
        explicit constexpr ApproxVec2(T x_, T y_) : x(x_), y(y_) { }

        friend constexpr bool operator==(const pt::Vector2<T>& lhs, const ApproxVec2& rhs) {
            return lhs.x == rhs.x
                && lhs.y == rhs.y;
        }

        friend constexpr bool operator!=(const pt::Vector2<T>& lhs, const ApproxVec2& rhs) {
            return !(lhs == rhs);
        }

        Approx<T> x, y;
    };

    template <typename T>
    struct ApproxVec3 {
        explicit constexpr ApproxVec3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) { }

        friend constexpr bool operator==(const pt::Vector3<T>& lhs, const ApproxVec3& rhs) {
            return lhs.x == rhs.x
                && lhs.y == rhs.y
                && lhs.z == rhs.z;
        }

        friend constexpr bool operator!=(const pt::Vector3<T>& lhs, const ApproxVec3& rhs) {
            return !(lhs == rhs);
        }

        Approx<T> x, y, z;
    };

    template <typename T>
    struct ApproxVec4 {
        explicit constexpr ApproxVec4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) { }

        friend constexpr bool operator==(const pt::Vector4<T>& lhs, const ApproxVec4& rhs) {
            return lhs.x == rhs.x
                && lhs.y == rhs.y
                && lhs.z == rhs.z
                && lhs.w == rhs.w;
        }

        friend constexpr bool operator!=(const pt::Vector4<T>& lhs, const ApproxVec4& rhs) {
            return !(lhs == rhs);
        }

        Approx<T> x, y, z, w;
    };
}


TEST_CASE("Functions") {
    SECTION("min") {
        REQUIRE(pt::min(-1.0f, 1.0f) == pt::Approx(-1.0f));
        REQUIRE(pt::min(pt::Vec2(1.0f, 2.0f), pt::Vec2(2.0f, 1.0f)) == pt::ApproxVec2(1.0f, 1.0f));
        REQUIRE(pt::min(pt::Vec3(1.0f, 3.0f, 2.0f), pt::Vec3(3.0f, 2.0f, 1.0f)) == pt::ApproxVec3(1.0f, 2.0f, 1.0f));
        REQUIRE(pt::min(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f), pt::Vec4(4.0f, 3.0f, 2.0f, 1.0f)) == pt::ApproxVec4(1.0f, 2.0f, 2.0f, 1.0f));
    }
    SECTION("max") {
        REQUIRE(pt::max(-1.0f, 1.0f) == pt::Approx(1.0f));
        REQUIRE(pt::max(pt::Vec2(1.0f, 2.0f), pt::Vec2(2.0f, 1.0f)) == pt::ApproxVec2(2.0f, 2.0f));
        REQUIRE(pt::max(pt::Vec3(1.0f, 3.0f, 2.0f), pt::Vec3(3.0f, 2.0f, 1.0f)) == pt::ApproxVec3(3.0f, 3.0f, 2.0f));
        REQUIRE(pt::max(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f), pt::Vec4(4.0f, 3.0f, 2.0f, 1.0f)) == pt::ApproxVec4(4.0f, 3.0f, 3.0f, 4.0f));
    }

    SECTION("minComponent") {
        REQUIRE(pt::minComponent(pt::Vec2(1.0f, 2.0f)) == pt::Approx(1.0f));
        REQUIRE(pt::minComponent(pt::Vec2(2.0f, 1.0f)) == pt::Approx(1.0f));

        REQUIRE(pt::minComponent(pt::Vec3(1.0f, 2.0f, 3.0f)) == pt::Approx(1.0f));
        REQUIRE(pt::minComponent(pt::Vec3(1.0f, 3.0f, 2.0f)) == pt::Approx(1.0f));
        REQUIRE(pt::minComponent(pt::Vec3(3.0f, 2.0f, 1.0f)) == pt::Approx(1.0f));

        REQUIRE(pt::minComponent(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == pt::Approx(1.0f));
        REQUIRE(pt::minComponent(pt::Vec4(1.0f, 2.0f, 4.0f, 3.0f)) == pt::Approx(1.0f));
        REQUIRE(pt::minComponent(pt::Vec4(1.0f, 4.0f, 3.0f, 2.0f)) == pt::Approx(1.0f));
        REQUIRE(pt::minComponent(pt::Vec4(4.0f, 2.0f, 3.0f, 1.0f)) == pt::Approx(1.0f));
    }
    SECTION("maxComponent") {
        REQUIRE(pt::maxComponent(pt::Vec2(1.0f, 2.0f)) == pt::Approx(2.0f));
        REQUIRE(pt::maxComponent(pt::Vec2(2.0f, 1.0f)) == pt::Approx(2.0f));

        REQUIRE(pt::maxComponent(pt::Vec3(1.0f, 2.0f, 3.0f)) == pt::Approx(3.0f));
        REQUIRE(pt::maxComponent(pt::Vec3(1.0f, 3.0f, 2.0f)) == pt::Approx(3.0f));
        REQUIRE(pt::maxComponent(pt::Vec3(3.0f, 2.0f, 1.0f)) == pt::Approx(3.0f));

        REQUIRE(pt::maxComponent(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == pt::Approx(4.0f));
        REQUIRE(pt::maxComponent(pt::Vec4(1.0f, 2.0f, 4.0f, 3.0f)) == pt::Approx(4.0f));
        REQUIRE(pt::maxComponent(pt::Vec4(1.0f, 4.0f, 3.0f, 2.0f)) == pt::Approx(4.0f));
        REQUIRE(pt::maxComponent(pt::Vec4(4.0f, 2.0f, 3.0f, 1.0f)) == pt::Approx(4.0f));
    }

    SECTION("clamp") {
        REQUIRE(pt::clamp(-1.0f, 1.5f, 1.75f) == pt::Approx(1.5f));
        REQUIRE(pt::clamp(1.6f, 1.5f, 1.75f) == pt::Approx(1.6f));
        REQUIRE(pt::clamp(3.0f, 1.5f, 1.75f) == pt::Approx(1.75f));

        REQUIRE(pt::clamp(pt::Vec2(-1.0f, -2.0f), 1.5f, 1.75f) == pt::ApproxVec2(1.5f, 1.5f));
        REQUIRE(pt::clamp(pt::Vec2(1.0f, 0.9f), 1.5f, 1.75f) == pt::ApproxVec2(1.5f, 1.5f));
        REQUIRE(pt::clamp(pt::Vec2(1.6f, 1.7f), 1.5f, 1.75f) == pt::ApproxVec2(1.6f, 1.7f));
        REQUIRE(pt::clamp(pt::Vec2(2.0f, 3.0f), 1.5f, 1.75f) == pt::ApproxVec2(1.75f, 1.75f));

        REQUIRE(pt::clamp(pt::Vec3(-1.0f, -2.0f, -3.0f), 1.5f, 1.75f) == pt::ApproxVec3(1.5f, 1.5f, 1.5f));
        REQUIRE(pt::clamp(pt::Vec3(1.0f, 1.0f, 1.0f), 1.5f, 1.75f) == pt::ApproxVec3(1.5f, 1.5f, 1.5f));
        REQUIRE(pt::clamp(pt::Vec3(1.6f, 1.7f, 1.72f), 1.5f, 1.75f) == pt::ApproxVec3(1.6f, 1.7f, 1.72f));
        REQUIRE(pt::clamp(pt::Vec3(2.0f, 3.0f, 4.0f), 1.5f, 1.75f) == pt::ApproxVec3(1.75f, 1.75f, 1.75f));

        REQUIRE(pt::clamp(pt::Vec4(-1.0f, -1.0f, -1.0f, -1.0f), 1.5f, 1.75f) == pt::ApproxVec4(1.5f, 1.5f, 1.5f, 1.5f));
        REQUIRE(pt::clamp(pt::Vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.5f, 1.75f) == pt::ApproxVec4(1.5f, 1.5f, 1.5f, 1.5f));
        REQUIRE(pt::clamp(pt::Vec4(1.6f, 1.7f, 1.72f, 1.74f), 1.5f, 1.75f) == pt::ApproxVec4(1.6f, 1.7f, 1.72f, 1.74f));
        REQUIRE(pt::clamp(pt::Vec4(2.0f, 3.0f, 4.0f, 5.0f), 1.5f, 1.75f) == pt::ApproxVec4(1.75f, 1.75f, 1.75f, 1.75f));
    }

    SECTION("saturate") {
        REQUIRE(pt::saturate(-1.0f) == pt::Approx(0.0f));
        REQUIRE(pt::saturate(0.6f) == pt::Approx(0.6f));
        REQUIRE(pt::saturate(2.0f) == pt::Approx(1.0f));

        REQUIRE(pt::saturate(pt::Vec2(-1.0f, -2.0f)) == pt::ApproxVec2(0.0f, 0.0f));
        REQUIRE(pt::saturate(pt::Vec2(0.6f, 0.7f)) == pt::ApproxVec2(0.6f, 0.7f));
        REQUIRE(pt::saturate(pt::Vec2(1.0f, 2.0f)) == pt::ApproxVec2(1.0f, 1.0f));

        REQUIRE(pt::saturate(pt::Vec3(-1.0f, -2.0f, -3.0f)) == pt::ApproxVec3(0.0f, 0.0f, 0.0f));
        REQUIRE(pt::saturate(pt::Vec3(0.6f, 0.7f, 0.8f)) == pt::ApproxVec3(0.6f, 0.7f, 0.8f));
        REQUIRE(pt::saturate(pt::Vec3(1.0f, 2.0f, 3.0f)) == pt::ApproxVec3(1.0f, 1.0f, 1.0f));

        REQUIRE(pt::saturate(pt::Vec4(-1.0f, -2.0f, -3.0f, -4.0f)) == pt::ApproxVec4(0.0f, 0.0f, 0.0f, 0.0f));
        REQUIRE(pt::saturate(pt::Vec4(0.6f, 0.7f, 0.8f, 0.9f)) == pt::ApproxVec4(0.6f, 0.7f, 0.8f, 0.9f));
        REQUIRE(pt::saturate(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == pt::ApproxVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    SECTION("abs") {
        REQUIRE(pt::abs(-1.0f) == pt::Approx(1.0f));
        REQUIRE(pt::abs(1.0f) == pt::Approx(1.0f));

        REQUIRE(pt::abs(pt::Vec2(1.0f, 2.0f)) == pt::ApproxVec2(1.0f, 2.0f));
        REQUIRE(pt::abs(pt::Vec2(-1.0f, -2.0f)) == pt::ApproxVec2(1.0f, 2.0f));

        REQUIRE(pt::abs(pt::Vec3(1.0f, 2.0f, 3.0f)) == pt::ApproxVec3(1.0f, 2.0f, 3.0f));
        REQUIRE(pt::abs(pt::Vec3(-1.0f, -2.0f, -3.0f)) == pt::ApproxVec3(1.0f, 2.0f, 3.0f));

        REQUIRE(pt::abs(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == pt::ApproxVec4(1.0f, 2.0f, 3.0f, 4.0f));
        REQUIRE(pt::abs(pt::Vec4(-1.0f, -2.0f, -3.0f, -4.0f)) == pt::ApproxVec4(1.0f, 2.0f, 3.0f, 4.0f));
    }

    SECTION("floor") {
        REQUIRE(pt::floor(-1.0f) == pt::Approx(-1.0f));
        REQUIRE(pt::floor(1.0f) == pt::Approx(1.0f));
        REQUIRE(pt::floor(-1.999f) == pt::Approx(-2.0f));
        REQUIRE(pt::floor(2.999f) == pt::Approx(2.0f));

        REQUIRE(pt::floor(pt::Vec2(-1.0f, -2.0f)) == pt::ApproxVec2(-1.0f, -2.0f));
        REQUIRE(pt::floor(pt::Vec2(1.0f, 2.0f)) == pt::ApproxVec2(1.0f, 2.0f));
        REQUIRE(pt::floor(pt::Vec2(-1.4f, -2.999f)) == pt::ApproxVec2(-2.0f, -3.0f));
        REQUIRE(pt::floor(pt::Vec2(1.4f, 2.999f)) == pt::ApproxVec2(1.0f, 2.0f));

        REQUIRE(pt::floor(pt::Vec3(-1.0f, -2.0f, -3.0f)) == pt::ApproxVec3(-1.0f, -2.0f, -3.0f));
        REQUIRE(pt::floor(pt::Vec3(1.0f, 2.0f, 3.0f)) == pt::ApproxVec3(1.0f, 2.0f, 3.0f));
        REQUIRE(pt::floor(pt::Vec3(-1.4f, -2.999f, -3.645f)) == pt::ApproxVec3(-2.0f, -3.0f, -4.0f));
        REQUIRE(pt::floor(pt::Vec3(1.4f, 2.999f, 3.645f)) == pt::ApproxVec3(1.0f, 2.0f, 3.0f));

        REQUIRE(pt::floor(pt::Vec4(-1.0f, -2.0f, -3.0f, -4.0f)) == pt::ApproxVec4(-1.0f, -2.0f, -3.0f, -4.0f));
        REQUIRE(pt::floor(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == pt::ApproxVec4(1.0f, 2.0f, 3.0f, 4.0f));
        REQUIRE(pt::floor(pt::Vec4(-1.4f, -2.999f, -3.645f, -4.031f)) == pt::ApproxVec4(-2.0f, -3.0f, -4.0f, -5.0f));
        REQUIRE(pt::floor(pt::Vec4(1.4f, 2.999f, 3.645f, 4.031f)) == pt::ApproxVec4(1.0f, 2.0f, 3.0f, 4.0f));
    }

    SECTION("ceil") {
        REQUIRE(pt::ceil(-1.0f) == pt::Approx(-1.0f));
        REQUIRE(pt::ceil(1.0f) == pt::Approx(1.0f));
        REQUIRE(pt::ceil(-1.999f) == pt::Approx(-1.0f));
        REQUIRE(pt::ceil(2.999f) == pt::Approx(3.0f));

        REQUIRE(pt::ceil(pt::Vec2(-1.0f, -2.0f)) == pt::ApproxVec2(-1.0f, -2.0f));
        REQUIRE(pt::ceil(pt::Vec2(1.0f, 2.0f)) == pt::ApproxVec2(1.0f, 2.0f));
        REQUIRE(pt::ceil(pt::Vec2(-1.4f, -2.999f)) == pt::ApproxVec2(-1.0f, -2.0f));
        REQUIRE(pt::ceil(pt::Vec2(1.4f, 2.999f)) == pt::ApproxVec2(2.0f, 3.0f));

        REQUIRE(pt::ceil(pt::Vec3(-1.0f, -2.0f, -3.0f)) == pt::ApproxVec3(-1.0f, -2.0f, -3.0f));
        REQUIRE(pt::ceil(pt::Vec3(1.0f, 2.0f, 3.0f)) == pt::ApproxVec3(1.0f, 2.0f, 3.0f));
        REQUIRE(pt::ceil(pt::Vec3(-1.4f, -2.999f, -3.645f)) == pt::ApproxVec3(-1.0f, -2.0f, -3.0f));
        REQUIRE(pt::ceil(pt::Vec3(1.4f, 2.999f, 3.645f)) == pt::ApproxVec3(2.0f, 3.0f, 4.0f));

        REQUIRE(pt::ceil(pt::Vec4(-1.0f, -2.0f, -3.0f, -4.0f)) == pt::ApproxVec4(-1.0f, -2.0f, -3.0f, -4.0f));
        REQUIRE(pt::ceil(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f)) == pt::ApproxVec4(1.0f, 2.0f, 3.0f, 4.0f));
        REQUIRE(pt::ceil(pt::Vec4(-1.4f, -2.999f, -3.645f, -4.031f)) == pt::ApproxVec4(-1.0f, -2.0f, -3.0f, -4.0f));
        REQUIRE(pt::ceil(pt::Vec4(1.4f, 2.999f, 3.645f, 4.031f)) == pt::ApproxVec4(2.0f, 3.0f, 4.0f, 5.0f));
    }

    SECTION("lerp") {
        REQUIRE(pt::lerp(1.0f, 3.0f, 0.5f) == pt::Approx(2.0f));
        REQUIRE(pt::lerp(pt::Vec2(1.0f, 2.0f), pt::Vec2(2.0f, 4.0f), 0.5f) == pt::ApproxVec2(1.5f, 3.0f));
        REQUIRE(pt::lerp(pt::Vec3(1.0f, 2.0f, 3.0f), pt::Vec3(2.0f, 4.0f, 6.0f), 0.5f) == pt::ApproxVec3(1.5f, 3.0f, 4.5f));
        REQUIRE(pt::lerp(pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f), pt::Vec4(2.0f, 4.0f, 6.0f, 8.0f), 0.5f) == pt::ApproxVec4(1.5f, 3.0f, 4.5f, 6.0f));
    }

    SECTION("remap") {
        REQUIRE(pt::remap(0.0f, 0.0f, 1.0f, -1.0f, 1.0f) == pt::Approx(-1.0f));
        REQUIRE(pt::remap(0.0f, -1.0f, 1.0f, 0.0f, 1.0f) == pt::Approx(0.5f));
    }

    SECTION("degrees") {
        REQUIRE(pt::degrees(0.0f) == pt::Approx(0.0f));
        REQUIRE(pt::degrees(pt::pi<float>) == pt::Approx(180.0f));
    }

    SECTION("radians") {
        REQUIRE(pt::radians(0.0f) == pt::Approx(0.0f));
        REQUIRE(pt::radians(180.0f) == pt::Approx(pt::pi<float>));
    }
}


TEST_CASE("Vector Operators") {
    auto v2 = pt::Vec2(4.0f);
    auto a2 = pt::Vec2(1.0f, 2.0f);
    auto b2 = pt::Vec2(3.0f, 5.0f);

    auto v3 = pt::Vec3(4.0f);
    auto a3 = pt::Vec3(1.0f, 2.0f, 3.0f);
    auto b3 = pt::Vec3(3.0f, 5.0f, 7.0f);

    auto v4 = pt::Vec4(4.0f);
    auto a4 = pt::Vec4(1.0f, 2.0f, 3.0f, 4.0f);
    auto b4 = pt::Vec4(5.0f, 7.0f, 9.0f, 11.0f);

    SECTION("data members") {
        REQUIRE(v2 == pt::ApproxVec2(4.0f, 4.0f));
        REQUIRE(a2 == pt::ApproxVec2(1.0f, 2.0f));
        REQUIRE(a2 == pt::ApproxVec2(a2.r, a2.g));
        REQUIRE(a2 == pt::ApproxVec2(a2.data[0], a2.data[1]));
        REQUIRE(a2 == pt::ApproxVec2(a2[0], a2[1]));

        REQUIRE(v3 == pt::ApproxVec3(4.0f, 4.0f, 4.0f));
        REQUIRE(a3 == pt::ApproxVec3(1.0f, 2.0f, 3.0f));
        REQUIRE(a3 == pt::ApproxVec3(a3.r, a3.g, a3.b));
        REQUIRE(a3 == pt::ApproxVec3(a3.data[0], a3.data[1], a3.data[2]));
        REQUIRE(a3 == pt::ApproxVec3(a3[0], a3[1], a3[2]));

        REQUIRE(v4 == pt::ApproxVec4(4.0f, 4.0f, 4.0f, 4.0f));
        REQUIRE(a4 == pt::ApproxVec4(1.0f, 2.0f, 3.0f, 4.0f));
        REQUIRE(a4 == pt::ApproxVec4(a4.r, a4.g, a4.b, a4.a));
        REQUIRE(a4 == pt::ApproxVec4(a4.data[0], a4.data[1], a4.data[2], a4.data[3]));
        REQUIRE(a4 == pt::ApproxVec4(a4[0], a4[1], a4[2], a4[3]));
    }

    SECTION("vec += vec") {
        pt::Vec2& c2 = (a2 += b2);
        REQUIRE(a2 == pt::ApproxVec2(4.0f, 7.0f));
        REQUIRE(&a2 == &c2);

        pt::Vec3& c3 = (a3 += b3);
        REQUIRE(a3 == pt::ApproxVec3(4.0f, 7.0f, 10.0f));
        REQUIRE(&a3 == &c3);

        pt::Vec4& c4 = (a4 += b4);
        REQUIRE(a4 == pt::ApproxVec4(6.0f, 9.0f, 12.0f, 15.0f));
        REQUIRE(&a4 == &c4);
    }
    SECTION("vec -= vec") {
        pt::Vec2& c2 = (a2 -= b2);
        REQUIRE(a2 == pt::ApproxVec2(-2.0f, -3.0f));
        REQUIRE(&a2 == &c2);

        pt::Vec3& c3 = (a3 -= b3);
        REQUIRE(a3 == pt::ApproxVec3(-2.0f, -3.0f, -4.0f));
        REQUIRE(&a3 == &c3);

        pt::Vec4& c4 = (a4 -= b4);
        REQUIRE(a4 == pt::ApproxVec4(-4.0f, -5.0f, -6.0f, -7.0f));
        REQUIRE(&a4 == &c4);
    }
    SECTION("vec *= vec") {
        pt::Vec2& c2 = (a2 *= b2);
        REQUIRE(a2 == pt::ApproxVec2(3.0f, 10.0f));
        REQUIRE(&a2 == &c2);

        pt::Vec3& c3 = (a3 *= b3);
        REQUIRE(a3 == pt::ApproxVec3(3.0f, 10.0f, 21.0f));
        REQUIRE(&a3 == &c3);

        pt::Vec4& c4 = (a4 *= b4);
        REQUIRE(a4 == pt::ApproxVec4(5.0f, 14.0f, 27.0f, 44.0f));
        REQUIRE(&a4 == &c4);
    }
    SECTION("vec *= scalar") {
        pt::Vec2& c2 = (a2 *= 3.0f);
        REQUIRE(a2 == pt::ApproxVec2(3.0f, 6.0f));
        REQUIRE(&a2 == &c2);

        pt::Vec3& c3 = (a3 *= 3.0f);
        REQUIRE(a3 == pt::ApproxVec3(3.0f, 6.0f, 9.0f));
        REQUIRE(&a3 == &c3);

        pt::Vec4& c4 = (a4 *= 3.0f);
        REQUIRE(a4 == pt::ApproxVec4(3.0f, 6.0f, 9.0f, 12.0f));
        REQUIRE(&a4 == &c4);
    }
    SECTION("vec /= vec") {
        pt::Vec2& c2 = (a2 /= b2);
        REQUIRE(a2 == pt::ApproxVec2(0.33333333333f, 0.4f));
        REQUIRE(&a2 == &c2);

        pt::Vec3& c3 = (a3 /= b3);
        REQUIRE(a3 == pt::ApproxVec3(0.33333333333f, 0.4f, 0.42857142857f));
        REQUIRE(&a3 == &c3);

        pt::Vec4& c4 = (a4 /= b4);
        REQUIRE(a4 == pt::ApproxVec4(0.2f, 0.28571428571f, 0.33333333333f, 0.36363636363f));
        REQUIRE(&a4 == &c4);
    }
    SECTION("vec /= scalar") {
        pt::Vec2& c2 = (a2 /= 3.0f);
        REQUIRE(a2 == pt::ApproxVec2(0.33333333333f, 0.66666666666f));
        REQUIRE(&a2 == &c2);

        pt::Vec3& c3 = (a3 /= 3.0f);
        REQUIRE(a3 == pt::ApproxVec3(0.33333333333f, 0.66666666666f, 1.0f));
        REQUIRE(&a3 == &c3);

        pt::Vec4& c4 = (a4 /= 3.0f);
        REQUIRE(a4 == pt::ApproxVec4(0.33333333333f, 0.66666666666f, 1.0f, 1.33333333333f));
        REQUIRE(&a4 == &c4);
    }

    SECTION("vec + vec") {
        REQUIRE(a2 + b2 == pt::ApproxVec2(4.0f, 7.0f));
        REQUIRE(a3 + b3 == pt::ApproxVec3(4.0f, 7.0f, 10.0f));
        REQUIRE(a4 + b4 == pt::ApproxVec4(6.0f, 9.0f, 12.0f, 15.0f));
    }
    SECTION("-vec") {
        REQUIRE(-a2 == pt::ApproxVec2(-1.0f, -2.0f));
        REQUIRE(-a3 == pt::ApproxVec3(-1.0f, -2.0f, -3.0f));
        REQUIRE(-a4 == pt::ApproxVec4(-1.0f, -2.0f, -3.0f, -4.0f));
    }
    SECTION("vec - vec") {
        REQUIRE(a2 - b2 == pt::ApproxVec2(-2.0f, -3.0f));
        REQUIRE(a3 - b3 == pt::ApproxVec3(-2.0f, -3.0f, -4.0f));
        REQUIRE(a4 - b4 == pt::ApproxVec4(-4.0f, -5.0f, -6.0f, -7.0f));
    }
    SECTION("vec * vec") {
        REQUIRE(a2 * b2 == pt::ApproxVec2(3.0f, 10.0f));
        REQUIRE(a3 * b3 == pt::ApproxVec3(3.0f, 10.0f, 21.0f));
        REQUIRE(a4 * b4 == pt::ApproxVec4(5.0f, 14.0f, 27.0f, 44.0f));
    }
    SECTION("scalar * vec") {
        REQUIRE(3.0f * a2 == pt::ApproxVec2(3.0f, 6.0f));
        REQUIRE(3.0f * a3 == pt::ApproxVec3(3.0f, 6.0f, 9.0f));
        REQUIRE(3.0f * a4 == pt::ApproxVec4(3.0f, 6.0f, 9.0f, 12.0f));
    }
    SECTION("vec * scalar") {
        REQUIRE(a2 * 3.0f == pt::ApproxVec2(3.0f, 6.0f));
        REQUIRE(a3 * 3.0f == pt::ApproxVec3(3.0f, 6.0f, 9.0f));
        REQUIRE(a4 * 3.0f == pt::ApproxVec4(3.0f, 6.0f, 9.0f, 12.0f));
    }
    SECTION("vec / vec") {
        REQUIRE(a2 / b2 == pt::ApproxVec2(0.33333333333f, 0.4f));
        REQUIRE(a3 / b3 == pt::ApproxVec3(0.33333333333f, 0.4f, 0.42857142857f));
        REQUIRE(a4 / b4 == pt::ApproxVec4(0.2f, 0.28571428571f, 0.33333333333f, 0.36363636363f));
    }
    SECTION("vec / scalar") {
        REQUIRE(a2 / 3.0f == pt::ApproxVec2(0.33333333333f, 0.66666666666f));
        REQUIRE(a3 / 3.0f == pt::ApproxVec3(0.33333333333f, 0.66666666666f, 1.0f));
        REQUIRE(a4 / 3.0f == pt::ApproxVec4(0.33333333333f, 0.66666666666f, 1.0f, 1.33333333333f));
    }
}


TEST_CASE("Vector Functions") {
    SECTION("dot") {
        REQUIRE(pt::dot(pt::Vec2(2.0f, 3.0f), pt::Vec2(3.0f, 1.0f)) == pt::Approx(9.0f));
        REQUIRE(pt::dot(pt::Vec3(2.0f, 3.0f, 4.0f), pt::Vec3(5.0f, 6.0f, 7.0f)) == pt::Approx(56.0f));
        REQUIRE(pt::dot(pt::Vec4(2.0f, 3.0f, 4.0f, 5.0f), pt::Vec4(5.0f, 6.0f, 7.0f, 8.0f)) == pt::Approx(96.0f));
    }

    SECTION("lengthSq") {
        REQUIRE(pt::lengthSq(pt::Vec2(2.0f, 3.0f)) == pt::Approx(13.0f));
        REQUIRE(pt::lengthSq(pt::Vec3(2.0f, 3.0f, 4.0f)) == pt::Approx(29.0f));
        REQUIRE(pt::lengthSq(pt::Vec4(2.0f, 3.0f, 4.0f, 5.0f)) == pt::Approx(54.0f));
    }

    SECTION("length") {
        REQUIRE(pt::length(pt::Vec2(2.0f, 3.0f)) == pt::Approx(3.6055512754639892931192212674705f));
        REQUIRE(pt::length(pt::Vec3(2.0f, 3.0f, 4.0f)) == pt::Approx(5.3851648071345040312507104915403f));
        REQUIRE(pt::length(pt::Vec4(2.0f, 3.0f, 4.0f, 5.0f)) == pt::Approx(7.3484692283495342945918522241177f));
    }

    SECTION("normalize") {
        REQUIRE(pt::normalize(pt::Vec2(2.0f, 3.0f)) == pt::ApproxVec2(0.55470019622f, 0.83205029433f));
        REQUIRE(pt::normalize(pt::Vec3(2.0f, 3.0f, 4.0f)) == pt::ApproxVec3(0.37139067635f, 0.55708601453f, 0.7427813527f));
        REQUIRE(pt::normalize(pt::Vec4(2.0f, 3.0f, 4.0f, 5.0f)) == pt::ApproxVec4(0.27216552697f, 0.40824829046f, 0.54433105395f, 0.68041381744f));
    }

    SECTION("isNormalized") {
        REQUIRE(pt::isNormalized(pt::Vec2(0.55470019622f, 0.83205029433f)));
        REQUIRE_FALSE(pt::isNormalized(pt::Vec2(1.55470019622f, 0.83205029433f)));

        REQUIRE(pt::isNormalized(pt::Vec3(0.37139067635f, 0.55708601453f, 0.7427813527f)));
        REQUIRE_FALSE(pt::isNormalized(pt::Vec3(1.37139067635f, 0.55708601453f, 0.7427813527f)));

        REQUIRE(pt::isNormalized(pt::Vec4(0.27216552697f, 0.40824829046f, 0.54433105395f, 0.68041381744f)));
        REQUIRE_FALSE(pt::isNormalized(pt::Vec4(1.27216552697f, 0.40824829046f, 0.54433105395f, 0.68041381744f)));
    }

    SECTION("cross") {
        auto vecA = pt::Vec3(1.0f, 0.0f, 0.0f);
        auto vecB = pt::Vec3(0.0f, 0.0f, 1.0f);
        auto vecC = pt::cross(vecB, vecA);

        REQUIRE(vecC == pt::ApproxVec3(0.0f, 1.0f, 0.0f));
        REQUIRE(pt::dot(vecA, vecC) == pt::Approx(0.0f));
        REQUIRE(pt::dot(vecB, vecC) == pt::Approx(0.0f));
        REQUIRE(pt::isNormalized(vecC));
    }
}

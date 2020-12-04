#pragma once

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

template <typename T>
struct ApproxMat4 {
    explicit constexpr ApproxMat4(
        T m11, T m12, T m13, T m14,
        T m21, T m22, T m23, T m24,
        T m31, T m32, T m33, T m34,
        T m41, T m42, T m43, T m44)
        : _11(m11), _12(m12), _13(m13), _14(m14)
        , _21(m21), _22(m22), _23(m23), _24(m24)
        , _31(m31), _32(m32), _33(m33), _34(m34)
        , _41(m41), _42(m42), _43(m43), _44(m44)
    {
    }

    friend constexpr bool operator==(const pt::Matrix4x4<T>& lhs, const ApproxMat4& rhs) {
        return lhs._11 == rhs._11 && lhs._12 == rhs._12 && lhs._13 == rhs._13 && lhs._14 == rhs._14
            && lhs._21 == rhs._21 && lhs._22 == rhs._22 && lhs._23 == rhs._23 && lhs._24 == rhs._24
            && lhs._31 == rhs._31 && lhs._32 == rhs._32 && lhs._33 == rhs._33 && lhs._34 == rhs._34
            && lhs._41 == rhs._41 && lhs._42 == rhs._42 && lhs._43 == rhs._43 && lhs._44 == rhs._44;
    }

    friend constexpr bool operator!=(const pt::Matrix4x4<T>& lhs, const ApproxMat4& rhs) {
        return !(lhs == rhs);
    }

    Approx<T> _11, _12, _13, _14;
    Approx<T> _21, _22, _23, _24;
    Approx<T> _31, _32, _33, _34;
    Approx<T> _41, _42, _43, _44;
};

} // namespace pt

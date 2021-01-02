#pragma once

#include "Vector3.h"
#include "Vector4.h"

#include <cmath>

namespace pt {

template <typename T>
struct Matrix4x4 {
    Matrix4x4() = default;
    explicit constexpr Matrix4x4(T s)
        : _11(s), _12(static_cast<T>(0)), _13(static_cast<T>(0)), _14(static_cast<T>(0))
        , _21(static_cast<T>(0)), _22(s), _23(static_cast<T>(0)), _24(static_cast<T>(0))
        , _31(static_cast<T>(0)), _32(static_cast<T>(0)), _33(s), _34(static_cast<T>(0))
        , _41(static_cast<T>(0)), _42(static_cast<T>(0)), _43(static_cast<T>(0)), _44(s)
    {
    }
    constexpr Matrix4x4(
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

    constexpr Vector4<T>& operator[](unsigned int row) { return rows[row]; }
    constexpr Vector4<T> operator[](unsigned int row) const { return rows[row]; }

    static constexpr Matrix4x4<T> fromRows(
        const Vector4<T>& row1,
        const Vector4<T>& row2,
        const Vector4<T>& row3,
        const Vector4<T>& row4)
    {
        Matrix4x4<T> m;
        m.rows[0] = row1;
        m.rows[1] = row2;
        m.rows[2] = row3;
        m.rows[3] = row4;
        return m;
    }

    static constexpr Matrix4x4<T> fromColumns(
        const Vector4<T>& col1,
        const Vector4<T>& col2,
        const Vector4<T>& col3,
        const Vector4<T>& col4)
    {
        return {
            col1.x, col2.x, col3.x, col4.x,
            col1.y, col2.y, col3.y, col4.y,
            col1.z, col2.z, col3.z, col4.z,
            col1.w, col2.w, col3.w, col4.w
        };
    }

    union {
        T data[4][4];
        Vector4<T> rows[4];
        struct {
            T _11, _12, _13, _14;
            T _21, _22, _23, _24;
            T _31, _32, _33, _34;
            T _41, _42, _43, _44;
        }; 
    };
};

template <typename T>
constexpr Matrix4x4<T> operator*(const Matrix4x4<T>& a, const Matrix4x4<T>& b) {
    return {
        a._11 * b._11 + a._12 * b._21 + a._13 * b._31 + a._14 * b._41,
        a._11 * b._12 + a._12 * b._22 + a._13 * b._32 + a._14 * b._42,
        a._11 * b._13 + a._12 * b._23 + a._13 * b._33 + a._14 * b._43,
        a._11 * b._14 + a._12 * b._24 + a._13 * b._34 + a._14 * b._44,

        a._21 * b._11 + a._22 * b._21 + a._23 * b._31 + a._24 * b._41,
        a._21 * b._12 + a._22 * b._22 + a._23 * b._32 + a._24 * b._42,
        a._21 * b._13 + a._22 * b._23 + a._23 * b._33 + a._24 * b._43,
        a._21 * b._14 + a._22 * b._24 + a._23 * b._34 + a._24 * b._44,

        a._31 * b._11 + a._32 * b._21 + a._33 * b._31 + a._34 * b._41,
        a._31 * b._12 + a._32 * b._22 + a._33 * b._32 + a._34 * b._42,
        a._31 * b._13 + a._32 * b._23 + a._33 * b._33 + a._34 * b._43,
        a._31 * b._14 + a._32 * b._24 + a._33 * b._34 + a._34 * b._44,

        a._41 * b._11 + a._42 * b._21 + a._43 * b._31 + a._44 * b._41,
        a._41 * b._12 + a._42 * b._22 + a._43 * b._32 + a._44 * b._42,
        a._41 * b._13 + a._42 * b._23 + a._43 * b._33 + a._44 * b._43,
        a._41 * b._14 + a._42 * b._24 + a._43 * b._34 + a._44 * b._44
    };
}

template <typename T>
constexpr Matrix4x4<T> operator*(const Matrix4x4<T>& a, T b) {
    Matrix4x4<T> r;
    r.rows[0] = a.rows[0] * b;
    r.rows[1] = a.rows[1] * b;
    r.rows[2] = a.rows[2] * b;
    r.rows[3] = a.rows[3] * b;
    return r;
}

template <typename T>
constexpr Matrix4x4<T> operator*(T a, const Matrix4x4<T>& b) {
    return b * a;
}

template <typename T>
constexpr Matrix4x4<T>& operator*=(Matrix4x4<T>& a, const Matrix4x4<T>& b) {
    a = a * b;
    return a;
}

template <typename T>
constexpr Matrix4x4<T>& operator*=(Matrix4x4<T>& a, T b) {
    a = a * b;
    return a;
}

template <typename T>
constexpr Matrix4x4<T> transpose(const Matrix4x4<T>& a) {
    return Matrix4x4<T>::fromColumns(a.rows[0], a.rows[1], a.rows[2], a.rows[3]);
}

template <typename T>
constexpr Vector3<T> transformPoint3x4(const Matrix4x4<T>& m, const Vector3<T>& p) {
    return {
        m._11 * p.x + m._12 * p.y + m._13 * p.z + m._14,
        m._21 * p.x + m._22 * p.y + m._23 * p.z + m._24,
        m._31 * p.x + m._32 * p.y + m._33 * p.z + m._34
    };
}

template <typename T>
constexpr Vector3<T> transformVector3x4(const Matrix4x4<T>& m, const Vector3<T>& v) {
    return {
        m._11 * v.x + m._12 * v.y + m._13 * v.z,
        m._21 * v.x + m._22 * v.y + m._23 * v.z,
        m._31 * v.x + m._32 * v.y + m._33 * v.z
    };
}

template <typename T>
constexpr Matrix4x4<T> lookAt(const Vector3<T>& eye, const Vector3<T>& at, const Vector3<T>& up) {
    const Vector3<T> z = normalize(eye - at);
    const Vector3<T> x = normalize(cross(up, z));
    const Vector3<T> y = cross(z, x);
    return {
        x.x, x.y, x.z, -dot(x, eye),
        y.x, y.y, y.z, -dot(y, eye),
        z.x, z.y, z.z, -dot(z, eye),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
    };
}

template <typename T>
constexpr Matrix4x4<T> translation(const Vector3<T>& t) {
    return {
        static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), t.x,
        static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), t.y,
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), t.z,
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
    };
}

template <typename T>
constexpr Matrix4x4<T> scaling(const Vector3<T>& s) {
    return {
        s.x,               static_cast<T>(0), static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), s.y,               static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), s.z,               static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
    };
}

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
constexpr Matrix4x4<T> rotationX(T radians) {
    T cos = std::cos(radians);
    T sin = std::sin(radians);
    return {
        static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), cos,              -sin,               static_cast<T>(0),
        static_cast<T>(0), sin,               cos,               static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
    };
}

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
constexpr Matrix4x4<T> rotationY(T radians) {
    T cos = std::cos(radians);
    T sin = std::sin(radians);
    return {
        cos,               static_cast<T>(0), sin,               static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(0),
        -sin,              static_cast<T>(0), cos,               static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
    };
}

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
constexpr Matrix4x4<T> rotationZ(T radians) {
    T cos = std::cos(radians);
    T sin = std::sin(radians);
    return {
        cos,              -sin,               static_cast<T>(0), static_cast<T>(0),
        sin,               cos,               static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
    };
}

using Mat4 = Matrix4x4<float>;

} // namespace pt

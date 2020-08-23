#pragma once

#include "MathUtils.h"

#include <cmath>

namespace pt {

template <typename T>
struct Vector4 {
    Vector4() = default;
    explicit constexpr Vector4(T s) : x(s), y(s), z(s), w(s) { }
    constexpr Vector4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) { }

    constexpr T& operator[](unsigned int index) { return data[index]; }
    constexpr T operator[](unsigned int index) const { return data[index]; }

    union {
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
        T data[4];
    };
};

template <typename T> constexpr Vector4<T>& operator+=(Vector4<T>& a, const Vector4<T>& b) { a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w; return a; }
template <typename T> constexpr Vector4<T>& operator-=(Vector4<T>& a, const Vector4<T>& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w; return a; }
template <typename T> constexpr Vector4<T>& operator*=(Vector4<T>& a, const Vector4<T>& b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w; return a; }
template <typename T> constexpr Vector4<T>& operator*=(Vector4<T>& a, T b) { a.x *= b; a.y *= b; a.z *= b; a.w *= b; return a; }
template <typename T> constexpr Vector4<T>& operator/=(Vector4<T>& a, const Vector4<T>& b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; a.w /= b.w; return a; }
template <typename T> constexpr Vector4<T>& operator/=(Vector4<T>& a, T b) { a.x /= b; a.y /= b; a.z /= b; a.w /= b; return a; }

template <typename T> constexpr Vector4<T> operator+(const Vector4<T>& a, const Vector4<T>& b) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
template <typename T> constexpr Vector4<T> operator-(const Vector4<T>& a) { return { -a.x, -a.y, -a.z, -a.w }; }
template <typename T> constexpr Vector4<T> operator-(const Vector4<T>& a, const Vector4<T>& b) { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
template <typename T> constexpr Vector4<T> operator*(const Vector4<T>& a, const Vector4<T>& b) { return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; }
template <typename T> constexpr Vector4<T> operator*(const Vector4<T>& a, T b) { return { a.x * b, a.y * b, a.z * b, a.w * b }; }
template <typename T> constexpr Vector4<T> operator*(T a, const Vector4<T>& b) { return b * a; }
template <typename T> constexpr Vector4<T> operator/(const Vector4<T>& a, const Vector4<T>& b) { return { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w }; }
template <typename T> constexpr Vector4<T> operator/(const Vector4<T>& a, T b) { return { a.x / b, a.y / b, a.z / b, a.w / b }; }

template <typename T>
constexpr Vector4<T> min(const Vector4<T>& a, const Vector4<T>& b) {
    return { min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w) };
}

template <typename T>
constexpr Vector4<T> max(const Vector4<T>& a, const Vector4<T>& b) {
    return { max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w) };
}

template <typename T>
constexpr T minComponent(const Vector4<T>& v) {
    return min(v.x, min(v.y, min(v.z, v.w)));
}

template <typename T>
constexpr T maxComponent(const Vector4<T>& v) {
    return max(v.x, max(v.y, max(v.z, v.w)));
}

template <typename T>
constexpr Vector4<T> clamp(const Vector4<T>& v, T minValue, T maxValue) {
    return {
        max(min(v.x, maxValue), minValue),
        max(min(v.y, maxValue), minValue),
        max(min(v.z, maxValue), minValue),
        max(min(v.w, maxValue), minValue)
    };
}

template <typename T>
constexpr Vector4<T> saturate(const Vector4<T>& v) {
    return clamp(v, static_cast<T>(0), static_cast<T>(1));
}

template <typename T>
inline Vector4<T> abs(const Vector4<T>& a) {
    return { std::abs(a.x), std::abs(a.y), std::abs(a.z), std::abs(a.w) };
}

template <typename T>
inline Vector4<T> floor(const Vector4<T>& a) {
    return { std::floor(a.x), std::floor(a.y), std::floor(a.z), std::floor(a.w) };
}

template <typename T>
inline Vector4<T> ceil(const Vector4<T>& a) {
    return { std::ceil(a.x), std::ceil(a.y), std::ceil(a.z), std::ceil(a.w) };
}

template <typename T>
constexpr Vector4<T> lerp(const Vector4<T>& a, const Vector4<T>& b, T t) {
    return (static_cast<T>(1) - t) * a + t * b;
}

template <typename T>
constexpr T dot(const Vector4<T>& a, const Vector4<T>& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

template <typename T>
constexpr T lengthSq(const Vector4<T>& a) {
    return dot(a, a);
}

template <typename T>
inline T length(const Vector4<T>& a) {
    return std::sqrt(dot(a, a));
}

template <typename T>
constexpr Vector4<T> normalize(const Vector4<T>& a) {
    return a * (static_cast<T>(1) / length(a));
}

template <typename T>
constexpr bool isNormalized(const Vector4<T>& a, T eps = static_cast<T>(1.0e-6)) {
    return abs(lengthSq(a) - static_cast<T>(1)) < eps;
}

using Vec4 = Vector4<float>;

} // namespace pt

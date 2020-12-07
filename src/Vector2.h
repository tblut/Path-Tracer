#pragma once

#include "MathUtils.h"

#include <ostream>
#include <cmath>

namespace pt {

template <typename T>
struct Vector2 {
    Vector2() = default;
    explicit constexpr Vector2(T s) : x(s), y(s) { }
    constexpr Vector2(T x_, T y_) : x(x_), y(y_) { }

    constexpr T& operator[](unsigned int index) { return data[index]; }
    constexpr T operator[](unsigned int index) const { return data[index]; }

    static Vector2<T> fromSpherical(T phi) {
        return { std::cos(phi), std::sin(phi) };
    }

    union {
        struct { T x, y; };
        struct { T r, g; };
        T data[2];
    };
};

template <typename T> constexpr Vector2<T>& operator+=(Vector2<T>& a, const Vector2<T>& b) { a.x += b.x; a.y += b.y; return a; }
template <typename T> constexpr Vector2<T>& operator-=(Vector2<T>& a, const Vector2<T>& b) { a.x -= b.x; a.y -= b.y; return a; }
template <typename T> constexpr Vector2<T>& operator*=(Vector2<T>& a, const Vector2<T>& b) { a.x *= b.x; a.y *= b.y; return a; }
template <typename T> constexpr Vector2<T>& operator*=(Vector2<T>& a, T b) { a.x *= b; a.y *= b; return a; }
template <typename T> constexpr Vector2<T>& operator/=(Vector2<T>& a, const Vector2<T>& b) { a.x /= b.x; a.y /= b.y; return a; }
template <typename T> constexpr Vector2<T>& operator/=(Vector2<T>& a, T b) { a.x /= b; a.y /= b; return a; }

template <typename T> constexpr Vector2<T> operator+(const Vector2<T>& a, const Vector2<T>& b) { return { a.x + b.x, a.y + b.y }; }
template <typename T> constexpr Vector2<T> operator-(const Vector2<T>& a) { return { -a.x, -a.y }; }
template <typename T> constexpr Vector2<T> operator-(const Vector2<T>& a, const Vector2<T>& b) { return { a.x - b.x, a.y - b.y }; }
template <typename T> constexpr Vector2<T> operator*(const Vector2<T>& a, const Vector2<T>& b) { return { a.x * b.x, a.y * b.y }; }
template <typename T> constexpr Vector2<T> operator*(const Vector2<T>& a, T b) { return { a.x * b, a.y * b }; }
template <typename T> constexpr Vector2<T> operator*(T a, const Vector2<T>& b) { return b * a; }
template <typename T> constexpr Vector2<T> operator/(const Vector2<T>& a, const Vector2<T>& b) { return { a.x / b.x, a.y / b.y }; }
template <typename T> constexpr Vector2<T> operator/(const Vector2<T>& a, T b) { return { a.x / b, a.y / b }; }

template <typename T>
std::ostream& operator<<(std::ostream& o, const Vector2<T>& v) {
    return o << "Vec2(" << v.x << ", " << v.y << ")";
}

template <typename T>
constexpr Vector2<T> min(const Vector2<T>& a, const Vector2<T>& b) {
    return { min(a.x, b.x), min(a.y, b.y) };
}

template <typename T>
constexpr Vector2<T> max(const Vector2<T>& a, const Vector2<T>& b) {
    return { max(a.x, b.x), max(a.y, b.y) };
}

template <typename T>
constexpr T minComponent(const Vector2<T>& v) {
    return min(v.x, v.y);
}

template <typename T>
constexpr T maxComponent(const Vector2<T>& v) {
    return max(v.x, v.y);
}

template <typename T>
constexpr Vector2<T> clamp(const Vector2<T>& v, T minValue, T maxValue) {
    return {
        max(min(v.x, maxValue), minValue),
        max(min(v.y, maxValue), minValue)
    };
}

template <typename T>
constexpr Vector2<T> saturate(const Vector2<T>& v) {
    return clamp(v, static_cast<T>(0), static_cast<T>(1));
}

template <typename T>
inline Vector2<T> abs(const Vector2<T>& a) {
    return { std::abs(a.x), std::abs(a.y) };
}

template <typename T>
inline Vector2<T> floor(const Vector2<T>& a) {
    return { std::floor(a.x), std::floor(a.y) };
}

template <typename T>
inline Vector2<T> ceil(const Vector2<T>& a) {
    return { std::ceil(a.x), std::ceil(a.y) };
}

template <typename T>
constexpr Vector2<T> lerp(const Vector2<T>& a, const Vector2<T>& b, T t) {
    return (static_cast<T>(1) - t) * a + t * b;
}

template <typename T>
constexpr T dot(const Vector2<T>& a, const Vector2<T>& b) {
    return a.x * b.x + a.y * b.y;
}

template <typename T>
constexpr T lengthSq(const Vector2<T>& a) {
    return dot(a, a);
}

template <typename T>
inline T length(const Vector2<T>& a) {
    return std::sqrt(dot(a, a));
}

template <typename T>
constexpr Vector2<T> normalize(const Vector2<T>& a) {
    return a * (static_cast<T>(1) / length(a));
}

template <typename T>
constexpr bool isNormalized(const Vector2<T>& a, T eps = static_cast<T>(1.0e-6)) {
    return abs(lengthSq(a) - static_cast<T>(1)) < eps;
}

using Vec2 = Vector2<float>;

} // namespace pt

#pragma once

#include <cmath>

namespace pt {

// Constants
template <typename T>
constexpr T pi = static_cast<T>(3.14159265358979323846);


// Types
template <typename T>
struct Vector3 {
    Vector3() = default;
    explicit constexpr Vector3(T s) : x(s), y(s), z(s) { }
    constexpr Vector3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) { }

    constexpr T& operator[](unsigned int index) { return data[index]; }
    constexpr T operator[](unsigned int index) const { return data[index]; }

    union {
        struct { T x, y, z; };
        T data[3];
    };
};


// Operators
template <typename T> constexpr Vector3<T>& operator+=(Vector3<T>& a, const Vector3<T>& b) { a.x += b.x; a.y += b.y; a.z += b.z; return a; }
template <typename T> constexpr Vector3<T>& operator-=(Vector3<T>& a, const Vector3<T>& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; }
template <typename T> constexpr Vector3<T>& operator*=(Vector3<T>& a, const Vector3<T>& b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; return a; }
template <typename T> constexpr Vector3<T>& operator*=(Vector3<T>& a, T b) { a.x *= b; a.y *= b; a.z *= b; return a; }
template <typename T> constexpr Vector3<T>& operator/=(Vector3<T>& a, const Vector3<T>& b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; return a; }
template <typename T> constexpr Vector3<T>& operator/=(Vector3<T>& a, T b) { a.x /= b; a.y /= b; a.z /= b; return a; }

template <typename T> constexpr Vector3<T> operator+(const Vector3<T>& a, const Vector3<T>& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
template <typename T> constexpr Vector3<T> operator-(const Vector3<T>& a) { return { -a.x, -a.y, -a.z }; }
template <typename T> constexpr Vector3<T> operator-(const Vector3<T>& a, const Vector3<T>& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
template <typename T> constexpr Vector3<T> operator*(const Vector3<T>& a, const Vector3<T>& b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }
template <typename T> constexpr Vector3<T> operator*(const Vector3<T>& a, T b) { return { a.x * b, a.y * b, a.z * b }; }
template <typename T> constexpr Vector3<T> operator*(T a, const Vector3<T>& b) { return b * a; }
template <typename T> constexpr Vector3<T> operator/(const Vector3<T>& a, const Vector3<T>& b) { return { a.x / b.x, a.y / b.y, a.z / b.z }; }
template <typename T> constexpr Vector3<T> operator/(const Vector3<T>& a, T b) { return { a.x / b, a.y / b, a.z / b }; }


// Common functions
using std::abs;
using std::floor;
using std::ceil;

template <typename T>
constexpr T min(T a, T b) {
    return a < b ? a : b;
}

template <typename T>
constexpr Vector3<T> min(const Vector3<T>& a, const Vector3<T>& b) {
    return { min(a.x, b.x), min(a.y, b.y), min(a.z, b.z) };
}

template <typename T>
constexpr T max(T a, T b) {
    return a > b ? a : b;
}

template <typename T>
constexpr Vector3<T> max(const Vector3<T>& a, const Vector3<T>& b) {
    return { max(a.x, b.x), max(a.y, b.y), max(a.z, b.z) };
}

template <typename T>
constexpr T clamp(T value, T minValue, T maxValue) {
    return max(min(value, maxValue), minValue);
}

template <typename T>
constexpr Vector3<T> clamp(const Vector3<T>& v, T minValue, T maxValue) {
    return {
        max(min(v.x, maxValue), minValue),
        max(min(v.y, maxValue), minValue),
        max(min(v.z, maxValue), minValue)
    };
}

template <typename T>
constexpr T saturate(T value) {
    return clamp(value, static_cast<T>(0), static_cast<T>(1));
}

template <typename T>
constexpr Vector3<T> saturate(const Vector3<T>& v) {
    return clamp(v, static_cast<T>(0), static_cast<T>(1));
}

template <typename T>
inline Vector3<T> abs(const Vector3<T>& a) {
    return { std::abs(a.x), std::abs(a.y), std::abs(a.z) };
}

template <typename T>
inline Vector3<T> floor(const Vector3<T>& a) {
    return { std::floor(a.x), std::floor(a.y), std::floor(a.z) };
}

template <typename T>
inline Vector3<T> ceil(const Vector3<T>& a) {
    return { std::ceil(a.x), std::ceil(a.y), std::ceil(a.z) };
}

template <typename T>
constexpr T degrees(T rad) {
    return rad * (static_cast<T>(180) / pi<T>);
}

template <typename T>
constexpr T radians(T deg) {
    return deg * (pi<T> / static_cast<T>(180));
}

template <typename T>
constexpr T lerp(T a, T b, T t) {
    return (static_cast<T>(1) - t) * a + t * b;
}

template <typename T>
constexpr Vector3<T> lerp(const Vector3<T>& a, const Vector3<T>& b, T t) {
    return (static_cast<T>(1) - t) * a + t * b;
}


// Vector functions
template <typename T>
constexpr T dot(const Vector3<T>& a, const Vector3<T>& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <typename T>
constexpr T lengthSq(const Vector3<T>& a) {
    return dot(a, a);
}

template <typename T>
inline T length(const Vector3<T>& a) {
    return std::sqrt(dot(a, a));
}

template <typename T>
constexpr Vector3<T> normalize(const Vector3<T>& a) {
    return a * (static_cast<T>(1) / length(a));
}

template <typename T>
constexpr Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

template <typename T>
constexpr Vector3<T> reflect(const Vector3<T>& i, const Vector3<T>& n) {
    return i - static_cast<T>(2) * n * dot(i, n);
}

template <typename T>
constexpr Vector3<T> refract(const Vector3<T>& i, const Vector3<T>& n, T iof) {
    const T dotNI = dot(n, i);
    const T k = static_cast<T>(1) - iof * iof * (static_cast<T>(1) - dotNI * dotNI);
    return k < static_cast<T>(0)
        ? Vector3<T>(static_cast<T>(0))
        : iof * i - (iof * dotNI + std::sqrt(k)) * n;
}


// Type aliases
using Vec3 = Vector3<float>;

} // namespace pt

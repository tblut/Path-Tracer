#pragma once

#include <cmath>

namespace pt {

template <typename T>
struct Vector3 {
    Vector3() = default;
    explicit constexpr Vector3(T s) : x(s), y(s), z(s) { }
    constexpr Vector3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) { }

    constexpr T& operator[](unsigned int index) { return data[index]; }
    constexpr T operator[](unsigned int index) const { return data[index]; }

    union {
        T data[3];
        struct { T x, y, z; };
    };
};


// Operators
template<typename T> constexpr Vector3<T>& operator+=(Vector3<T>& a, const Vector3<T>& b) { a.x += b.x; a.y += b.y; a.z += b.z; return a; }
template<typename T> constexpr Vector3<T>& operator-=(Vector3<T>& a, const Vector3<T>& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; }
template<typename T> constexpr Vector3<T>& operator*=(Vector3<T>& a, const Vector3<T>& b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; return a; }
template<typename T> constexpr Vector3<T>& operator*=(Vector3<T>& a, float b) { a.x *= b; a.y *= b; a.z *= b; return a; }
template<typename T> constexpr Vector3<T>& operator/=(Vector3<T>& a, const Vector3<T>& b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; return a; }
template<typename T> constexpr Vector3<T>& operator/=(Vector3<T>& a, float b) { a.x /= b; a.y /= b; a.z /= b; return a; }

template<typename T> constexpr Vector3<T> operator+(const Vector3<T>& a, const Vector3<T>& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
template<typename T> constexpr Vector3<T> operator-(const Vector3<T>& a) { return { -a.x, -a.y, -a.z }; }
template<typename T> constexpr Vector3<T> operator-(const Vector3<T>& a, const Vector3<T>& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
template<typename T> constexpr Vector3<T> operator*(const Vector3<T>& a, const Vector3<T>& b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }
template<typename T> constexpr Vector3<T> operator*(const Vector3<T>& a, float b) { return { a.x * b, a.y * b,   a.z * b }; }
template<typename T> constexpr Vector3<T> operator*(float a, const Vector3<T>& b) { return b * a; }
template<typename T> constexpr Vector3<T> operator/(const Vector3<T>& a, const Vector3<T>& b) { return { a.x / b.x, a.y / b.y, a.z / b.z }; }
template<typename T> constexpr Vector3<T> operator/(const Vector3<T>& a, float b) { return { a.x / b, a.y / b, a.z / b }; }


// Common functions
template <typename T> constexpr T min(T a, T b) { return b < a ? b : a; }
template <typename T> constexpr Vector3<T> min(const Vector3<T>& a, const Vector3<T>& b) {
    return { min(a.x, b.x), min(a.y, b.y), min(a.z, b.z) };
}

template <typename T> constexpr T max(T a, T b) { return a < b ? b : a; }
template <typename T> constexpr Vector3<T> max(const Vector3<T>& a, const Vector3<T>& b) {
    return { max(a.x, b.x), max(a.y, b.y), max(a.z, b.z) };
}

using std::abs;
template<typename T> inline Vector3<T> abs(const Vector3<T>& a) {
    return { std::abs(a.x), std::abs(a.y), std::abs(a.z) };
}

using std::floor;
template<typename T> inline Vector3<T> floor(const Vector3<T>& a) {
    return { std::floor(a.x), std::floor(a.y), std::floor(a.z) };
}

using std::ceil;
template<typename T> inline Vector3<T> ceil(const Vector3<T>& a) {
    return { std::ceil(a.x), std::ceil(a.y), std::ceil(a.z) };
}


// Vector functions
template<typename T> constexpr float dot(const Vector3<T>& a, const Vector3<T>& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<typename T> constexpr float lengthSq(const Vector3<T>& a) {
    return dot(a, a);
}

template<typename T> inline float length(const Vector3<T>& a) {
    return std::sqrt(dot(a, a));
}

template<typename T> constexpr Vector3<T> normalize(const Vector3<T>& a) {
    return a * (1.0f / length(a));
}

template<typename T> constexpr Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}


// Type aliases
using Vec3 = Vector3<float>;

} // namespace pt

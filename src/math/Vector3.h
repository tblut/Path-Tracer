#pragma once

namespace pt {

template <typename T>
struct Vector3 {
    Vector3() = default;
    explicit constexpr Vector3(T s) : x(s), y(s), z(s) { }
    constexpr Vector3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) { }

    constexpr T& operator[](unsigned int index) { return data[index]; }
    constexpr T operator[](unsigned int index) const { return data[index]; }

    union {
        struct { T x, y, z; };
        struct { T r, g, b; };
        T data[3];
    };
};

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

using Vec3 = Vector3<float>;

} // namespace pt

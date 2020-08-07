#pragma once

#include <cmath>

namespace pt {

template <typename T>
constexpr T pi = static_cast<T>(3.14159265358979323846);

using std::abs;
using std::floor;
using std::ceil;

template <typename T>
constexpr T min(T a, T b) {
    return a < b ? a : b;
}

template <typename T>
constexpr T max(T a, T b) {
    return a > b ? a : b;
}

template <typename T>
constexpr T clamp(T value, T minValue, T maxValue) {
    return max(min(value, maxValue), minValue);
}

template <typename T>
constexpr T saturate(T value) {
    return clamp(value, static_cast<T>(0), static_cast<T>(1));
}

template <typename T>
constexpr T lerp(T a, T b, T t) {
    return (static_cast<T>(1) - t) * a + t * b;
}

template <typename T>
constexpr T degrees(T rad) {
    return rad * (static_cast<T>(180) / pi<T>);
}

template <typename T>
constexpr T radians(T deg) {
    return deg * (pi<T> / static_cast<T>(180));
}

} // namespace pt
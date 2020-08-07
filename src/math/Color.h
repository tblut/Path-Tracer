#pragma once

#include "Vector3.h"

#include <cmath>

namespace pt {

template <typename T>
inline T linearToSRGB(T u) {
    return u <= static_cast<T>(0.0031308)
        ? static_cast<T>(12.92) * u
        : static_cast<T>(1.055) * std::pow(u, static_cast<T>(1.0 / 2.4)) - static_cast<T>(0.055);
}

template <typename T>
inline Vector3<T> linearToSRGB(const Vector3<T>& linear) {
    return { linearToSRGB(linear.r), linearToSRGB(linear.g), linearToSRGB(linear.b) };
}

template <typename T>
inline T srgbToLinear(T u) {
    return u <= static_cast<T>(0.04045)
        ? u / static_cast<T>(12.92)
        : std::pow((u + static_cast<T>(0.055)) / static_cast<T>(1.055), static_cast<T>(2.4));
}

template <typename T>
inline Vector3<T> srgbToLinear(const Vector3<T>& srgb) {
    return { srgbToLinear(srgb.r), srgbToLinear(srgb.g), srgbToLinear(srgb.b) };
}

} // namespace pt

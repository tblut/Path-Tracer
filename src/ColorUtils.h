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

template <typename T>
inline Vector3<T> tonemapReinhard(const Vector3<T>& hdrSrgb) {
    return saturate(hdrSrgb / (Vector3<T>(static_cast<T>(1)) + hdrSrgb));
}

// See: https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl
template <typename T>
inline Vector3<T> tonemapACES(const Vector3<T>& hdrSrgb) {
    Vector3<T> result = Vector3<T>(
        static_cast<T>(0.59719) * hdrSrgb.r + static_cast<T>(0.35458) * hdrSrgb.g + static_cast<T>(0.04823) * hdrSrgb.b,
        static_cast<T>(0.07600) * hdrSrgb.r + static_cast<T>(0.90834) * hdrSrgb.g + static_cast<T>(0.01566) * hdrSrgb.b,
        static_cast<T>(0.02840) * hdrSrgb.r + static_cast<T>(0.13383) * hdrSrgb.g + static_cast<T>(0.83777) * hdrSrgb.b
    );

    const Vector3<T> a = result * (result + Vector3<T>(static_cast<T>(0.0245786))) - Vector3<T>(static_cast<T>(0.000090537));
    const Vector3<T> b = result * (static_cast<T>(0.983729) * result + Vector3<T>(static_cast<T>(0.4329510))) + Vector3<T>(static_cast<T>(0.238081));
    result = a / b;

    result = Vector3<T>(
        static_cast<T>(1.60475) * result.r + static_cast<T>(-0.53108) * result.g + static_cast<T>(-0.07367) * result.b,
        static_cast<T>(-0.10208) * result.r + static_cast<T>(1.10813) * result.g + static_cast<T>(-0.00605) * result.b,
        static_cast<T>(-0.00327) * result.r + static_cast<T>(-0.07276) * result.g + static_cast<T>(1.07602) * result.b
    );

    return saturate(result);
}

} // namespace pt

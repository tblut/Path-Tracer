#pragma once

#include "MathUtils.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#include <ostream>
#include <cmath>

namespace pt {

template <typename T>
constexpr T testEps = static_cast<T>(1e-8);

template <typename T>
struct Approx {
    explicit constexpr Approx(T value_) : value(value_), eps(testEps<T>) { }

    Approx& epsilon(T eps_) {
        eps = eps_;
        return *this;
    }

    friend constexpr bool operator==(T lhs, const Approx& rhs) {
        return std::abs(lhs - rhs.value) < rhs.eps;
    }

    friend constexpr bool operator!=(T lhs, const Approx& rhs) {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& o, const Approx& v) {
        return o << "Approx(" << v.value << ")";
    }

    T value;
    T eps;
};

template <typename T>
struct ApproxVec2 {
    explicit constexpr ApproxVec2(T x_, T y_) : x(x_), y(y_) { }

    ApproxVec2& epsilon(T eps_) {
        x.epsilon(eps_);
        y.epsilon(eps_);
        return *this;
    }

    friend constexpr bool operator==(const pt::Vector2<T>& lhs, const ApproxVec2& rhs) {
        return lhs.x == rhs.x
            && lhs.y == rhs.y;
    }

    friend constexpr bool operator!=(const pt::Vector2<T>& lhs, const ApproxVec2& rhs) {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& o, const ApproxVec2& v) {
        return o << "ApproxVec2(" << v.x.value << ", " << v.y.value << ")";
    }

    Approx<T> x, y;
};

template <typename T>
struct ApproxVec3 {
    explicit constexpr ApproxVec3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) { }

    ApproxVec3& epsilon(T eps_) {
        x.epsilon(eps_);
        y.epsilon(eps_);
        z.epsilon(eps_);
        return *this;
    }

    friend constexpr bool operator==(const pt::Vector3<T>& lhs, const ApproxVec3& rhs) {
        return lhs.x == rhs.x
            && lhs.y == rhs.y
            && lhs.z == rhs.z;
    }

    friend constexpr bool operator!=(const pt::Vector3<T>& lhs, const ApproxVec3& rhs) {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& o, const ApproxVec3& v) {
        return o << "ApproxVec3(" << v.x.value << ", " << v.y.value << ", " << v.z.value << ")";
    }

    Approx<T> x, y, z;
};

template <typename T>
struct ApproxVec4 {
    explicit constexpr ApproxVec4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) { }

    ApproxVec4& epsilon(T eps_) {
        x.epsilon(eps_);
        y.epsilon(eps_);
        z.epsilon(eps_);
        w.epsilon(eps_);
        return *this;
    }

    friend constexpr bool operator==(const pt::Vector4<T>& lhs, const ApproxVec4& rhs) {
        return lhs.x == rhs.x
            && lhs.y == rhs.y
            && lhs.z == rhs.z
            && lhs.w == rhs.w;
    }

    friend constexpr bool operator!=(const pt::Vector4<T>& lhs, const ApproxVec4& rhs) {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& o, const ApproxVec4& v) {
        return o << "ApproxVec4(" << v.x.value << ", " << v.y.value << ", " << v.z.value << ", " << v.w.value << ")";
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

inline double incompleteLowerGamma(double s, double z) {
    double x = std::pow(z, s) * std::exp(-z) * (1.0 / s);
    double nominator = 1.0;
    double denomiator = 1.0;
    double sum = 1.0;

    for (int i = 1; ; i++) {
        nominator *= z;
        denomiator *= (s + i);
        sum += nominator / denomiator;

        // Test if machine epsilon precision has been reached
        if (sum == sum + (nominator / denomiator)) {
            break;
        }
    }

    return x * sum;
}


inline double chi2cdf(int k, double x) {
    if (k < 1 || x < 0.0) {
        return 0.0;
    }
    else if (k == 2) {
        return 1.0 - std::exp(-x / 2.0);
    }

    double lowerGamma = incompleteLowerGamma(k / 2.0, x / 2.0);
    if (std::isnan(lowerGamma) || std::isinf(lowerGamma)) {
        return 1.0 - std::numeric_limits<double>::epsilon();
    }

    return lowerGamma / std::tgamma(k / 2.0);
}

} // namespace pt

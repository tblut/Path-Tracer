#include "Camera.h"
#include "Vector2.h"

#include <cmath>

namespace {

using namespace pt;

Mat4 computeInverseViewMatrix(const Mat4& viewMatrix) {
    Mat4 viewMatrixInv = transpose(viewMatrix);
    Vec3 cameraTranslation(viewMatrix[0].w, viewMatrix[1].w, viewMatrix[2].w);
    Vec3 cameraOrigin = transformVector3x4(viewMatrixInv, -cameraTranslation);
    viewMatrixInv[0].w = cameraOrigin.x;
    viewMatrixInv[1].w = cameraOrigin.y;
    viewMatrixInv[2].w = cameraOrigin.z;
    viewMatrixInv[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    return viewMatrixInv;
}

Vec2 sampleCocentricDisk(float u1, float u2) {
    Vec2 offset = 2.0f * Vec2(u1, u2) - Vec2(1.0f, 1.0f);
    if (offset.x == 0.0f && offset.y == 0.0f) {
        return Vec2(0.0f, 0.0f);
    }

    float theta, r;
    if (abs(offset.x) > abs(offset.y)) {
        r = offset.x;
        theta = pi<float> * 0.25f * (offset.y / offset.x);
    }
    else {
        r = offset.y;
        theta = pi<float> * 0.5f - pi<float> * 0.25f * (offset.x / offset.y);
    }

    return r * Vec2(std::cos(theta), std::sin(theta));
}

} // namespace


namespace pt {

Camera::Camera(float fovY, float aspect, float aperture, float focalDistance, const Mat4& viewMatrix)
    : aperture_(aperture)
    , focalDistance_(focalDistance)
{
    viewMatrixInv_ = computeInverseViewMatrix(viewMatrix);
    scaleY_ = std::tan(fovY * 0.5f);
    scaleX_ = scaleY_ * aspect;
}

Ray Camera::generateRay(float u, float v, float u1, float u2) const {
    float pu = (2.0f * u - 1.0f) * scaleX_;
    float pv = (1.0f - 2.0f * v) * scaleY_;
    Vec3 rayOrigin(0.0f);
    Vec3 rayDirection = normalize(Vec3(pu, pv, -1.0f));

    if (aperture_ > 0.0f) {
        Vec2 lensPoint = aperture_ * sampleCocentricDisk(u1, u2);
        float focusDistance = focalDistance_ / abs(rayDirection.z);
        Vec3 focusPoint = rayOrigin + focusDistance * rayDirection;
        rayOrigin = Vec3(lensPoint.x, lensPoint.y, 0.0f);
        rayDirection = normalize(focusPoint - rayOrigin);
    }

    rayOrigin = transformPoint3x4(viewMatrixInv_, rayOrigin);
    rayDirection = transformVector3x4(viewMatrixInv_, rayDirection);
    return Ray(rayOrigin, rayDirection);
}

} // namespace pt

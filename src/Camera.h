#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"
#include "Ray.h"

namespace pt {

class Camera {
public:
    Camera(float fovY, float aspect, float aperture, float focalDistance, const Mat4& viewMatrix);
    
    Ray generateRay(float u, float v, float u1, float u2) const;

private:
    Mat4 viewMatrixInv_;
    float scaleX_;
    float scaleY_;
    float aperture_;
    float focalDistance_;
};

} // namespace pt

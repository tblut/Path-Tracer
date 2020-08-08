#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"
#include "Ray.h"

#include <cmath>

namespace pt {

class Camera {
public:
    Camera(float fovY, float aspect, const Mat4& viewMatrix) {
        viewMatrixTr_ = transpose(viewMatrix);
        Vec3 cameraTranslation(viewMatrix[0].w, viewMatrix[1].w, viewMatrix[2].w);
        origin_ = transformVector3x4(viewMatrixTr_, -cameraTranslation);
        scaleY_ = std::tan(fovY * 0.5f);
        scaleX_ = scaleY_ * aspect;
    }
    
    Ray generateRay(float u, float v) const {
        float pu = (2.0f * u - 1.0f) * scaleX_;
        float pv = (2.0f * v - 1.0f) * scaleY_;
        Vec3 direction = transformVector3x4(viewMatrixTr_, Vec3(pu, pv, -1.0f));
        return Ray(origin_, normalize(direction));
    }

private:
    Vec3 origin_;
    Mat4 viewMatrixTr_;
    float scaleX_;
    float scaleY_;
};

} // namespace pt

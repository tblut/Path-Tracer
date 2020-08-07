#pragma once

#include "Vector3.h"
#include "Ray.h"

namespace pt {

class Camera;
class Film;

class Renderer {
public:
    void render(const Camera& camera, Film& film);

private:
    Vec3 radiance(const Ray& ray);
};

} // namespace pt

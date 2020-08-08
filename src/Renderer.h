#pragma once

#include "Vector3.h"
#include "Ray.h"

#include <cstdint>

namespace pt {

class Camera;
class Film;

class Renderer {
public:
    void render(const Camera& camera, Film& film, uint32_t spp);

private:
    Vec3 radiance(const Ray& ray);
};

} // namespace pt

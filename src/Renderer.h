#pragma once

#include "Vector3.h"
#include "Ray.h"

#include <cstdint>

namespace pt {

class Scene;
class Camera;
class Film;

class Renderer {
public:
    void render(const Scene& scene, const Camera& camera, Film& film, uint32_t spp);

private:
    Vec3 radiance(const Ray& ray, const Scene& scene);
};

} // namespace pt

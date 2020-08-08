#pragma once

#include "Vector3.h"
#include "Ray.h"
#include "RandomSeries.h"

#include <cstdint>

namespace pt {

class Scene;
class Camera;
class Film;

class Renderer {
public:
    void render(const Scene& scene, const Camera& camera, Film& film);

    void setMaxDepth(uint32_t depth) { maxDepth_ = depth; }
    void setSamplesPerPixel(uint32_t spp) { samplesPerPixel_ = spp; }

private:
    Vec3 radiance(const Scene& scene, const Ray& ray, Vec3 lambda, uint32_t depth = 0);

    uint32_t maxDepth_ = 10;
    uint32_t samplesPerPixel_ = 16;
    RandomSeries rng_;
};

} // namespace pt

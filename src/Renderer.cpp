#include "Renderer.h"
#include "Camera.h"
#include "Film.h"

#include "Sphere.h"
#include "RandomSeries.h"

namespace pt {

void Renderer::render(const Camera& camera, Film& film, uint32_t spp) {
    RandomSeries rng;
    for (uint32_t y = 0; y < film.getHeight(); y++) {
        for (uint32_t x = 0; x < film.getWidth(); x++) {
            for (uint32_t s = 0; s < spp; s++) {
                float u = (x + rng.uniformFloat()) / static_cast<float>(film.getWidth() - 1);
                float v = (y + rng.uniformFloat()) / static_cast<float>(film.getHeight() - 1);
                Ray ray = camera.generateRay(u, v);
                film.addSample(x, y, radiance(ray));
            }
        }
    }
}

Vec3 Renderer::radiance(const Ray& ray) {
    Sphere sphere(Vec3(0, 0, -2), 1);
    RayHit hit = intersect(ray, sphere);
    if (hit.t >= 0.0f) {
        return Vec3(1, 0, 0);
    }
    return Vec3(1);
}

} // namespace pt

#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "Film.h"
#include "RandomSeries.h"
#include "Material.h"

namespace pt {

void Renderer::render(const Scene& scene, const Camera& camera, Film& film, uint32_t spp) {
    RandomSeries rng;
    for (uint32_t y = 0; y < film.getHeight(); y++) {
        for (uint32_t x = 0; x < film.getWidth(); x++) {
            for (uint32_t s = 0; s < spp; s++) {
                float u = (x + rng.uniformFloat()) / static_cast<float>(film.getWidth() - 1);
                float v = (y + rng.uniformFloat()) / static_cast<float>(film.getHeight() - 1);
                Ray ray = camera.generateRay(u, v);
                film.addSample(x, y, radiance(ray, scene));
            }
        }
    }
}

Vec3 Renderer::radiance(const Ray& ray, const Scene& scene) {
    RayHit hit = scene.intersect(ray);
    if (hit.t >= 0.0f) {
        return hit.material->baseColor;
    }

    return Vec3(1);
}

} // namespace pt

#include "Renderer.h"
#include "Camera.h"
#include "Film.h"

namespace pt {

void Renderer::render(const Camera& camera, Film& film) {
    for (uint32_t y = 0; y < film.getHeight(); y++) {
        for (uint32_t x = 0; x < film.getWidth(); x++) {
            float u = x / static_cast<float>(film.getWidth());
            float v = y / static_cast<float>(film.getHeight());
            Ray ray = camera.generateRay(u, v);
            film.addSample(x, y, radiance(ray));
        }
    }
}

Vec3 Renderer::radiance(const Ray& ray) {
    Vec3 unit_direction = normalize(ray.direction);
    float t = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f - t) * Vec3(1, 1, 1) + t * Vec3(0.5f, 0.7f, 1.0f);
}

} // namespace pt

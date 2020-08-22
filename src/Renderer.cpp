#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "RandomSeries.h"
#include "Material.h"
#include "Shape.h"
#include "OrthonormalBasis.h"

namespace pt {

void Renderer::render(const Scene& scene, const Camera& camera, Film& film) {
    auto filmTiles = film.getTiles(tileWidth_, tileHeight_);
    uint32_t numThreads = std::thread::hardware_concurrency();
    workerThreads_.reserve(numThreads);

    for (uint32_t i = 0; i < numThreads; i++) {
        workerThreads_.emplace_back([&] {
            workerThreadMain(i, scene, camera, film, filmTiles);
        });
    }

    for (std::thread& thread : workerThreads_) {
        thread.join();
    }
    workerThreads_.clear();
}

void Renderer::workerThreadMain(uint32_t id, const Scene& scene,
        const Camera& camera, Film& film, std::vector<Film::Tile>& filmTiles) {
    RandomSeries rng;
    rng.seed(RandomSeries::defaultState * (id + 1),
        RandomSeries::defaultInc * (id - 1));

    while (true) {
        Film::Tile tile;
        {
            std::scoped_lock<std::mutex> lock(mutex_);
            if (filmTiles.empty()) {
                break;
            }
            tile = filmTiles.back();
            filmTiles.pop_back();
        }

        for (uint32_t y = tile.startY; y <= tile.endY; y++) {
            for (uint32_t x = tile.startX; x <= tile.endX; x++) {
                for (uint32_t s = 0; s < samplesPerPixel_; s++) {
                    float u = (x + rng.uniformFloat()) / static_cast<float>(film.getWidth() - 1);
                    float v = (y + rng.uniformFloat()) / static_cast<float>(film.getHeight() - 1);
                    Ray ray = camera.generateRay(u, v);
                    Vec3 color = radiance(scene, rng, ray, Vec3(1.0f));
                    film.addSample(x, y, color);
                }
            }
        }
    }
}

Vec3 Renderer::radiance(const Scene& scene, RandomSeries& rng, const Ray& ray, Vec3 lambda, uint32_t depth) {
    if (depth > maxDepth_) {
        return Vec3(0.0f);
    }

    RayHit hit = scene.intersect(ray);
    if (hit.t < 0.0f) {
        return backgroundColor_ * lambda;
    }
    if (hit.shape->isLight() && depth == 0) {
        return lambda * hit.shape->material->emittance;
    }

    Vec3 normal = hit.normal;
    Vec3 intersectionPoint = ray.at(hit.t) + hit.normal * 0.001f;
    Vec3 wo = normalize(-ray.direction);
    const Material* material = hit.shape->material;

    Vec3 color(0.0f);
    for (auto light : scene.getLights()) {
        float lightPdf;
        Vec3 wi = light->sampleDirection(intersectionPoint,
            rng.uniformFloat(), rng.uniformFloat(), &lightPdf);

        float cosTheta = dot(wi, hit.normal);
        if (cosTheta <= 0.0f) {
            continue;
        }

        RayHit lightHit = scene.intersect(Ray(intersectionPoint, wi));
        if (lightHit.shape != light || lightHit.shape == hit.shape) {
            continue;
        }

        Vec3 brdf = material->evaluate(wi, wo, normal);

        // TODO: Multiple importance sampling

        assert(lightPdf > 0.0f && !std::isinf(lightPdf) && !std::isnan(lightPdf));
        color += lambda * light->material->emittance * brdf * cosTheta / lightPdf;
    }

    float pdf;
    Vec3 wi = material->sampleDirection(wo, normal, rng.uniformFloat(), rng.uniformFloat(), &pdf);
    float cosTheta = dot(normal, wi);
    if (cosTheta <= 0.0f) {
        return color;
    }

    assert(pdf > 0.0f && !std::isinf(pdf) && !std::isnan(pdf));
    Vec3 brdf = material->evaluate(wi, wo, normal);
    lambda *= brdf * cosTheta / pdf;
    
    float rrProb = max(lambda.r, max(lambda.g, lambda.b));
    if (depth < minRRDepth_ || rng.uniformFloat() <= rrProb) {
        if (depth >= minRRDepth_) {
            assert(rrProb > 0.0f);
            lambda /= rrProb;
        }

        Ray newRay(intersectionPoint, wi);
        color += lambda * radiance(scene, rng, newRay, lambda, depth + 1);
    }

    // TODO: Radiance clamping after N bounces?
    return color;
}

} // namespace pt

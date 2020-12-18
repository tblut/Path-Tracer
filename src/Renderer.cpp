#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "RandomSeries.h"
#include "Material.h"
#include "Shape.h"
#include "OrthonormalBasis.h"
#include "BSDF.h"

namespace {

float powerHeuristic(int nf, float pdfF, int ng, float pdfG) {
    float f = nf * pdfF;
    float g = ng * pdfG;
    float f2 = f * f;
    return f2 / (f2 + g * g);
}

} // namespace

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
                    Ray ray = camera.generateRay(u, v, rng.uniformFloat(), rng.uniformFloat());
                    Vec3 color = radiance(scene, rng, ray, Vec3(1.0f));
                    assert(!std::isnan(color.r) && !std::isnan(color.g) && !std::isnan(color.b));
                    assert(!std::isinf(color.r) && !std::isinf(color.g) && !std::isinf(color.b));
                    assert(color.r >= 0.0f && color.g >= 0.0f && color.b >= 0.0f);
                    film.addSample(x, y, color);
                }
            }
        }
    }
}

Vec3 Renderer::radiance(const Scene& scene, RandomSeries& rng,
        const Ray& ray, Vec3 lambda, uint32_t depth) const {
    if (depth > maxDepth_) {
        return Vec3(0.0f);
    }

    RayHit hit = scene.intersect(ray);
    if (hit.t < 0.0f) {
        Vec3 c = lambda * backgroundColor_;
        assert(!std::isnan(c.r) && !std::isnan(c.g) && !std::isnan(c.b));
        assert(!std::isinf(c.r) && !std::isinf(c.g) && !std::isinf(c.b));
        assert(c.r >= 0.0f && c.g >= 0.0f && c.b >= 0.0f);
        return c;
    }
    if (hit.shape->isLight() && depth == 0) {
        Vec3 c = lambda * hit.shape->material->getEmittance();
        assert(!std::isnan(c.r) && !std::isnan(c.g) && !std::isnan(c.b));
        assert(!std::isinf(c.r) && !std::isinf(c.g) && !std::isinf(c.b));
        assert(c.r >= 0.0f && c.g >= 0.0f && c.b >= 0.0f);
        return c;
    }

    Vec3 normal = hit.normal;
    Vec3 wo = normalize(-ray.direction);
    const Material* material = hit.shape->material;
   
    OrthonormalBasis basis(normal);
    wo = basis.worldToLocal(wo);
    normal = basis.worldToLocal(normal);
    Vec3 intersectionPoint = ray.at(hit.t);

    Vec3 color(0.0f);
    { // Direct illuminaton
        size_t lightIndex = static_cast<size_t>(rng.uniformFloat() * scene.getNumLights());
        const Shape* light = scene.getLights()[lightIndex];

        // MIS light sampling
        float lightPdf;
        Vec3 lightDir = light->sampleDirection(intersectionPoint,
            rng.uniformFloat(), rng.uniformFloat(), &lightPdf);
        Vec3 wi = basis.worldToLocal(lightDir);

        float dotNL = abs(cosTheta(wi));
        if (dotNL > 0.0f && lightPdf > 0.0f) {
            Ray lightRay;
            lightRay.origin = cosTheta(wi) > 0.0f
                ? intersectionPoint + hit.normal * 0.001f
                : intersectionPoint - hit.normal * 0.001f;
            lightRay.direction = lightDir;

            RayHit lightHit = scene.intersect(lightRay);
            if (lightHit.shape == light && lightHit.shape != hit.shape) {
                Vec3 brdf = material->evaluate(wi, wo, normal);
                float brdfPdf = material->pdf(wi, wo, normal);
                if (brdfPdf > 0.0f) {
                    float misWeight = powerHeuristic(1, lightPdf, 1, brdfPdf);
                    assert(lightPdf > 0.0f && !std::isinf(lightPdf) && !std::isnan(lightPdf));
                    assert(brdfPdf > 0.0f && !std::isinf(brdfPdf) && !std::isnan(brdfPdf));
                    color += lambda * light->material->getEmittance() * brdf * dotNL * misWeight / lightPdf;
                    assert(!std::isnan(color.r) && !std::isnan(color.g) && !std::isnan(color.b));
                    assert(!std::isinf(color.r) && !std::isinf(color.g) && !std::isinf(color.b));
                    assert(color.r >= 0.0f && color.g >= 0.0f && color.b >= 0.0f);
                }
            }
        }

        // MIS brdf sampling
        float brdfPdf;
        wi = material->sampleDirection(wo, normal, rng.uniformFloat(), rng.uniformFloat(), &brdfPdf);
        dotNL = abs(cosTheta(wi));
        if (dotNL > 0.0f && brdfPdf > 0.0f) {
            Ray lightRay;
            lightRay.origin = cosTheta(wi) > 0.0f
                ? intersectionPoint + hit.normal * 0.001f
                : intersectionPoint - hit.normal * 0.001f;
            lightRay.direction = basis.localToWorld(wi);

            RayHit lightHit = scene.intersect(lightRay);
            if (lightHit.shape == light && lightHit.shape != hit.shape) {
                Vec3 brdf = material->evaluate(wi, wo, normal);
                lightPdf = light->pdf(intersectionPoint);
                if (lightPdf > 0.0f) {
                    float misWeight = powerHeuristic(1, brdfPdf, 1, lightPdf);
                    assert(lightPdf > 0.0f && !std::isinf(lightPdf) && !std::isnan(lightPdf));
                    assert(brdfPdf > 0.0f && !std::isinf(brdfPdf) && !std::isnan(brdfPdf));
                    color += lambda * light->material->getEmittance() * brdf * dotNL * misWeight / brdfPdf;
                    assert(!std::isnan(color.r) && !std::isnan(color.g) && !std::isnan(color.b));
                    assert(!std::isinf(color.r) && !std::isinf(color.g) && !std::isinf(color.b));
                    assert(color.r >= 0.0f && color.g >= 0.0f && color.b >= 0.0f);
                }
            }
        }

        float lightProb = 1.0f / scene.getNumLights();
        color /= lightProb;
    }

    // Indirect illumination
    float pdf;
    Vec3 wi = material->sampleDirection(wo, normal, rng.uniformFloat(), rng.uniformFloat(), &pdf);
    float dotNL = abs(cosTheta(wi));
    if (dotNL <= 0.0f || pdf <= 0.0f) {
        assert(!std::isnan(color.r) && !std::isnan(color.g) && !std::isnan(color.b));
        assert(!std::isinf(color.r) && !std::isinf(color.g) && !std::isinf(color.b));
        assert(color.r >= 0.0f && color.g >= 0.0f && color.b >= 0.0f);
        return color;
    }

    assert(pdf > 0.0f && !std::isinf(pdf) && !std::isnan(pdf));
    Vec3 brdf = material->evaluate(wi, wo, normal);
    assert(lengthSq(brdf) > 0.0f);
    lambda *= brdf * dotNL / pdf;
    
    float rrProb = max(lambda.r, max(lambda.g, lambda.b));
    if (depth < minRRDepth_ || rng.uniformFloat() <= rrProb) {
        if (depth >= minRRDepth_) {
            assert(rrProb > 0.0f);
            lambda /= rrProb;
        }

        Ray newRay;
        newRay.origin = cosTheta(wi) > 0.0f
            ? intersectionPoint + hit.normal * 0.001f
            : intersectionPoint - hit.normal * 0.001f;
        newRay.direction = basis.localToWorld(wi);

        color += lambda * radiance(scene, rng, newRay, lambda, depth + 1);
        assert(!std::isnan(color.r) && !std::isnan(color.g) && !std::isnan(color.b));
        assert(!std::isinf(color.r) && !std::isinf(color.g) && !std::isinf(color.b));
        assert(color.r >= 0.0f && color.g >= 0.0f && color.b >= 0.0f);
    }

    // TODO: Radiance clamping after N bounces?
    return color;
}

} // namespace pt

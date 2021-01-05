#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "RandomSeries.h"
#include "Material.h"
#include "Shape.h"
#include "OrthonormalBasis.h"
#include "BSDF.h"
#include "ProgressBar.h"

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
    std::atomic<size_t> nextTileIndex = 0;
    uint32_t numThreads = max(1u, std::thread::hardware_concurrency());
    workerThreads_.reserve(numThreads);

    ProgressBar progressBar(filmTiles.size(), "Rendering");
    for (uint32_t i = 0; i < numThreads; i++) {
        workerThreads_.emplace_back([&] {
            workerThreadMain(i, scene, camera, film,
                filmTiles, nextTileIndex, progressBar);
        });
    }

    for (std::thread& thread : workerThreads_) {
        thread.join();
    }
    workerThreads_.clear();
}

void Renderer::workerThreadMain(uint32_t id, const Scene& scene,
        const Camera& camera, Film& film, std::vector<Film::Tile>& filmTiles,
        std::atomic<size_t>& nextTileIndex, ProgressBar& progressBar) {
    RandomSeries rng;
    rng.seed(RandomSeries::defaultState * (id + 1),
        RandomSeries::defaultInc * (id - 1));

    while (true) {
        size_t tileIndex = nextTileIndex.fetch_add(1, std::memory_order_relaxed);
        if (tileIndex >= filmTiles.size()) {
            break;
        }
        Film::Tile tile = filmTiles[tileIndex];

        for (uint32_t y = tile.startY; y <= tile.endY; y++) {
            for (uint32_t x = tile.startX; x <= tile.endX; x++) {
                for (uint32_t s = 0; s < samplesPerPixel_; s++) {
                    float u = (x + rng.uniformFloat()) / static_cast<float>(film.getWidth() - 1);
                    float v = (y + rng.uniformFloat()) / static_cast<float>(film.getHeight() - 1);
                    Ray ray = camera.generateRay(u, v, rng.uniformFloat(), rng.uniformFloat());
                    Vec3 color = radiance(scene, rng, ray);
                    assert(isFinite(color) && color.r >= 0.0f && color.g >= 0.0f && color.b >= 0.0f);
                    film.addSample(x, y, color);
                }
            }
        }

        progressBar.update();
    }
}

Vec3 Renderer::radiance(const Scene& scene, RandomSeries& rng, Ray ray) const {
    Vec3 lambda(1.0f);
    Vec3 color(0.0f);
    RayHit hit = scene.intersect(ray);

    for (uint32_t depth = 0; depth <= maxDepth_; depth++) {
        if (hit.t < 0.0f) {
            color += lambda * backgroundColor_;
            break;
        }
        if (hit.shape->isLight() && depth == 0) {
            color += lambda * hit.shape->material->getEmittance();
            break;
        }

        const Material* material = hit.shape->material;
        Vec3 intersectionPoint = ray.at(hit.t);
        Vec3 wo = normalize(-ray.direction);
        OrthonormalBasis basis(hit.normal);
        wo = basis.worldToLocal(wo);

        // Sample a single light source
        size_t lightIndex = static_cast<size_t>(rng.uniformFloat() * scene.getNumLights());
        const Shape* light = scene.getLights()[lightIndex];
        float lightProb = 1.0f / scene.getNumLights();

        // MIS light sampling
        float lightPdf;
        Vec3 lightDir = light->sampleDirection(intersectionPoint,
            rng.uniformFloat(), rng.uniformFloat(), &lightPdf);
        Vec3 wi = basis.worldToLocal(lightDir);

        float cosThetaI = abs(cosTheta(wi));
        if (cosThetaI > 0.0f && lightPdf > 0.0f) {
            Ray lightRay(intersectionPoint + sign(cosTheta(wi)) * hit.normal * 0.001f, lightDir);
            RayHit lightHit = scene.intersect(lightRay);
            if (lightHit.shape == light && lightHit.shape != hit.shape) {
                float bsdfPdf = material->pdf(wi, wo);
                if (bsdfPdf > 0.0f) {
                    float misWeight = powerHeuristic(1, lightPdf, 1, bsdfPdf);
                    Vec3 bsdf = material->evaluate(wi, wo);
                    color += lambda * light->material->getEmittance() * bsdf * cosThetaI * misWeight / (lightPdf * lightProb);
                    assert(isFinite(color) && color.r >= 0.0f && color.g >= 0.0f && color.b >= 0.0f);
                }
            }
        }

        // Sample BSDF for direct and indirect illumination
        float bsdfPdf;
        wi = material->sampleDirection(wo, rng.uniformFloat(), rng.uniformFloat(), &bsdfPdf);
        cosThetaI = abs(cosTheta(wi));
        if (cosThetaI <= 0.0f || bsdfPdf <= 0.0f) {
            break;
        }
        Vec3 bsdf = material->evaluate(wi, wo);

        // MIS BSDF sampling
        Ray lightRay(intersectionPoint + sign(cosTheta(wi)) * hit.normal * 0.001f, basis.localToWorld(wi));
        RayHit lightHit = scene.intersect(lightRay);
        if (lightHit.shape == light && lightHit.shape != hit.shape) {
            lightPdf = light->pdf(intersectionPoint, lightRay.direction);
            if (lightPdf > 0.0f) {
                float misWeight = powerHeuristic(1, bsdfPdf, 1, lightPdf);
                color += lambda * light->material->getEmittance() * bsdf * cosThetaI * misWeight / (bsdfPdf * lightProb);
                assert(isFinite(color) && color.r >= 0.0f && color.g >= 0.0f && color.b >= 0.0f);
            }
        }

        // Update path throughput
        lambda *= bsdf * cosThetaI / bsdfPdf;
        assert(isFinite(lambda) && lambda.r >= 0.0f && lambda.g >= 0.0f && lambda.b >= 0.0f);

        // Russian roulette
        float rrProb = min(0.95f, max(lambda.r, max(lambda.g, lambda.b)));
        assert(rrProb > 0.0f && std::isfinite(rrProb));
        if (depth >= minRRDepth_) {
            if (rng.uniformFloat() > rrProb) {
                break;
            }
            lambda /= rrProb;
        }

        // Reuse ray and hit from MIS
        ray = lightRay;
        hit = lightHit;
    }

    return color;
}

} // namespace pt

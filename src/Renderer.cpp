#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "Film.h"
#include "RandomSeries.h"
#include "Material.h"
#include "BRDF.h"
#include "Shape.h"

namespace pt {

void Renderer::render(const Scene& scene, const Camera& camera, Film& film) {
    RandomSeries rng;
    for (int y = 0; y < (int)film.getHeight(); y++) {
        for (uint32_t x = 0; x < film.getWidth(); x++) {
            for (uint32_t s = 0; s < samplesPerPixel_; s++) {
                float u = (x + rng.uniformFloat()) / static_cast<float>(film.getWidth() - 1);
                float v = (y + rng.uniformFloat()) / static_cast<float>(film.getHeight() - 1);
                Ray ray = camera.generateRay(u, v);
                film.addSample(x, y, radiance(scene, ray, Vec3(1.0f)));
            }
        }
    }
}

Vec3 Renderer::radiance(const Scene& scene, const Ray& ray, Vec3 lambda, uint32_t depth) {
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

    Vec3 intersectionPoint = ray.at(hit.t) + hit.normal * 0.001f;

    Vec3 color(0.0f);
    for (auto light : scene.getLights()) {
        float lightPdf;
        Vec3 lightDir = light->sampleDirection(intersectionPoint,
            rng_.uniformFloat(), rng_.uniformFloat(), lightPdf);

        float cosTheta = dot(lightDir, hit.normal);
        if (cosTheta <= 0.0f) {
            continue;
        }

        RayHit lightHit = scene.intersect(Ray(intersectionPoint, lightDir));
        if (lightHit.shape != light || lightHit.shape == hit.shape) {
            continue;
        }

        // TODO: Sample the BRDF as well and use multiple importance sampling

        Vec3 brdf = brdfLambert(hit.shape->material->baseColor);
        color += lambda * light->material->emittance * brdf * cosTheta / lightPdf;
    }

    Vec3 b1, b2;
    makeOrthonormalBasis(hit.normal, b1, b2);
    Vec3 wi = sampleCosineHemisphere(rng_.uniformFloat(), rng_.uniformFloat());
    wi = transformVectorToBasis(wi, b1, b2, hit.normal);

    float cosTheta = dot(hit.normal, wi);
    assert(cosTheta >= 0.0f);
    float pdf = pdfCosineHemisphere(hit.normal, wi);
    Vec3 brdf = brdfLambert(hit.shape->material->baseColor);
    lambda *= brdf * cosTheta / pdf;
    
    float p = max(lambda.r, max(lambda.g, lambda.b));
    if (depth < minRRDepth_ || rng_.uniformFloat() <= p) {
        if (depth >= minRRDepth_) {
            lambda /= p;
        }

        Ray newRay(intersectionPoint, wi);
        color += lambda * radiance(scene, newRay, lambda, depth + 1);
    }

    // TODO: Radiance clamping after N bounces?
    return color;
}

} // namespace pt

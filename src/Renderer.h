#pragma once

#include "Vector3.h"
#include "Ray.h"
#include "RandomSeries.h"
#include "Film.h"

#include <vector>
#include <thread>
#include <mutex>
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
    void setMinRRDepth(uint32_t depth) { minRRDepth_ = depth; }
    void setTileSize(uint32_t width, uint32_t height) { tileWidth_ = width; tileHeight_ = height; }
    void setBackgroundColor(const Vec3& color) { backgroundColor_ = color; }

private:
    void workerThreadMain(uint32_t id, const Scene& scene,
        const Camera& camera, Film& film, std::vector<Film::Tile>& filmTiles);
    Vec3 radiance(const Scene& scene, RandomSeries& rng, Ray ray) const;

    uint32_t maxDepth_ = 10;
    uint32_t samplesPerPixel_ = 16;
    uint32_t minRRDepth_ = 3;
    uint32_t tileWidth_ = 64;
    uint32_t tileHeight_ = 64;
    Vec3 backgroundColor_ = Vec3(0.0f);
    std::vector<std::thread> workerThreads_;
    std::mutex mutex_;
};

} // namespace pt

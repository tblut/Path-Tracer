#pragma once

#include "Vector3.h"
#include "Ray.h"
#include "RandomSeries.h"
#include "Film.h"

#include <vector>
#include <thread>
#include <atomic>
#include <cstdint>

namespace pt {

class Scene;
class Camera;
class Film;
class Sampler;
class ProgressBar;

class Renderer {
public:
    void render(const Scene& scene, const Camera& camera, Film& film, Sampler& sampler);

    void setMaxDepth(uint32_t depth) { maxDepth_ = depth; }
    void setMinRRDepth(uint32_t depth) { minRRDepth_ = depth; }
    void setTileSize(uint32_t width, uint32_t height) { tileWidth_ = width; tileHeight_ = height; }
    void setBackgroundColor(const Vec3& color) { backgroundColor_ = color; }

private:
    void workerThreadMain(uint32_t id, const Scene& scene,
        const Camera& camera, Film& film, Sampler& sampler,
        std::vector<Film::Tile>& filmTiles, std::atomic<size_t>& nextTileIndex,
        ProgressBar& progressBar);
    Vec3 radiance(const Scene& scene, Sampler& sampler, Ray ray) const;

    uint32_t maxDepth_ = 10;
    uint32_t minRRDepth_ = 3;
    uint32_t tileWidth_ = 64;
    uint32_t tileHeight_ = 64;
    Vec3 backgroundColor_ = Vec3(0.0f);
    std::vector<std::thread> workerThreads_;
};

} // namespace pt

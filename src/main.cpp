#include "Film.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Material.h"
#include "Sphere.h"
#include "Triangle.h"
#include "SceneFileParser.h"
#include "RandomSampler.h"
#include "CMJSampler.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <filesystem>

int loadAndRenderScene(const std::filesystem::path& scenePath,
        const std::filesystem::path& outputPath, uint32_t samplesPerPixelOverride) {
    pt::SceneFileParser sceneParser(scenePath);
    if (!sceneParser.isValid()) {
        return 1;
    }

    pt::Film film = sceneParser.parseFilm();
    float filmAspectRatio = film.getWidth() / static_cast<float>(film.getHeight());
    pt::Camera camera = sceneParser.parseCamera(filmAspectRatio);
    auto sampler = sceneParser.parseSampler(samplesPerPixelOverride);
    pt::Renderer renderer = sceneParser.parseRenderer();

    std::vector<pt::Sphere> spheres;
    std::vector<pt::Triangle> triangles;
    std::vector<pt::Material> materials;
    sceneParser.parseScene(spheres, triangles, materials);

    pt::Scene scene;
    for (const auto& shape : spheres) {
        scene.add(shape);
    }
    for (const auto& shape : triangles) {
        scene.add(shape);
    }
    scene.compile();

    auto start = std::chrono::high_resolution_clock::now();
    renderer.render(scene, camera, film, *sampler);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Render completed in " << (end - start).count() * 1.0e-9 << " seconds\n";
    film.saveToFile(outputPath.generic_u8string());

    return 0;
}

int main(int argc, char** argv) {
    std::string scenePath = "../scenes/cornell.json"; // Default scene for debugging
    std::string outputPath = "output.png";
    uint32_t samplesPerPixel = 0;

    if (argc > 1) {
        scenePath = std::string(argv[1]);
    
        for (int i = 2; i < argc; i++) {
            std::string arg(argv[i]);
            if (arg == "-o" || arg == "--output") {
                outputPath = std::string(argv[++i]);
            }
            else if (arg == "-s" || arg == "--spp") {
                samplesPerPixel = std::atoi(argv[++i]);
            }
            else {
                std::cout << "[ERROR]: Unknown argument \"" << arg << "\"\n";
                return 1;
            }
        }
    }

    return loadAndRenderScene(scenePath, outputPath, samplesPerPixel);
}

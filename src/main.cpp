#include "Film.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Matrix4x4.h"
#include "Material.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Ray.h"
#include "ColorUtils.h"

#include <json.hpp>

#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

using json = nlohmann::json;

pt::Film parseFilm(const json& root) {
    uint32_t filmWidth, filmHeight;
    if (auto it = root.find("film"); it != root.end()) {
        for (const auto& item : it->items()) {
            const auto& v = item.value();
            if (item.key() == "size") {
                if (v.is_array()) {
                    v[0].get_to(filmWidth);
                    v[1].get_to(filmHeight);
                }
                else {
                    v.get_to(filmWidth);
                    filmHeight = filmWidth;
                }
            }
        }
    }

    return pt::Film(filmWidth, filmHeight);
}

pt::Camera parseCamera(const json& root, float filmAspect) {
    float fovY = 60.0f;
    float aspect = filmAspect;
    float aperture = 0.0f;
    float focalDistance = 0.0f;
    pt::Mat4 viewMatrix(1.0f);

    if (auto it = root.find("camera"); it != root.end()) {
        for (const auto& item : it->items()) {
            const auto& v = item.value();
            if (item.key() == "fovY") {
                v.get_to(fovY);
            }
            else if (item.key() == "aspect") {
                if (float temp = v.get<float>(); temp > 0.0f) {
                    aspect = temp;
                }
            }
            else if (item.key() == "aperture") {
                v.get_to(aperture);
            }
            else if (item.key() == "focalDistance") {
                v.get_to(focalDistance);
            }
            else if (item.key() == "lookAt") {
                pt::Vec3 pos(v[0].get<float>(), v[1].get<float>(), v[2].get<float>());
                pt::Vec3 dir(v[3].get<float>(), v[4].get<float>(), v[5].get<float>());
                pt::Vec3 up(v[6].get<float>(), v[7].get<float>(), v[8].get<float>());
                viewMatrix = pt::lookAt(pos, dir, up);
            }
        }
    }

    return pt::Camera(pt::radians(fovY), aspect, aperture, focalDistance, viewMatrix);
}

pt::Renderer parseRenderer(const json& root) {
    pt::Renderer renderer;
    if (auto it = root.find("renderer"); it != root.end()) {
        for (const auto& item : it->items()) {
            const auto& v = item.value();
            if (item.key() == "maxDepth") {
                renderer.setMaxDepth(v.get<uint32_t>());
            }
            else if (item.key() == "samplesPerPixel") {
                renderer.setSamplesPerPixel(v.get<uint32_t>());
            }
            else if (item.key() == "minRRDepth") {
                renderer.setMinRRDepth(v.get<uint32_t>());
            }
            else if (item.key() == "backgroundColor") {
                pt::Vec3 color;
                if (v.is_array()) {
                    color = pt::Vec3(v[0].get<float>(), v[1].get<float>(), v[2].get<float>());
                }
                else {
                    color = pt::Vec3(v.get<float>());
                }
                renderer.setBackgroundColor(color);
            }
            else if (item.key() == "tileSize") {
                uint32_t tileWidth, tileHeight;
                if (v.is_array()) {
                    v[0].get_to(tileWidth);
                    v[1].get_to(tileHeight);
                }
                else {
                    v.get_to(tileWidth);
                    tileHeight = tileWidth;
                }
                renderer.setTileSize(tileWidth, tileHeight);
            }
        }
    }

    return renderer;
}

void parseMaterials(const json& root, std::vector<pt::Material>& materials,
        std::unordered_map<std::string, size_t>& materialMap) {
    auto iterMaterials = root.find("materials");
    if (iterMaterials == root.end()) {
        return;
    }

    for (const auto& materialDesc : iterMaterials.value()) {
        pt::Vec3 baseColor(1.0f);
        float roughness = 1.0f;
        float metalness = 0.0f;
        float transmission = 0.0f;
        float ior = 1.5f;
        pt::Vec3 emittance(0.0f);

        for (const auto& item : materialDesc.items()) {
            const auto& v = item.value();
            if (item.key() == "name") {
                materialMap[v.get<std::string>()] = materials.size();
            }
            else if (item.key() == "baseColor") {
                if (v.is_array()) {
                    baseColor = pt::Vec3(v[0].get<float>(), v[1].get<float>(), v[2].get<float>());
                }
                else {
                    baseColor = pt::Vec3(v.get<float>());
                }
            }
            else if (item.key() == "roughness") {
                v.get_to(roughness);
            }
            else if (item.key() == "metalness") {
                v.get_to(metalness);
            }
            else if (item.key() == "ior") {
                v.get_to(ior);
            }
            else if (item.key() == "transmission") {
                v.get_to(transmission);
            }
            else if (item.key() == "emittance") {
                if (v.is_array()) {
                    emittance = pt::Vec3(v[0].get<float>(), v[1].get<float>(), v[2].get<float>());
                }
                else {
                    emittance = pt::Vec3(v.get<float>());
                }
            }
        }

        materials.emplace_back(baseColor, roughness, metalness, transmission, ior, emittance);
    }
}

void parseShapes(const json& root, const std::vector<pt::Material>& materials,
        const std::unordered_map<std::string, size_t>& materialMap,
        std::vector<pt::Sphere>& spheres, std::vector<pt::Triangle>& triangles) {
    if (auto iterShapes = root.find("shapes"); iterShapes != root.end()) {
        for (const auto& shapeDesc : iterShapes.value()) {
            const pt::Material& material = materials[materialMap.at(shapeDesc["material"])];
            if (shapeDesc["type"] == "sphere") {
                auto& centerObj = shapeDesc["center"];
                pt::Vec3 center(centerObj[0].get<float>(), centerObj[1].get<float>(), centerObj[2].get<float>());
                float radius = shapeDesc["radius"].get<float>();
                spheres.emplace_back(center, radius, material);
            }
            else if (shapeDesc["type"] == "triangle") {
                auto& verticesObj = shapeDesc["vertices"];
                pt::Vec3 vertices[3];
                vertices[0] = pt::Vec3(verticesObj[0].get<float>(), verticesObj[1].get<float>(), verticesObj[2].get<float>());
                vertices[1] = pt::Vec3(verticesObj[3].get<float>(), verticesObj[4].get<float>(), verticesObj[5].get<float>());
                vertices[2] = pt::Vec3(verticesObj[6].get<float>(), verticesObj[7].get<float>(), verticesObj[8].get<float>());
                triangles.emplace_back(vertices[0], vertices[1], vertices[2], material);
            }
            else if (shapeDesc["type"] == "triangleMesh") {
                auto& verticesObj = shapeDesc["vertices"];
                auto& indicesObj = shapeDesc["indices"];
                for (size_t i = 0; i < indicesObj.size(); i += 3) {
                    uint32_t i0 = indicesObj[i + 0].get<uint32_t>();
                    uint32_t i1 = indicesObj[i + 1].get<uint32_t>();
                    uint32_t i2 = indicesObj[i + 2].get<uint32_t>();
                    pt::Vec3 p0 = pt::Vec3(
                        verticesObj[i0 * 3 + 0].get<float>(),
                        verticesObj[i0 * 3 + 1].get<float>(),
                        verticesObj[i0 * 3 + 2].get<float>());
                    pt::Vec3 p1 = pt::Vec3(
                        verticesObj[i1 * 3 + 0].get<float>(),
                        verticesObj[i1 * 3 + 1].get<float>(),
                        verticesObj[i1 * 3 + 2].get<float>());
                    pt::Vec3 p2 = pt::Vec3(
                        verticesObj[i2 * 3 + 0].get<float>(),
                        verticesObj[i2 * 3 + 1].get<float>(),
                        verticesObj[i2 * 3 + 2].get<float>());
                    triangles.emplace_back(p0, p1, p2, material);
                }
            }
        }
    }
}

int loadAndRenderScene(const std::string& scenePath, const std::string& outputPath, uint32_t samplesPerPixel) {
    std::ifstream fileStream(scenePath);
    if (!fileStream.is_open()) {
        std::cout << "[ERROR]: The scene file \"" << scenePath << "\" doesn't exist.\n";
        return 1;
    }

    json root = json::parse(fileStream, nullptr, false, true);
    if (root.is_discarded()) {
        std::cout << "[ERROR]: The scene file is not a valid JSON file.\n";
        return 1;
    }

    pt::Film film = parseFilm(root);
    pt::Camera camera = parseCamera(root,
        film.getWidth() / static_cast<float>(film.getHeight()));
    pt::Renderer renderer = parseRenderer(root);

    // Override if needed
    if (samplesPerPixel > 0) {
        renderer.setSamplesPerPixel(samplesPerPixel);
    }

    std::vector<pt::Sphere> spheres;
    std::vector<pt::Triangle> triangles;
    std::vector<pt::Material> materials;
    std::unordered_map<std::string, size_t> materialMap;

    if (auto iterScene = root.find("scene"); iterScene != root.end()) {
        parseMaterials(*iterScene, materials, materialMap);
        parseShapes(*iterScene, materials, materialMap, spheres, triangles);
    }

    pt::Scene scene;
    for (const auto& shape : spheres) {
        scene.add(shape);
    }
    for (const auto& shape : triangles) {
        scene.add(shape);
    }
    scene.compile();

    auto start = std::chrono::high_resolution_clock::now();
    renderer.render(scene, camera, film);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Render completed in " << (end - start).count() * 1.0e-9 << " seconds\n";
    film.saveToFile(outputPath);

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

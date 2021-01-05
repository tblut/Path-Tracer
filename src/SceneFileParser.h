#pragma once

#include "Film.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Material.h"

#include <json.hpp>

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace pt {

class SceneFileParser {
public:
    SceneFileParser(const std::filesystem::path& sceneFilePath);

    pt::Film parseFilm();
    pt::Camera parseCamera(float filmAspectRatio);
    pt::Renderer parseRenderer();
    void parseScene(std::vector<pt::Sphere>& spheres,
        std::vector<pt::Triangle>& triangles,
        std::vector<pt::Material>& materials);

    bool isValid() const { return !root_.is_discarded(); }

private:
    void parseMaterials(const nlohmann::json& node, std::vector<pt::Material>& materials);
    void parseShapes(const nlohmann::json& node, const std::vector<pt::Material>& materials,
        std::vector<pt::Sphere>& spheres, std::vector<pt::Triangle>& triangles);
    void parseTriangleMesh(const nlohmann::json& node, const Material& material,
        const Mat4& transform, const Mat4& normalTransform, std::vector<pt::Triangle>& triangles);

    std::filesystem::path sceneFilePath_;
    nlohmann::json root_;
    std::unordered_map<std::string, size_t> materialMap_;
};

} // namespace pt

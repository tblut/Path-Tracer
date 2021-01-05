#include "SceneFileParser.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <fstream>
#include <iostream>

namespace pt {

using json = nlohmann::json;

SceneFileParser::SceneFileParser(const std::filesystem::path& sceneFilePath)
    : sceneFilePath_(sceneFilePath)
{
    std::ifstream fileStream(sceneFilePath);
    if (!fileStream.is_open()) {
        std::cout << "[ERROR]: The scene file \"" << sceneFilePath << "\" doesn't exist.\n";
        return;
    }

    root_ = json::parse(fileStream, nullptr, false, true);
    if (root_.is_discarded()) {
        std::cout << "[ERROR]: The scene file is not a valid JSON file.\n";
        return;
    }
}

Film SceneFileParser::parseFilm() {
    uint32_t filmWidth, filmHeight;
    if (auto it = root_.find("film"); it != root_.end()) {
        for (const auto& item : it->items()) {
            const json& v = item.value();
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

    return Film(filmWidth, filmHeight);
}

Camera SceneFileParser::parseCamera(float filmAspectRatio) {
    float fovY = 60.0f;
    float aspect = filmAspectRatio;
    float aperture = 0.0f;
    float focalDistance = 0.0f;
    Mat4 viewMatrix(1.0f);

    if (auto it = root_.find("camera"); it != root_.end()) {
        for (const auto& item : it->items()) {
            const json& v = item.value();
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
                Vec3 pos(v[0].get<float>(), v[1].get<float>(), v[2].get<float>());
                Vec3 dir(v[3].get<float>(), v[4].get<float>(), v[5].get<float>());
                Vec3 up(v[6].get<float>(), v[7].get<float>(), v[8].get<float>());
                viewMatrix = lookAt(pos, dir, up);
            }
        }
    }

    return Camera(radians(fovY), aspect, aperture, focalDistance, viewMatrix);
}

Renderer SceneFileParser::parseRenderer() {
    Renderer renderer;
    if (auto it = root_.find("renderer"); it != root_.end()) {
        for (const auto& item : it->items()) {
            const json& v = item.value();
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
                Vec3 color;
                if (v.is_array()) {
                    color = Vec3(v[0].get<float>(), v[1].get<float>(), v[2].get<float>());
                }
                else {
                    color = Vec3(v.get<float>());
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

void SceneFileParser::parseScene(std::vector<Sphere>& spheres,
        std::vector<Triangle>& triangles, std::vector<Material>& materials) {
    if (auto iterScene = root_.find("scene"); iterScene != root_.end()) {
        parseMaterials(*iterScene, materials);
        parseShapes(*iterScene, materials, spheres, triangles);
    }
}

void SceneFileParser::parseMaterials(const json& node, std::vector<Material>& materials) {
    auto iterMaterials = node.find("materials");
    if (iterMaterials == node.end()) {
        return;
    }

    for (const auto& materialDesc : iterMaterials.value()) {
        Vec3 baseColor(1.0f);
        float roughness = 1.0f;
        float metalness = 0.0f;
        float transmission = 0.0f;
        float ior = 1.5f;
        Vec3 emittance(0.0f);

        for (const auto& item : materialDesc.items()) {
            const json& v = item.value();
            if (item.key() == "name") {
                materialMap_[v.get<std::string>()] = materials.size();
            }
            else if (item.key() == "baseColor") {
                if (v.is_array()) {
                    baseColor = Vec3(v[0].get<float>(), v[1].get<float>(), v[2].get<float>());
                }
                else {
                    baseColor = Vec3(v.get<float>());
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
                    emittance = Vec3(v[0].get<float>(), v[1].get<float>(), v[2].get<float>());
                }
                else {
                    emittance = Vec3(v.get<float>());
                }
            }
        }

        materials.emplace_back(baseColor, roughness, metalness, transmission, ior, emittance);
    }
}

void SceneFileParser::parseShapes(const json& node, const std::vector<Material>& materials,
        std::vector<Sphere>& spheres, std::vector<Triangle>& triangles) {
    if (auto iterShapes = node.find("shapes"); iterShapes != node.end()) {
        for (const auto& shapeDesc : iterShapes.value()) {
            const Material& material = materials[materialMap_.at(shapeDesc["material"])];

            Mat4 transform(1.0f);
            if (auto it = shapeDesc.find("transform"); it != shapeDesc.end()) {
                const json& v = it.value();
                transform = Mat4(
                    v[0].get<float>(), v[1].get<float>(), v[2].get<float>(), v[3].get<float>(),
                    v[4].get<float>(), v[5].get<float>(), v[6].get<float>(), v[7].get<float>(),
                    v[8].get<float>(), v[9].get<float>(), v[10].get<float>(), v[11].get<float>(),
                    v[12].get<float>(), v[13].get<float>(), v[14].get<float>(), v[15].get<float>());
            }
            else {
                if (auto it = shapeDesc.find("translation"); it != shapeDesc.end()) {
                    const json& v = it.value();
                    transform *= translation(Vec3(v[0].get<float>(), v[1].get<float>(), v[2].get<float>()));
                }
                if (auto it = shapeDesc.find("rotation"); it != shapeDesc.end()) {
                    const json& v = it.value();
                    transform *= rotationZ(radians(v[2].get<float>()));
                    transform *= rotationY(radians(v[1].get<float>()));
                    transform *= rotationX(radians(v[0].get<float>()));
                }
                if (auto it = shapeDesc.find("scale"); it != shapeDesc.end()) {
                    const json& v = it.value();
                    if (v.is_array()) {
                        transform *= scaling(Vec3(v[0].get<float>(), v[1].get<float>(), v[2].get<float>()));
                    }
                    else {
                        transform *= scaling(Vec3(v.get<float>()));
                    }
                }
            }

            if (shapeDesc["type"] == "sphere") {
                const json& centerObj = shapeDesc["center"];
                Vec3 center(centerObj[0].get<float>(), centerObj[1].get<float>(), centerObj[2].get<float>());
                center = transformPoint3x4(transform, center);

                float radius = shapeDesc["radius"].get<float>();
                radius = maxComponent(transformVector3x4(transform, Vec3(radius)));

                spheres.emplace_back(center, radius, material);
            }
            else if (shapeDesc["type"] == "triangle") {
                const json& verticesObj = shapeDesc["vertices"];
                Vec3 vertices[3];
                vertices[0] = Vec3(verticesObj[0].get<float>(), verticesObj[1].get<float>(), verticesObj[2].get<float>());
                vertices[1] = Vec3(verticesObj[3].get<float>(), verticesObj[4].get<float>(), verticesObj[5].get<float>());
                vertices[2] = Vec3(verticesObj[6].get<float>(), verticesObj[7].get<float>(), verticesObj[8].get<float>());

                vertices[0] = transformPoint3x4(transform, vertices[0]);
                vertices[1] = transformPoint3x4(transform, vertices[1]);
                vertices[2] = transformPoint3x4(transform, vertices[2]);

                triangles.emplace_back(vertices[0], vertices[1], vertices[2], material);
            }
            else if (shapeDesc["type"] == "triangleMesh") {
                parseTriangleMesh(shapeDesc, material, transform, triangles);
            }
        }
    }
}

void SceneFileParser::parseTriangleMesh(const nlohmann::json& node, const Material& material,
        const Mat4& transform, std::vector<Triangle>& triangles) {
    bool hasVertices = node.contains("vertices");
    bool hasIndices = node.contains("indices");

    if (hasVertices && hasIndices) {
        const json& vertices = node["vertices"];
        const json& indices = node["indices"];
        for (size_t i = 0; i < indices.size(); i += 3) {
            uint32_t i0 = indices[i + 0].get<uint32_t>();
            uint32_t i1 = indices[i + 1].get<uint32_t>();
            uint32_t i2 = indices[i + 2].get<uint32_t>();
            Vec3 p0 = Vec3(
                vertices[i0 * 3 + 0].get<float>(),
                vertices[i0 * 3 + 1].get<float>(),
                vertices[i0 * 3 + 2].get<float>());
            Vec3 p1 = Vec3(
                vertices[i1 * 3 + 0].get<float>(),
                vertices[i1 * 3 + 1].get<float>(),
                vertices[i1 * 3 + 2].get<float>());
            Vec3 p2 = Vec3(
                vertices[i2 * 3 + 0].get<float>(),
                vertices[i2 * 3 + 1].get<float>(),
                vertices[i2 * 3 + 2].get<float>());

            p0 = transformPoint3x4(transform, p0);
            p1 = transformPoint3x4(transform, p1);
            p2 = transformPoint3x4(transform, p2);

            triangles.emplace_back(p0, p1, p2, material);
        }
    }
    else if (hasVertices && !hasIndices) {
        const json& vertices = node["vertices"];
        for (size_t i = 0; i < vertices.size(); i += 9) {
            Vec3 p0 = Vec3(
                vertices[i + 0].get<float>(),
                vertices[i + 1].get<float>(),
                vertices[i + 2].get<float>());
            Vec3 p1 = Vec3(
                vertices[i + 3].get<float>(),
                vertices[i + 4].get<float>(),
                vertices[i + 5].get<float>());
            Vec3 p2 = Vec3(
                vertices[i + 6].get<float>(),
                vertices[i + 7].get<float>(),
                vertices[i + 8].get<float>());

            p0 = transformPoint3x4(transform, p0);
            p1 = transformPoint3x4(transform, p1);
            p2 = transformPoint3x4(transform, p2);

            triangles.emplace_back(p0, p1, p2, material);
        }
    }
    else if (!hasVertices && !hasIndices) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::string errors;

        std::filesystem::path objPath = sceneFilePath_.parent_path() / node["modelPath"].get<std::string>();
        bool success = tinyobj::LoadObj(&attrib, &shapes, nullptr, &errors, objPath.generic_u8string().c_str());
        if (!errors.empty()) {
            std::cout << "[WARNING]: " << errors << "\n";
        }
        if (!success) {
            std::cout << "[WARNING]: Skipping invalid obj triangle mesh\n";
            return;
        }

        for (size_t shapeIndex = 0; shapeIndex < shapes.size(); shapeIndex++) {
            size_t indexOffset = 0;
            size_t numFaces = shapes[shapeIndex].mesh.num_face_vertices.size();

            for (size_t faceIndex = 0; faceIndex < numFaces; faceIndex++) {
                size_t numVertices = shapes[shapeIndex].mesh.num_face_vertices[faceIndex];

                for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex += 3) {
                    tinyobj::index_t i0 = shapes[shapeIndex].mesh.indices[indexOffset + vertexIndex + 0];
                    tinyobj::index_t i1 = shapes[shapeIndex].mesh.indices[indexOffset + vertexIndex + 1];
                    tinyobj::index_t i2 = shapes[shapeIndex].mesh.indices[indexOffset + vertexIndex + 2];

                    Vec3 v0 = Vec3(
                        attrib.vertices[i0.vertex_index * 3 + 0],
                        attrib.vertices[i0.vertex_index * 3 + 1],
                        attrib.vertices[i0.vertex_index * 3 + 2]);
                    Vec3 v1 = Vec3(
                        attrib.vertices[i1.vertex_index * 3 + 0],
                        attrib.vertices[i1.vertex_index * 3 + 1],
                        attrib.vertices[i1.vertex_index * 3 + 2]);
                    Vec3 v2 = Vec3(
                        attrib.vertices[i2.vertex_index * 3 + 0],
                        attrib.vertices[i2.vertex_index * 3 + 1],
                        attrib.vertices[i2.vertex_index * 3 + 2]);

                    v0 = transformPoint3x4(transform, v0);
                    v1 = transformPoint3x4(transform, v1);
                    v2 = transformPoint3x4(transform, v2);

                    if (!attrib.normals.empty()) {
                        Vec3 n0 = Vec3(
                            attrib.normals[i0.normal_index * 3 + 0],
                            attrib.normals[i0.normal_index * 3 + 1],
                            attrib.normals[i0.normal_index * 3 + 2]);
                        Vec3 n1 = Vec3(
                            attrib.normals[i1.normal_index * 3 + 0],
                            attrib.normals[i1.normal_index * 3 + 1],
                            attrib.normals[i1.normal_index * 3 + 2]);
                        Vec3 n2 = Vec3(
                            attrib.normals[i2.normal_index * 3 + 0],
                            attrib.normals[i2.normal_index * 3 + 1],
                            attrib.normals[i2.normal_index * 3 + 2]);

                        // TODO: Transform with inverse transpose instead
                        //n0 = normalize(transformVector3x4(transform, n0));
                        //n1 = normalize(transformVector3x4(transform, n1));
                        //n2 = normalize(transformVector3x4(transform, n2));

                        triangles.emplace_back(v0, v1, v2, n0, n1, n2, material);
                    }
                    else {
                        triangles.emplace_back(v0, v1, v2, material);
                    }
                }

                indexOffset += numVertices;
            }
        }
    }
    else {
        std::cout << "[WARNING]: Skipping invalid triangle mesh definition\n";
    }
}

} // namespace pt

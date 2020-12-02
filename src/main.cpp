#include "Film.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Matrix4x4.h"
#include "Material.h"
#include "Sphere.h"
#include "Ray.h"
#include "ColorUtils.h"

#include <chrono>
#include <iostream>

int main(int argc, char** argv) {
    pt::Film film(640, 480);
    pt::Camera camera(
        pt::radians(60.0f),
        film.getWidth() / static_cast<float>(film.getHeight()),
        0.250f, 4.5f,
        pt::lookAt(pt::Vec3(0.0f, 0.0f, 5.0f), pt::Vec3(0.0f, 0.0f, 0.0f), pt::Vec3(0, 1, 0))
    );

    pt::Material redMat{ pt::srgbToLinear(pt::Vec3(0.8f, 0, 0)), 1.0f, 0.0f, pt::Vec3(0) };
    pt::Material greenMat{ pt::srgbToLinear(pt::Vec3(0, 0.8f, 0)), 1.0f, 0.0f, pt::Vec3(0) };
    pt::Material whiteMat{ pt::srgbToLinear(pt::Vec3(0.8f, 0.8f, 0.8f)), 1.0f, 0.0f, pt::Vec3(0) };
    pt::Material plasticMat{ pt::srgbToLinear(pt::Vec3(0.8f, 0.8f, 0.8f)), 0.0f, 0.0f, pt::Vec3(0) };
    pt::Material metallMat{ pt::srgbToLinear(pt::Vec3(0.8f, 0.8f, 0.8f)), 0.0f, 1.0f, pt::Vec3(0) };
    pt::Material lightMat{ pt::Vec3(0), 1.0f, 0.0f, pt::Vec3(16) };
    pt::Material lightMat2{ pt::Vec3(0), 1.0f, 0.0f, pt::Vec3(100) };

    pt::Sphere left(pt::Vec3(-1002, 0, 0), 1000, redMat);
    pt::Sphere right(pt::Vec3(1002, 0, 0), 1000, greenMat);
    pt::Sphere back(pt::Vec3(0, 0, -1002), 1000, whiteMat);
    pt::Sphere top(pt::Vec3(0, 1002, 0), 1000, whiteMat);
    pt::Sphere bottom(pt::Vec3(0, -1002, 0), 1000, whiteMat);
    pt::Sphere light(pt::Vec3(0.0f, 2.7f, 0.0f), 1.0f, lightMat);
    pt::Sphere light2(pt::Vec3(-1, 1.f, -1.5f), 0.1f, lightMat2);
    pt::Sphere sphere1(pt::Vec3(-0.75f, -1.2f, -0.75f), 0.75f, plasticMat);
    pt::Sphere sphere2(pt::Vec3(0.8f, -1.25f, 0.5f), 0.75f, metallMat);

    pt::Scene scene;
    scene.add(left);
    scene.add(right);
    scene.add(back);
    scene.add(top);
    scene.add(bottom);
    scene.add(light);
    scene.add(light2);
    scene.add(sphere1);
    scene.add(sphere2);
    scene.compile();

    pt::Renderer renderer;
    renderer.setSamplesPerPixel(128);
    renderer.setMaxDepth(10);

    auto start = std::chrono::high_resolution_clock::now();
    renderer.render(scene, camera, film);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Render completed in " << (end - start).count() * 1.0e-9 << " sec\n";
    film.saveToFile("test.png");
    return 0;
}

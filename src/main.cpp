#include "Film.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Matrix4x4.h"
#include "Material.h"

#include <iostream>
#include <vector>
#include <cassert>

int main(int argc, char** argv) {
    pt::Film film(640, 480);
    pt::Camera camera(
        pt::pi<float> * 0.5f,
        film.getWidth() / static_cast<float>(film.getHeight()),
        pt::lookAt(pt::Vec3(5, 2, 4), pt::Vec3(0, 0, 0), pt::Vec3(0, 1, 0))
    );

    pt::Material mat1{ pt::Vec3(1, 0, 0) };
    pt::Material mat2{ pt::Vec3(1, 0, 1) };
    pt::Sphere sphere1(pt::Vec3(0, 0, 0), 1, mat1);
    pt::Sphere sphere2(pt::Vec3(3, 2, 0), 0.2f, mat2);

    pt::Scene scene;
    scene.add({ sphere1, sphere2 });

    pt::Renderer renderer;
    renderer.render(scene, camera, film, 32);
    film.saveToFile("test.png");

    return 0;
}

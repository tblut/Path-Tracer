#include "Film.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Matrix4x4.h"
#include "Material.h"
#include "Sphere.h"
#include "Plane.h"

#include <iostream>
#include <vector>
#include <cassert>

#include "BRDF.h"
#include "RandomSeries.h"

int main(int argc, char** argv) {
    pt::Film film(640, 480);
    pt::Camera camera(
        pt::pi<float> * 0.5f,
        film.getWidth() / static_cast<float>(film.getHeight()),
        pt::lookAt(pt::Vec3(0, 0, 4), pt::Vec3(0, 0, 0), pt::Vec3(0, 1, 0))
    );

    pt::Material mat1{ pt::Vec3(1, 0, 0), pt::Vec3(0) };
    pt::Material mat2{ pt::Vec3(1, 0, 1), pt::Vec3(1) };
    pt::Material mat3{ pt::Vec3(0, 1, 0), pt::Vec3(0) };
    pt::Sphere sphere1(pt::Vec3(0, 0, -5), 1, mat1);
    pt::Sphere sphere2(pt::Vec3(0, 4, -5), 2, mat2);
    pt::Plane plane(pt::Vec3(0, -1, 0), pt::Vec3(0, 1, 0), mat3);

    pt::Scene scene;
    scene.add({ sphere1, sphere2 });
    scene.add(plane);

    pt::Renderer renderer;
    renderer.setSamplesPerPixel(256);
    renderer.render(scene, camera, film);
    film.saveToFile("test.png");

    return 0;
}

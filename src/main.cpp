#include "Film.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Matrix4x4.h"

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
    pt::Scene scene;
    scene.addSphere(pt::Vec3(0, 0, 0), 1);
    scene.addSphere(pt::Vec3(3, 2, 0), 0.1f);

    pt::Renderer renderer;
    renderer.render(scene, camera, film, 32);
    film.saveToFile("test.png");

    return 0;
}

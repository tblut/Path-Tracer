#include "Film.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Matrix4x4.h"
#include "Material.h"
#include "Sphere.h"

#include "RandomSeries.h"

int main(int argc, char** argv) {
    pt::Film film(640, 480);
    pt::Camera camera(
        pt::radians(60.0f),
        film.getWidth() / static_cast<float>(film.getHeight()),
        pt::lookAt(pt::Vec3(0, 0, 6), pt::Vec3(0, 0, 0), pt::Vec3(0, 1, 0))
    );

    pt::Material redMat{ pt::Vec3(1, 0, 0), pt::Vec3(0) };
    pt::Material greenMat{ pt::Vec3(0, 1, 0), pt::Vec3(0) };
    pt::Material whiteMat{ pt::Vec3(1, 1, 1), pt::Vec3(0) };
    pt::Material lightMat{ pt::Vec3(0), pt::Vec3(4) };

    pt::Sphere left(pt::Vec3(-1002, 0, 0), 1000, redMat);
    pt::Sphere right(pt::Vec3(1002, 0, 0), 1000, greenMat);
    pt::Sphere back(pt::Vec3(0, 0, -1002), 1000, whiteMat);
    pt::Sphere top(pt::Vec3(0, 1002, 0), 1000, whiteMat);
    pt::Sphere bottom(pt::Vec3(0, -1002, 0), 1000, whiteMat);
    pt::Sphere light(pt::Vec3(0, 2.8f, 0), 1, lightMat);
    pt::Sphere sphere1(pt::Vec3(-0.75f, -1.25f, -0.5f), 0.75f, whiteMat);
    pt::Sphere sphere2(pt::Vec3(0.8f, -1.25f, 0.75f), 0.75f, whiteMat);

    pt::Scene scene;
    scene.add({ left, right, back, top, bottom, light, sphere1, sphere2 });

    pt::Renderer renderer;
    renderer.setSamplesPerPixel(512);
    renderer.setMaxDepth(10);
    renderer.render(scene, camera, film);
    film.saveToFile("test.png");

    return 0;
}

#include "Film.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Matrix4x4.h"
#include "Material.h"
#include "Sphere.h"


#include "RandomSeries.h"
#include "BRDF.h"

int main(int argc, char** argv) {
    pt::Film film(640, 480);
    pt::Camera camera(
        pt::radians(60.0f),
        film.getWidth() / static_cast<float>(film.getHeight()),
        pt::lookAt(pt::Vec3(-0.75f, 0.4f, 2.5), pt::Vec3(-0.75f, -1.2f, 0.5f), pt::Vec3(0, 1, 0))
    );

    pt::Material redMat{ pt::Vec3(1, 0, 0), 1.0f, 0.0f, pt::Vec3(0) };
    pt::Material greenMat{ pt::Vec3(0, 1, 0), 1.0f, 0.0f, pt::Vec3(0) };
    pt::Material whiteMat{ pt::Vec3(1, 1, 1), 1.0f, 0.0f, pt::Vec3(0) };
    pt::Material plasticMat{ pt::Vec3(0.8f, 0.0f, 0.0f), 0.7f, 0.0f, pt::Vec3(0) };
    pt::Material metallMat{ pt::Vec3(1, 1, 1), 0.3f, 1.0f, pt::Vec3(0) };
    pt::Material lightMat{ pt::Vec3(0), 1.0f, 0.0f, pt::Vec3(400) };
    pt::Material lightMat2{ pt::Vec3(0), 1.0f, 0.0f, pt::Vec3(100) };

    pt::Sphere left(pt::Vec3(-1002, 0, 0), 1000, redMat);
    pt::Sphere right(pt::Vec3(1002, 0, 0), 1000, greenMat);
    pt::Sphere back(pt::Vec3(0, 0, -1002), 1000, whiteMat);
    pt::Sphere top(pt::Vec3(0, 1002, 0), 1000, whiteMat);
    pt::Sphere bottom(pt::Vec3(0, -1002, 0), 1000, whiteMat);
    pt::Sphere light(pt::Vec3(-0.75f, 2.8f, 0.5f), 0.2f, lightMat);
    //pt::Sphere light2(pt::Vec3(-1, 1.f, -1.5f), 0.1f, lightMat2);
    pt::Sphere sphere1(pt::Vec3(-0.75f, -1.2f, 0.5f), 0.75f, plasticMat);
    //pt::Sphere sphere2(pt::Vec3(0.8f, -1.25f, -0.75f), 0.75f, metallMat);

    pt::Scene scene;
    //scene.add(left);
    //scene.add(right);
    //scene.add(back);
    //scene.add(top);
    scene.add(bottom);
    scene.add(light);
    //scene.add(light2);
    scene.add(sphere1);
    //scene.add(sphere2);
    scene.compile();

    pt::Renderer renderer;
    renderer.setSamplesPerPixel(512);
    renderer.setMaxDepth(10);
    //renderer.setMinRRDepth(10000);
    renderer.render(scene, camera, film);
    film.saveToFile("test.png");

    return 0;
}

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

#include <chrono>
#include <iostream>

int main(int argc, char** argv) {
    pt::Film film(640, 480);
    pt::Camera camera(
        pt::radians(60.0f),
        film.getWidth() / static_cast<float>(film.getHeight()),
        0.0f, 4.5f,
        pt::lookAt(pt::Vec3(0.0f, 0.0f, 4.8f), pt::Vec3(0.0f, 0.0f, 0.0f), pt::Vec3(0, 1, 0))
    );

    pt::Material redMat{     pt::Vec3(0.8f, 0.0f, 0.0f), 1.0f, 0.0f, 0.0f, 1.5f, pt::Vec3(0) };
    pt::Material greenMat{   pt::Vec3(0.0f, 0.8f, 0.0f), 1.0f, 0.0f, 0.0f, 1.5f, pt::Vec3(0) };
    pt::Material whiteMat{   pt::Vec3(0.8f, 0.8f, 0.8f), 1.0f, 0.0f, 0.0f, 1.5f, pt::Vec3(0) };
    pt::Material plasticMat{ pt::Vec3(1.0f, 0.9f, 0.8f), 0.2f, 1.0f, 0.0f, 1.5f, pt::Vec3(0) };
    pt::Material metallMat{  pt::Vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 1.0f, 1.6f, pt::Vec3(0) };
    pt::Material lightMat{ pt::Vec3(0), 1.0f, 0.0f, 0.0f, 1.5f, pt::Vec3(12) };
    pt::Material lightMat2{ pt::Vec3(0), 1.0f, 0.0f, 0.0f, 1.5f, pt::Vec3(6) };

    pt::Sphere sphere1(pt::Vec3(-0.75f, -1.2f, -0.75f), 0.75f, plasticMat);
    pt::Sphere sphere2(pt::Vec3(0.8f, -1.25f, 0.5f), 0.75f, metallMat);

    pt::Vec3 fbl(-2.0f, -2.0f, 2.2f);
    pt::Vec3 fbr(2.0f, -2.0f, 2.2f);
    pt::Vec3 ftl(-2.0f, 2.0f, 2.2f);
    pt::Vec3 ftr(2.0f, 2.0f, 2.2f);
    pt::Vec3 bbl(-2.0f, -2.0f, -2.0f);
    pt::Vec3 bbr(2.0f, -2.0f, -2.0f);
    pt::Vec3 btl(-2.0f, 2.0f, -2.0f);
    pt::Vec3 btr(2.0f, 2.0f, -2.0f);

    pt::Triangle left1(bbl, ftl, fbl, redMat);
    pt::Triangle left2(bbl, btl, ftl, redMat);
    pt::Triangle right1(bbr, fbr, btr, greenMat);
    pt::Triangle right2(btr, fbr, ftr, greenMat);
    pt::Triangle back1(bbl, bbr, btr, whiteMat);
    pt::Triangle back2(bbl, btr, btl, whiteMat);
    pt::Triangle top1(ftl, btr, ftr, whiteMat);
    pt::Triangle top2(ftl, btl, btr, whiteMat);
    pt::Triangle bottom1(fbl, fbr, bbr, whiteMat);
    pt::Triangle bottom2(fbl, bbr, bbl, whiteMat);

    float size = 0.75f;
    pt::Triangle light1(
        pt::Vec3(-size, 1.95f, -size),
        pt::Vec3(-size, 1.95f,  size),
        pt::Vec3( size, 1.95f,  size),
        lightMat
    );
    pt::Triangle light2(
        pt::Vec3( size, 1.95f,  size),
        pt::Vec3( size, 1.95f, -size),
        pt::Vec3(-size, 1.95f, -size),
        lightMat
    );

    pt::Scene scene;
    scene.add(left1);
    scene.add(left2);
    scene.add(right1);
    scene.add(right2);
    scene.add(back1);
    scene.add(back2);
    scene.add(top1);
    scene.add(top2);
    scene.add(bottom1);
    scene.add(bottom2);
    scene.add(light1);
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
    std::cout << "Render completed in " << (end - start).count() * 1.0e-9 << " seconds\n";
    film.saveToFile("output.png");
    return 0;
}

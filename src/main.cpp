#include "Film.h"
#include "Camera.h"
#include "Renderer.h"
#include "RandomSeries.h"
#include "Matrix4x4.h"

#include <iostream>
#include <vector>
#include <cassert>

int main(int argc, char** argv) {
    pt::Film film(256, 128);
    pt::Camera camera(
        0.785398163f*2,//pt::radians(pt::pi<float> * 0.5f),
        film.getWidth() / static_cast<float>(film.getHeight()),
        pt::lookAt(pt::Vec3(0, 0, 0), pt::Vec3(0, 0, -1), pt::Vec3(0, 1, 0)));

    pt::Renderer renderer;
    renderer.render(camera, film);
    film.saveToFile("test.png");

    auto m = pt::lookAt<float>(pt::Vec3(1, 0, 0), pt::Vec3(2, 0, 0), pt::Vec3(0, 1, 0));
    auto v = pt::Vec3(0, 0, -1);
    auto u = pt::transformPoint3x4(m, v);


    return 0;
}

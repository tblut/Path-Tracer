#include "Film.h"
#include "Renderer.h"

#include <iostream>
#include <vector>
#include <cassert>

int main(int argc, char** argv) {
    pt::Film film(256, 128);
    pt::Renderer renderer;
    renderer.render(film);
    film.saveToFile("test.png");

    return 0;
}

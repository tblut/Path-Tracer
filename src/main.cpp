#include "Film.h"

#include <iostream>
#include <vector>
#include <cassert>

int main(int argc, char** argv) {
    pt::Film film(256, 128);

    for (uint32_t y = 0; y < film.getHeight(); y++) {
        for (uint32_t x = 0; x < film.getWidth(); x++) {
            film.addSample(x, y, { 1, 0, 0 });
            film.addSample(x, y, { 0, 0, 1 });
        }
    }

    film.saveToFile("test.png");

    return 0;
}

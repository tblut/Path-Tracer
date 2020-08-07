#include "Renderer.h"
#include "Film.h"

namespace pt {

void Renderer::render(Film& film) {
    for (uint32_t y = 0; y < film.getHeight(); y++) {
        for (uint32_t x = 0; x < film.getWidth(); x++) {
            film.addSample(x, y, { 1, 0, 0 });
            film.addSample(x, y, { 0, 0, 1 });
        }
    }
}

} // namespace pt

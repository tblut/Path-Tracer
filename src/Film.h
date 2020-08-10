#pragma once

#include "Vector3.h"

#include <vector>
#include <string>
#include <cstdint>
#include <cassert>

namespace pt {

class Film {
public:
    Film(uint32_t width, uint32_t height);

    void addSample(uint32_t x, uint32_t y, const Vec3& color);
    std::vector<uint8_t> getImageBuffer(bool tonemap = true) const;
    bool saveToFile(std::string path) const;

    uint32_t getWidth() const { return width_; }
    uint32_t getHeight() const { return height_; }

private:
    struct Pixel {
        uint32_t numSamples;
        Vec3 accumColor;
    };

    uint32_t width_;
    uint32_t height_;
    std::vector<Pixel> pixels_;
};

} // namespace pt

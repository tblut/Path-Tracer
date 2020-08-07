#include "Film.h"
#include "stb_image_write.h"

#include <algorithm>
#include <cctype>
#include <cassert>

namespace pt {

Film::Film(uint32_t width, uint32_t height)
    : width_(width)
    , height_(height)
    , pixels_(width * height, { 0u, Vec3(0.0f) })
{
}

void Film::addSample(uint32_t x, uint32_t y, const Vec3& color) {
    assert(x < width_);
    assert(y < height_);
    auto& pixel = pixels_[x + y * width_];
    pixel.accumColor += color;
    pixel.numSamples++;
}

std::vector<uint8_t> Film::getImageBuffer() const {
    std::vector<uint8_t> imageBuffer;
    imageBuffer.reserve(width_ * height_ * 3);

    for (const auto& pixel : pixels_) {
        Vec3 color = pixel.accumColor / static_cast<float>(pixel.numSamples);
        color = linearToSRGB(color);
        imageBuffer.push_back(static_cast<uint8_t>(color.r * 255.0 + 0.5));
        imageBuffer.push_back(static_cast<uint8_t>(color.g * 255.0 + 0.5));
        imageBuffer.push_back(static_cast<uint8_t>(color.b * 255.0 + 0.5));
    }

    return imageBuffer;
}

bool Film::saveToFile(std::string path) const {
    auto image = getImageBuffer();

    auto ext = path.substr(path.rfind('.'));
    std::transform(ext.begin(), ext.end(), ext.begin(),
        [](unsigned char c) { return std::tolower(c); });

    int bytesWritten = 0;
    if (ext == ".png") {
        bytesWritten = stbi_write_png(path.data(), static_cast<int>(width_),
            static_cast<int>(height_), 3, image.data(), 3);
    }
    else if (ext == ".jpg" || ext == ".jpeg") {
        bytesWritten = stbi_write_jpg(path.data(), static_cast<int>(width_),
            static_cast<int>(height_), 3, image.data(), 90);
    }
    else if (ext == ".bmp") {
        bytesWritten = stbi_write_bmp(path.data(), static_cast<int>(width_),
            static_cast<int>(height_), 3, image.data());
    }
    else if (ext == ".tga") {
        bytesWritten = stbi_write_tga(path.data(), static_cast<int>(width_),
            static_cast<int>(height_), 3, image.data());
    }

    return bytesWritten > 0;
}

} // namespace pt

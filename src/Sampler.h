#pragma once

#include "Vector2.h"

#include <memory>
#include <cstdint>

namespace pt {

class Sampler {
public:
    Sampler(uint32_t samplesPerPixel)
        : samplesPerPixel_(samplesPerPixel)
    {
    }

    virtual std::unique_ptr<Sampler> clone(uint64_t seed) = 0;
    virtual float get1D() = 0;
    virtual Vec2 get2D() = 0;
    virtual void startNextSample() = 0;
    virtual void startPixel(uint32_t pixelIndex) = 0;

    uint32_t getSamplesPerPixel() const {
        return samplesPerPixel_;
    }

protected:
    uint32_t samplesPerPixel_;
};

} // namespace pt

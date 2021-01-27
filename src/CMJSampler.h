#pragma once

#include "Sampler.h"
#include "RandomSeries.h"

#include <vector>

namespace pt {

// See: Correlated Multi-Jittered Sampling (2013), Andrew Kensler
class CMJSampler : public Sampler {
public:
    CMJSampler(uint32_t samplesPerPixel);

    virtual std::unique_ptr<Sampler> clone(uint64_t seed) override;
    virtual float get1D() override;
    virtual Vec2 get2D() override;
    virtual void startNextSample() override;
    virtual void startPixel(uint32_t pixelIndex) override;

private:
    uint32_t numSamplesX_;
    uint32_t numSamplesY_;
    uint32_t sampleIndex_ = 0;
    uint32_t hashedPixelIndex_ = 0;
    uint32_t nextPattern_ = 0;
};

} // namespace pt

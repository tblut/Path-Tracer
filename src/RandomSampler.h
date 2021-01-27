#pragma once

#include "Sampler.h"
#include "RandomSeries.h"

namespace pt {

class RandomSampler : public Sampler {
public:
    RandomSampler(uint32_t samplesPerPixel);

    virtual std::unique_ptr<Sampler> clone(uint64_t seed) override;
    virtual float get1D() override;
    virtual Vec2 get2D() override;
    virtual void startNextSample() override;
    virtual void startPixel(uint32_t pixelIndex) override;

private:
    RandomSeries rng_;
};

} // namespace pt

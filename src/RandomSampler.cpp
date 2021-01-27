#include "RandomSampler.h"

namespace pt {

RandomSampler::RandomSampler(uint32_t samplesPerPixel)
    : Sampler(samplesPerPixel)
{
}

std::unique_ptr<Sampler> RandomSampler::clone(uint64_t seed) {
    auto clonedSampler = std::make_unique<RandomSampler>(getSamplesPerPixel());
    constexpr uint64_t oddBitsMask = 0x5555555555555555ul;
    constexpr uint64_t evenBitsMask = 0xaaaaaaaaaaaaaaaaul;
    clonedSampler->rng_.seed(
        RandomSeries::defaultState ^ (seed & oddBitsMask),
        RandomSeries::defaultInc ^ (seed & evenBitsMask));
    return clonedSampler;
}

float RandomSampler::get1D() {
    return rng_.uniformFloat();
}

Vec2 RandomSampler::get2D() {
    return Vec2(rng_.uniformFloat(), rng_.uniformFloat());
}

void RandomSampler::startNextSample() {
    // Nothing to do
}

void RandomSampler::startPixel(uint32_t pixelIndex) {
    // Nothing to do
}

} // namespace pt

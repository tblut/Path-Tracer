#include "CMJSampler.h"
#include "HashUtils.h"
#include "MathUtils.h"

#include <cassert>

namespace {

using namespace pt;

uint32_t cmjPermute(uint32_t i, uint32_t l, uint32_t p) {
    assert(l > 0);
    uint32_t w = (1 << (32 - countLeadingZeros(l - 1))) - 1;

    do {
        i ^= p;             i *= 0xe170893d;
        i ^= p >> 16;
        i ^= (i & w) >> 4;
        i ^= p >> 8;        i *= 0x0929eb3f;
        i ^= p >> 23;
        i ^= (i & w) >> 1;  i *= 1 | p >> 27;
        i *= 0x6935fa69;
        i ^= (i & w) >> 11; i *= 0x74dcb303;
        i ^= (i & w) >> 2;  i *= 0x9e501cc3;
        i ^= (i & w) >> 2;  i *= 0xc860a3df;
        i &= w;
        i ^= i >> 5;
    } while (i >= l);

    return (i + p) % l;
}

float cmjRandFloat(uint32_t i, uint32_t p) {
    i ^= p;
    i ^= i >> 17;
    i ^= i >> 10;       i *= 0xb36534e5;
    i ^= i >> 12;
    i ^= i >> 21;       i *= 0x93fc4795;
    i ^= 0xdf6e307f;
    i ^= i >> 17;       i *= 1 | p >> 18;
    return i * (1.0f / 4294967808.0f);
}

} // namespace


namespace pt {

CMJSampler::CMJSampler(uint32_t samplesPerPixel)
    : Sampler(samplesPerPixel)
{
    numSamplesX_ = static_cast<uint32_t>(std::sqrt(samplesPerPixel));
    numSamplesY_ = (samplesPerPixel - 1) / numSamplesX_ + 1;
    samplesPerPixel_ = numSamplesX_ * numSamplesY_;
}

std::unique_ptr<Sampler> CMJSampler::clone(uint64_t seed) {
    return std::make_unique<CMJSampler>(samplesPerPixel_);
}

float CMJSampler::get1D() {
    uint32_t n = samplesPerPixel_;
    uint32_t p = hashedPixelIndex_ + nextPattern_;
    nextPattern_++;

    uint32_t s = sampleIndex_;
    uint32_t sx = cmjPermute(s, n, p * 0x68bc21eb);
    float jx = cmjRandFloat(s, p * 0x967a889b);
    float x = (sx + jx) / n;

    return min(x, oneMinusEpsilon<float>);
}

Vec2 CMJSampler::get2D() {
    uint32_t m = numSamplesX_;
    uint32_t n = numSamplesY_;
    uint32_t p = hashedPixelIndex_ + nextPattern_;
    nextPattern_++;

    uint32_t s = cmjPermute(sampleIndex_, samplesPerPixel_, p * 0x51633e2d);
    uint32_t sx = cmjPermute(s % m, m, p * 0x68bc21eb);
    uint32_t sy = cmjPermute(s / m, n, p * 0x02e5be93);
    float jx = cmjRandFloat(s, p * 0x967a889b);
    float jy = cmjRandFloat(s, p * 0x368cc8b7);
    float x = (sx + (sy + jx) / n) / m;
    float y = (s + jy) / samplesPerPixel_;

    x = min(x, oneMinusEpsilon<float>);
    y = min(y, oneMinusEpsilon<float>);
    return Vec2(x, y);
}

void CMJSampler::startNextSample() {
    sampleIndex_++;
    nextPattern_ = 0;
}

void CMJSampler::startPixel(uint32_t pixelIndex) {
    hashedPixelIndex_ = hash(pixelIndex);
    sampleIndex_ = 0;
    nextPattern_ = 0;
}

} // namespace pt

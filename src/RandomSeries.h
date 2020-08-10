#pragma once

#include "MathUtils.h"

#include <cstdint>

namespace pt {

// See: https://www.pcg-random.org/
class RandomSeries {
public:
    constexpr void seed(uint64_t initstate, uint64_t initseq) {
        state_ = 0U;
        inc_ = (initseq << 1u) | 1u;
        uniformUint32();
        state_ += initstate;
        uniformUint32();
    }

    constexpr uint32_t uniformUint32() {
        uint64_t oldState = state_;
        state_ = oldState * 6364136223846793005ull + (inc_ | 1);
        uint32_t xorShifted = static_cast<uint32_t>(((oldState >> 18u) ^ oldState) >> 27u);
        uint32_t rot = static_cast<uint32_t>(oldState >> 59u);
        return (xorShifted >> rot) | (xorShifted << ((~rot + 1u) & 31));
    }

    constexpr float uniformFloat() {
        return static_cast<float>(uniformUint32()) / 0xffffffff;
    }

    constexpr double uniformDouble() {
        return static_cast<double>(uniformUint32()) / 0xffffffff;
    }

    // Seed number for initstate=42 and initseq=54
    static constexpr uint64_t defaultState = 1753877967969059832ull;
    static constexpr uint64_t defaultInc = 109ull;

private:
    uint64_t state_ = defaultState;
    uint64_t inc_ = defaultInc;
};

} // namespace pt

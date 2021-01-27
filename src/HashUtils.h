#pragma once

#include <cstdint>

namespace pt {

// Murmurhash3 32-bit finalizer
inline uint32_t hash(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

// Murmurhash3 64-bit finalizer
inline uint64_t hash(uint64_t k) {
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdllu;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53llu;
    k ^= k >> 33;
    return k;
}

} // namespace pt

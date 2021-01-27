#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "TestHelpers.h"
#include "RandomSeries.h"
#include "CMJSampler.h"

#include <array>
#include <vector>
#include <algorithm>

TEST_CASE("RandomSeries Uniform Sampling") {
    constexpr int numSamples = 1000000;
    constexpr int numBuckets = 100;
    constexpr double significance = 0.05;

    std::array<int, numBuckets> buckets;
    buckets.fill(0);

    pt::RandomSeries rng;
    for (int i = 0; i < numSamples; i++) {
        float value = rng.uniformFloat();
        REQUIRE(value >= 0.0f);
        REQUIRE(value < 1.0f);

        int bucketIndex = static_cast<int>(value * numBuckets);
        buckets[bucketIndex]++;
    }

    // Chi squared test
    int dof = numBuckets - 1;
    double criticalValue = 0.0;
    for (int frequency : buckets) {
        double expected = (1.0 / numBuckets) * numSamples;
        double diff = frequency - expected;
        criticalValue += diff * diff / expected;
    }
    double p = 1.0 - pt::chi2cdf(dof, criticalValue);

    REQUIRE(p > significance);
}

TEST_CASE("CMJ Sampling") {
    constexpr uint32_t m = 12;
    constexpr uint32_t n = 13;
    constexpr uint32_t N = m * n;

    std::vector<uint32_t> strata1D(N, 0);
    std::vector<uint32_t> strata2D(N, 0);
    std::vector<uint32_t> substrataRows2D(N, 0);
    std::vector<uint32_t> substrataCols2D(N, 0);

    auto strataIndex = [](const pt::Vec2& sample, uint32_t m, uint32_t n) {
        uint32_t strataX = static_cast<uint32_t>(sample.x * m);
        uint32_t strataY = static_cast<uint32_t>(sample.y * n);
        return strataX + strataY * m;
    };

    pt::CMJSampler sampler(N);
    for (uint32_t pixelIndex = 0; pixelIndex < 10; pixelIndex++) {
        sampler.startPixel(pixelIndex);

        std::fill(strata1D.begin(), strata1D.end(), 0);
        std::fill(strata2D.begin(), strata2D.end(), 0);
        std::fill(substrataRows2D.begin(), substrataRows2D.end(), 0);
        std::fill(substrataCols2D.begin(), substrataCols2D.end(), 0);

        for (uint32_t sampleIndex = 0; sampleIndex < N; sampleIndex++) {
            float sample1D = sampler.get1D();
            pt::Vec2 sample2D = sampler.get2D();

            uint32_t index = static_cast<uint32_t>(sample1D * N);
            strata1D[index]++;

            uint32_t strataX = static_cast<uint32_t>(sample2D.x * m);
            uint32_t strataY = static_cast<uint32_t>(sample2D.y * n);
            index = strataX + strataY * m;
            strata2D[index]++;

            uint32_t substrataRow = static_cast<uint32_t>(sample2D.y * N);
            substrataRows2D[substrataRow]++;

            uint32_t substrataCol = static_cast<uint32_t>(sample2D.x * N);
            substrataCols2D[substrataCol]++;

            sampler.startNextSample();
        }

        for (uint32_t count : strata1D) CHECK(count == 1);
        for (uint32_t count : strata2D) CHECK(count == 1);
        for (uint32_t count : substrataRows2D) CHECK(count == 1);
        for (uint32_t count : substrataCols2D) CHECK(count == 1);
    }
}

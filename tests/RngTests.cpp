#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "TestHelpers.h"
#include "RandomSeries.h"

#include <array>

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

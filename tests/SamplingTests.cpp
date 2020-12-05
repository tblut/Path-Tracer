#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "TestHelpers.h"
#include "RandomSeries.h"

#include <array>

TEST_CASE("Chi Squared CDF Sanity Check") {
    constexpr double eps = 1e-5;

    CHECK(pt::chi2cdf(1, 2.70554) == pt::Approx(0.9).epsilon(eps));
    CHECK(pt::chi2cdf(1, 3.84146) == pt::Approx(0.95).epsilon(eps));
    CHECK(pt::chi2cdf(1, 5.02389) == pt::Approx(0.975).epsilon(eps));
    CHECK(pt::chi2cdf(1, 6.6349) == pt::Approx(0.99).epsilon(eps));

    CHECK(pt::chi2cdf(2, 4.60517) == pt::Approx(0.9).epsilon(eps));
    CHECK(pt::chi2cdf(2, 5.99146) == pt::Approx(0.95).epsilon(eps));
    CHECK(pt::chi2cdf(2, 7.37776) == pt::Approx(0.975).epsilon(eps));
    CHECK(pt::chi2cdf(2, 9.21034) == pt::Approx(0.99).epsilon(eps));

    CHECK(pt::chi2cdf(3, 6.25139) == pt::Approx(0.9).epsilon(eps));
    CHECK(pt::chi2cdf(3, 7.81473) == pt::Approx(0.95).epsilon(eps));
    CHECK(pt::chi2cdf(3, 9.3484) == pt::Approx(0.975).epsilon(eps));
    CHECK(pt::chi2cdf(3, 11.3449) == pt::Approx(0.99).epsilon(eps));

    CHECK(pt::chi2cdf(12, 18.5493) == pt::Approx(0.9).epsilon(eps));
    CHECK(pt::chi2cdf(12, 21.0261) == pt::Approx(0.95).epsilon(eps));
    CHECK(pt::chi2cdf(12, 23.3367) == pt::Approx(0.975).epsilon(eps));
    CHECK(pt::chi2cdf(12, 26.217) == pt::Approx(0.99).epsilon(eps));

    CHECK(pt::chi2cdf(201, 227.085) == pt::Approx(0.9).epsilon(eps));
    CHECK(pt::chi2cdf(201, 235.077) == pt::Approx(0.95).epsilon(eps));
    CHECK(pt::chi2cdf(201, 242.156) == pt::Approx(0.975).epsilon(eps));
    CHECK(pt::chi2cdf(201, 250.561) == pt::Approx(0.99).epsilon(eps));
}


TEST_CASE("RandomSeries Uniform Sampling") {
    constexpr size_t numSamples = 1000000;
    constexpr int numBuckets = 100;
    std::array<size_t, numBuckets> buckets;
    buckets.fill(0);

    pt::RandomSeries rng;
    for (size_t i = 0; i < numSamples; i++) {
        float value = rng.uniformFloat();
        REQUIRE(value >= 0.0f);
        REQUIRE(value < 1.0f);

        size_t bucketIndex = static_cast<size_t>(value * numBuckets);
        buckets[bucketIndex]++;
    }

    // Chi squared test
    int dof = numBuckets - 1;
    double criticalValue = 0.0;
    for (size_t frequency : buckets) {
        double expected = (1.0 / numBuckets) * numSamples;
        double diff = frequency - expected;
        criticalValue += diff * diff / expected;
    }

    double p = 1.0 - pt::chi2cdf(dof, criticalValue);
    REQUIRE(p > 0.001);
}

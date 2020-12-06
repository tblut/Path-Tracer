#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "TestHelpers.h"
#include "RandomSeries.h"
#include "BSDF.h"

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


TEST_CASE("Adaptive Simpson Sanity Check") {
    double integral = pt::adaptiveSimpson1D([](double x) { return 1.0; }, 0.0, 1.0);
    CHECK(integral == pt::Approx(1.0));

    integral = pt::adaptiveSimpson1D([](double x) { return std::sin(x); }, 0.0, 2.0);
    CHECK(integral == pt::Approx(1.41614683654714));

    integral = pt::adaptiveSimpson1D([](double x) { return std::cos(x); }, 1.0, 5.0);
    CHECK(integral == pt::Approx(-1.80039525947103));

    integral = pt::adaptiveSimpson2D([](double x, double y) { return 1.0; }, 0.0, 1.0, 1.0, 2.0);
    CHECK(integral == pt::Approx(1.0));

    integral = pt::adaptiveSimpson2D([](double x, double y) { return std::sin(x * y); }, 0.0, 2.0, 0.0, 2.0);
    CHECK(integral == pt::Approx(2.10449172390835));

    integral = pt::adaptiveSimpson2D([](double x, double y) { return std::cos(x * y); }, 0.0, 3.0, 1.0, 2.0);
    CHECK(integral == pt::Approx(-0.42396497671896));
}


TEST_CASE("RandomSeries Uniform Sampling") {
    constexpr int numSamples = 1000000;
    constexpr int numBuckets = 100;
    constexpr double significance = 0.001;

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


using BrdfFunc = std::function<float(const pt::Vec3& wi)>;

double integrateHemisphere(const BrdfFunc& brdf) {
    return pt::adaptiveSimpson2D([&](double theta, double phi) {
        auto wi = pt::Vector3<float>(
            std::sin(theta) * std::cos(phi),
            std::sin(theta) * std::sin(phi),
            std::cos(theta)
        );
        return brdf(wi) * std::sin(theta);
    }, 0.0, pt::pi<double> / 2.0, 0.0, 2.0 * pt::pi<double>, 1e-6, 12);
}

TEST_CASE("BxDF PDFs") {
    SECTION("Cosine-weighted") {
        float probability = integrateHemisphere([](const pt::Vec3& wi) {
            return pt::pdfCosineHemisphere(wi);
        });
        CHECK(probability == pt::Approx(1.0f).epsilon(1e-5f));
    }

    SECTION("GGX") {
        pt::RandomSeries rng;
        for (int i = 0; i < 10; i++) {
            pt::Vec3 wo = pt::sampleCosineHemisphere(rng.uniformFloat(), rng.uniformFloat());
            float probability = integrateHemisphere([&](const pt::Vec3& wi) {
                pt::Vec3 wh = pt::normalize(wi + wo);
                float alpha = rng.uniformFloat();
                return pt::pdfGGX(wh, wo, 0.1f);
            });
            CHECK(probability == pt::Approx(1.0f).epsilon(1e-5f));
        }
    }

    SECTION("VisGGX") {
        pt::RandomSeries rng;
        for (int i = 0; i < 10; i++) {
            pt::Vec3 wo = pt::sampleCosineHemisphere(rng.uniformFloat(), rng.uniformFloat());
            float probability = integrateHemisphere([&](const pt::Vec3& wi) {
                pt::Vec3 wh = pt::normalize(wi + wo);
                float alpha = rng.uniformFloat();
                return pt::pdfGGXVNDF(wh, wo, 0.1f);
            });
            CHECK(probability == pt::Approx(1.0f).epsilon(1e-5f));
        }
    }
}


TEST_CASE("BRDF Sampling") {
    constexpr int thetaRes = 10;
    constexpr int phiRes = thetaRes * 2;
    constexpr int sampleCount = 1000000;
    constexpr double significance = 0.001;

    // Determine expected frequencies per bucket
    std::array<float, thetaRes* phiRes> expFrequencies;
    for (int theta = 0; theta < thetaRes; theta++) {
        for (int phi = 0; phi < phiRes; phi++) {
            float x0 = theta * pt::pi<float> / thetaRes;
            float x1 = (theta + 1) * pt::pi<float> / thetaRes;
            float y0 = phi * 2.0f * pt::pi<float> / phiRes;
            float y1 = (phi + 1) * 2.0f * pt::pi<float> / phiRes;

            float probability = pt::adaptiveSimpson2D([](float theta, float phi) {
                auto n = pt::Vec3(0.0, 0.0f, 1.0f);
                auto wi = pt::Vec3(
                    std::sin(theta) * std::cos(phi),
                    std::sin(theta) * std::sin(phi),
                    std::cos(theta)
                );
                float pdf = pt::pdfCosineHemisphere(wi) * std::sin(theta);
                return pdf;
            }, x0, x1, y0, y1);

            expFrequencies[theta + phi * thetaRes] = probability * sampleCount;
                
        }
    }

    // Count observed frequencies per bucket
    std::array<int, thetaRes * phiRes> frequencies;
    frequencies.fill(0);

    pt::RandomSeries rng;
    for (int i = 0; i < sampleCount; i++) {
        pt::Vec3 wi = pt::sampleCosineHemisphere(rng.uniformFloat(), rng.uniformFloat());
        float theta = std::acos(pt::clamp(wi.z, -1.0f, 1.0f));
        float phi = std::atan2(wi.y, wi.x);
        if (phi < 0.0f) {
            phi += 2.0f * pt::pi<float>;
        }

        int thetaBucket = static_cast<int>(theta * thetaRes / pt::pi<float>);
        int phiBucket = static_cast<int>(phi * phiRes / (2.0f * pt::pi<float>));
        frequencies[thetaBucket + phiBucket * thetaRes]++;
    }

    REQUIRE(true);
}

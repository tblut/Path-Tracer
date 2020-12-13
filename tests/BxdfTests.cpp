#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "TestHelpers.h"
#include "RandomSeries.h"
#include "BSDF.h"
#include "OrthonormalBasis.h"

#include <array>
#include <algorithm>

constexpr int sampleCount = 1000000;
constexpr int numTestRuns = 5;
constexpr int thetaRes = 10;
constexpr int phiRes = thetaRes * 2;
constexpr float thetaFactor = pt::pi<float> / thetaRes;
constexpr float phiFactor = 2.0f * pt::pi<float> / phiRes;
constexpr float significance = 0.001f;
constexpr float minExpFrequency = 5.0f;

using BxdfPdf = std::function<float(const pt::Vec3& wi, const pt::Vec3& wo)>;
using BxdfSample = std::function<pt::Vec3(const pt::Vec3& wo, float u1, float u2)>;

pt::RandomSeries rng;
void testBxdfGoodnessOfFit(const BxdfPdf& pdf, const BxdfSample& sample) {
    for (int testRun = 0; testRun < numTestRuns; testRun++) {
        pt::Vec3 wo = pt::sampleUniformSphere(rng.uniformFloat(), rng.uniformFloat());

        // Determine expected frequencies per bucket
        std::array<float, thetaRes * phiRes> expFrequencies;
        for (int theta = 0; theta < thetaRes; theta++) {
            for (int phi = 0; phi < phiRes; phi++) {
                float x0 = (theta + 0) * thetaFactor;
                float x1 = (theta + 1) * thetaFactor;
                float y0 = (phi + 0) * phiFactor;
                float y1 = (phi + 1) * phiFactor;

                float probability = pt::adaptiveSimpson2D([&](float theta, float phi) {
                    auto wi = pt::Vec3::fromSpherical(theta, phi);
                    return pdf(wi, wo) * std::sin(theta);
                }, x0, x1, y0, y1, 1e-6f, 8);

                expFrequencies[theta + phi * thetaRes] = probability * sampleCount;
            }
        }

        // Count observed frequencies per bucket
        std::array<int, thetaRes * phiRes> frequencies;
        frequencies.fill(0);

        for (int i = 0; i < sampleCount; i++) {
            pt::Vec3 wi = sample(wo, rng.uniformFloat(), rng.uniformFloat());
            if (pt::lengthSq(wi) < 1e-6f) {
                // An invalid direction was sampled
                continue;
            }

            float theta = std::acos(pt::clamp(wi.z, -1.0f, 1.0f));
            float phi = std::atan2(wi.y, wi.x);
            if (phi < 0.0f) {
                phi += 2.0f * pt::pi<float>;
            }

            int thetaBucket = static_cast<int>(theta / thetaFactor);
            int phiBucket = static_cast<int>(phi / phiFactor);
            thetaBucket = pt::clamp(thetaBucket, 0, thetaRes - 1);
            phiBucket = pt::clamp(phiBucket, 0, phiRes - 1);
            frequencies[thetaBucket + phiBucket * thetaRes]++;
        }

        // Sort buckets by expected frequency
        std::array<int, thetaRes* phiRes> bucketIndices;
        for (int i = 0; i < static_cast<int>(bucketIndices.size()); i++) {
            bucketIndices[i] = i;
        }
        std::sort(bucketIndices.begin(), bucketIndices.end(), [&](int a, int b) {
            return expFrequencies[a] < expFrequencies[b];
        });


        // Cound degrees of freedom and compute critical value
        int dof = 0;
        float criticalValue = 0.0f;
        int numMerged = 0;
        float mergedFrequency = 0.0f;
        float mergedExpFrequency = 0.0f;

        for (int i = 0; i < static_cast<int>(bucketIndices.size()); i++) {
            int index = bucketIndices[i];

            // Merge buckets with less than 5 expected samples
            if (expFrequencies[index] < minExpFrequency) {
                numMerged++;
                mergedFrequency += frequencies[index];
                mergedExpFrequency += expFrequencies[index];
            }
            else if (numMerged > 0 && mergedExpFrequency < minExpFrequency) {
                numMerged++;
                mergedFrequency += frequencies[index];
                mergedExpFrequency += expFrequencies[index];
            }
            else {
                float diff = frequencies[index] - expFrequencies[index];
                criticalValue += diff * diff / expFrequencies[index];
                dof++;
            }
        }

        if (numMerged > 0) {
            float diff = mergedFrequency - mergedExpFrequency;
            criticalValue += diff * diff / mergedExpFrequency;
            dof++;
        }

        dof--; // Degrees of fredom = # buckets - 1
        float p = 1.0f - static_cast<float>(pt::chi2cdf(dof, criticalValue));

        // Dunn-Sidak correction
        float correctedSignificance = 1.0f - std::pow(1.0f - significance, 1.0f / numTestRuns);

        REQUIRE(p > correctedSignificance);
    }
}


TEST_CASE("Othonormal Basis Construction") {
    pt::OrthonormalBasis basis(pt::Vec3(0.0f, 0.0f, 1.0f));
    REQUIRE(pt::dot(basis.b1, basis.b2) == pt::Approx(0.0f));
    REQUIRE(pt::dot(basis.b1, basis.b3) == pt::Approx(0.0f));
    REQUIRE(pt::dot(basis.b2, basis.b3) == pt::Approx(0.0f));
}


TEST_CASE("Othonormal Basis Change of Basis") {
    pt::OrthonormalBasis basis(pt::Vec3(1.0f, 0.0f, 0.0f));
    pt::Vec3 v = basis.worldToLocal(basis.localToWorld(pt::Vec3(1.0f, 2.0f, 3.0f)));
    REQUIRE(v == pt::ApproxVec3(1.0f, 2.0f, 3.0f));
}


TEST_CASE("GGX Normal Distribution (D Term)") {
    for (int i = 0; i < 20; i++) {
        float alpha = 0.01f + (i / 20.0f) * (1.0f - 0.01f);

        float integral = pt::adaptiveSimpson2D([&](float theta, float phi) {
            pt::Vec3 wh = pt::Vec3::fromSpherical(theta, phi);
            return pt::D_GGX(wh, alpha) * pt::cosTheta(wh) * std::sin(theta);
        }, 0.0f, 0.5f * pt::pi<float>, 0.0f, 2.0f * pt::pi<float>, 1e-6f, 8);

        REQUIRE(integral == pt::Approx(1.0f).epsilon(4e-3f));
    }
}


TEST_CASE("Uniform Sphere Sampling") {
    testBxdfGoodnessOfFit(
        [](const pt::Vec3& wi, const pt::Vec3& wo) {
            return pt::pdfUniformSphere(wi, wo);
        },
        [](const pt::Vec3& wo, float u1, float u2) {
            return pt::sampleUniformSphere(u1, u2);
        });
}


TEST_CASE("Cosine-weighted Hemisphere Sampling") {
    testBxdfGoodnessOfFit(
        [](const pt::Vec3& wi, const pt::Vec3& wo) {
            return pt::pdfCosineHemisphere(wi, wo);
        },
        [](const pt::Vec3& wo, float u1, float u2) {
            return pt::sampleCosineHemisphere(u1, u2);
        });
}


TEST_CASE("GGX Reflection Sampling") {
    for (int i = 0; i < 20; i++) {
        float alpha = 0.01f + (i / 20.0f) * (1.0f - 0.01f);
        testBxdfGoodnessOfFit(
            [&](const pt::Vec3& wi, const pt::Vec3& wo) {
                return pt::pdfGGX_reflection(wi, wo, alpha);
            },
            [&](const pt::Vec3& wo, float u1, float u2) {
                pt::Vec3 wh = pt::sampleGGX(alpha, u1, u2);
                if (pt::dot(wo, wh) < 0.0f) return pt::Vec3(0.0f);
                pt::Vec3 wi = pt::reflect(wo, wh);
                if (!pt::sameHemisphere(wi, wo)) return pt::Vec3(0.0f);
                return wi;
            });
    }
}


TEST_CASE("GGX VNDF Reflection Sampling") {
    for (int i = 0; i < 20; i++) {
        float alpha = 0.01f + (i / 20.0f) * (1.0f - 0.01f);
        testBxdfGoodnessOfFit(
            [&](const pt::Vec3& wi, const pt::Vec3& wo) {
                return pt::pdfGGX_VNDF_reflection(wi, wo, alpha);
            },
            [&](const pt::Vec3& wo, float u1, float u2) {
                pt::Vec3 wh = pt::sampleGGX_VNDF(wo, alpha, u1, u2);
                if (pt::dot(wo, wh) < 0.0f) return pt::Vec3(0.0f);
                pt::Vec3 wi = pt::reflect(wo, wh);
                if (!pt::sameHemisphere(wi, wo)) return pt::Vec3(0.0f);
                return wi;
            });
    }
}


TEST_CASE("GGX Transmission Sampling") {
    for (int theta = 0; theta <= 180; theta++) {
        auto wh = pt::Vec3::fromSpherical(pt::radians((float)theta), pt::radians(0.0f));
        pt::Vec3 wt;
        bool b = pt::refract({ 1, 0, 0 }, pt::normalize(pt::Vec3{ 1.f, 1.f, 0.f }), 1.0f, wt);
        assert(b);
    }
    pt::Vec3 wt;
    bool b = pt::refract({ 1, 0, 0 }, pt::normalize(pt::Vec3{ -1.f, 0.f, 0.f }), 1.0f, wt);

    constexpr float etaI = 1.0f;
    constexpr float etaT = 1.0f;

    for (int i = 0; i < 20; i++) {
        float alpha = 0.01f + (i / 20.0f) * (1.0f - 0.01f);
        testBxdfGoodnessOfFit(
            [&](const pt::Vec3& wi, const pt::Vec3& wo) {
                //float eta = pt::cosTheta(wo) > 0.0f ? etaT / etaI : etaI / etaT;
                float pdf =  pt::pdfGGX_transmission(wi, wo, etaI, etaT, alpha);
                assert(pdf >= 0.0f);
                return pdf;
            },
            [&](const pt::Vec3& wo, float u1, float u2) {
                //float eta = pt::cosTheta(wo) > 0.0f ? etaT / etaI : etaI / etaT;
                pt::Vec3 wh = pt::sampleGGX(alpha, u1, u2);
                if (pt::cosTheta(wo) < 0.0f) wh = -wh;
                if (pt::dot(wo, wh) < 0.0f) return pt::Vec3(0.0f);
                assert(pt::sameHemisphere(wh, wo));
                assert(pt::dot(wo, wh) >= 0.0f);

                pt::Vec3 wi;
                bool tir = !pt::refract(wo, wh, 1.0f, wi);
                if (tir || pt::sameHemisphere(wi, wo)) return pt::Vec3(0.0f);
                /*if (tir) {
                    wi = pt::reflect(wo, wh);
                    if (!pt::sameHemisphere(wi, wo)) return pt::Vec3(0.0f);
                    assert(pt::sameHemisphere(wh, wi));
                    assert(pt::sameHemisphere(wi, wo));
                }
                else {
                    assert(!pt::sameHemisphere(wh, wi));
                    if (pt::sameHemisphere(wi, wo)) return pt::Vec3(0.0f);
                    assert(!pt::sameHemisphere(wi, wo));
                }*/
                return wi;
            });
    }
}

/*
TEST_CASE("GGX VNDF Transmission Sampling") {
    for (int i = 0; i < 20; i++) {
        float alpha = 0.01f + (i / 20.0f) * (1.0f - 0.01f);
        testBxdfGoodnessOfFit(
            [&](const pt::Vec3& wi, const pt::Vec3& wo) {
                pt::Vec3 wh = pt::normalize(wi + wo);
                return pt::pdfGGX_VNDF_reflection(wi, wo, alpha);
            },
            [&](const pt::Vec3& wo, float u1, float u2) {
                pt::Vec3 wh = pt::sampleGGX_VNDF(wo, alpha, u1, u2);
                return pt::reflect(wo, wh);
            });
    }
}
*/

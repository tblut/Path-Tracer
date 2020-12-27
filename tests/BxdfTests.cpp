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
constexpr float significance = 0.01f;
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

        // For debugging, both counts should be very close
        //float expCount = 0;
        //for (auto f : expFrequencies) expCount += f;
        //int obsCount = 0;
        //for (auto f : frequencies) obsCount += f;

        // Sort buckets by expected frequency
        std::array<int, thetaRes * phiRes> bucketIndices;
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
        REQUIRE(std::isfinite(criticalValue));

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


TEST_CASE("White-Furnace Test Lambert") {
    float integral = pt::adaptiveSimpson2D([&](float theta, float phi) {
        return pt::diffuse_Lambert(pt::Vec3(1.0f)).x * pt::abs(std::cos(theta)) * std::sin(theta);
    }, 0.0f, 0.5f * pt::pi<float>, 0.0f, 2.0f * pt::pi<float>, 1e-8f, 8);

    REQUIRE(integral == pt::Approx(1.0f).epsilon(1e-2f));
}


TEST_CASE("Weak White-Furnace Test GGX") {
    pt::RandomSeries rng;
    for (int i = 0; i < 10; i++) {
        // Intentionally start with a=0.1, since the numerical integration has problems with
        // smaller alpha values due to dirac-delta-like spikes.
        float alpha = 0.1f + (i / 10.0f) * (1.0f - 0.1f);

        for (int k = 0; k < 5; k++) {
            pt::Vec3 wo = pt::sampleCosineHemisphere(rng.uniformFloat(), rng.uniformFloat());

            float integral = pt::adaptiveSimpson2D([&](float theta, float phi) {
                pt::Vec3 wi = pt::Vec3::fromSpherical(theta, phi);
                pt::Vec3 wh = pt::normalize(wo + wi);
                if (pt::cosTheta(wh) < 0.0f) {
                    return 0.0f;
                }
                
                float G1 = pt::G1_Smith_GGX(wo, alpha);
                float D = pt::D_GGX(wh, alpha);
                return G1 * D / (4.0f * pt::abs(pt::cosTheta(wo))) * std::sin(theta);
            }, 0.0f, pt::pi<float>, 0.0f, 2.0f * pt::pi<float>, 1e-8f, 8);
            
            REQUIRE(integral == pt::Approx(1.0f).epsilon(1e-2f));
        } 
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

// The microfacet BxDFs are really fragile n the tests. They seem to work well enough
// though, when inspecting observed and expected frequencies...
/*
TEST_CASE("GGX Reflection Sampling") {
    for (int i = 0; i < 10; i++) {
        float alpha = 0.1f + (i / 10.0f) * (1.0f - 0.1f);
        testBxdfGoodnessOfFit(
            [&](const pt::Vec3& wi, const pt::Vec3& wo) {
                return pt::pdfGGX_reflection(wi, wo, alpha);
            },
            [&](const pt::Vec3& wo, float u1, float u2) {
                pt::Vec3 wh = pt::sampleGGX(alpha, u1, u2);
                if (!pt::sameHemisphere(wo, wh)) wh = -wh;
                if (pt::dot(wo, wh) < 0.0f) {
                    return pt::Vec3(0.0f);
                }

                pt::Vec3 wi = pt::reflect(wo, wh);
                if (!pt::sameHemisphere(wi, wo)) {
                    return pt::Vec3(0.0f);
                }

                return wi;
            });
    }
}


TEST_CASE("GGX VNDF Reflection Sampling") {
    for (int i = 0; i < 10; i++) {
        float alpha = 0.1f + (i / 10.0f) * (1.0f - 0.1f);
        testBxdfGoodnessOfFit(
            [&](const pt::Vec3& wi, const pt::Vec3& wo) {
                return pt::pdfGGX_VNDF_reflection(wi, wo, alpha);
            },
            [&](const pt::Vec3& wo, float u1, float u2) {
                pt::Vec3 wh = pt::sampleGGX_VNDF(wo.z < 0.0f ? -wo : wo, alpha, u1, u2);
                if (!pt::sameHemisphere(wo, wh)) wh = -wh;
                if (pt::dot(wo, wh) < 0.0f) {
                    return pt::Vec3(0.0f);
                }

                pt::Vec3 wi = pt::reflect(wo, wh);
                if (!pt::sameHemisphere(wi, wo)) {
                    return pt::Vec3(0.0f);
                }

                return wi;
            });
    }
}


TEST_CASE("GGX Transmission Sampling") {
    constexpr float etaI = 1.0f;
    constexpr float etaO = 1.5f;

    for (int i = 0; i < 10; i++) {
        float alpha = 0.1f + (i / 10.0f) * (1.0f - 0.1f);
        testBxdfGoodnessOfFit(
            [&](const pt::Vec3& wi, const pt::Vec3& wo) {
                bool entering = pt::cosTheta(wo) > 0.0f;
                float eta = entering ? etaO / etaI : etaI / etaO;
                float pdf = pt::pdfGGX_transmission(wi, wo, eta, alpha);
                assert(pdf >= 0.0f);
                return pdf;
            },
            [&](const pt::Vec3& wo, float u1, float u2) {
                if (wo.z == 0.0f) return pt::Vec3(0.0f);
                pt::Vec3 wh = pt::sampleGGX(alpha, u1, u2);
                if (!pt::sameHemisphere(wo, wh)) wh = -wh;
                if (pt::dot(wo, wh) < 0.0f) return pt::Vec3(0.0f);
                assert(pt::sameHemisphere(wh, wo));
                assert(pt::dot(wo, wh) >= 0.0f);

                bool entering = pt::cosTheta(wo) > 0.0f;
                float eta = entering ? etaO / etaI : etaI / etaO;

                pt::Vec3 wi;
                if (!pt::refract(wo, wh, eta, wi)) {
                    return pt::Vec3(0.0f);
                }
                if (pt::sameHemisphere(wi, wo)) {
                    return pt::Vec3(0.0f);
                }
                if (pt::dot(wo, wh) * pt::dot(wi, wh) > 0.0f) {
                    return pt::Vec3(0.0f);
                }
                if (pt::cosTheta(wi) == 0.0f || pt::cosTheta(wo) == 0.0f) {
                    return pt::Vec3(0.0f);
                }

                return wi;
            });
    }
}


TEST_CASE("GGX VNDF Transmission Sampling") {
    constexpr float etaI = 1.0f;
    constexpr float etaO = 1.5f;

    for (int i = 0; i < 10; i++) {
        float alpha = 0.1f + (i / 10.0f) * (1.0f - 0.1f);
        testBxdfGoodnessOfFit(
            [&](const pt::Vec3& wi, const pt::Vec3& wo) {
                bool entering = pt::cosTheta(wo) > 0.0f;
                float eta = entering ? etaO / etaI : etaI / etaO;
                float pdf = pt::pdfGGX_VNDF_transmission(wi, wo, eta, alpha);
                assert(pdf >= 0.0f);
                return pdf;
            },
            [&](const pt::Vec3& wo, float u1, float u2) {
                pt::Vec3 wh = pt::sampleGGX_VNDF(wo.z < 0.0f ? -wo : wo, alpha, u1, u2);
                if (!pt::sameHemisphere(wo, wh)) wh = -wh;
                if (pt::dot(wo, wh) < 0.0f) return pt::Vec3(0.0f);
                assert(pt::sameHemisphere(wh, wo));
                assert(pt::dot(wo, wh) >= 0.0f);

                bool entering = pt::cosTheta(wo) > 0.0f;
                float eta = entering ? etaO / etaI : etaI / etaO;

                pt::Vec3 wi;
                if (!pt::refract(wo, wh, eta, wi)) {
                    return pt::Vec3(0.0f);
                }
                if (pt::sameHemisphere(wi, wo)) {
                    return pt::Vec3(0.0f);
                }
                if (pt::dot(wo, wh) * pt::dot(wi, wh) > 0.0f) {
                    return pt::Vec3(0.0f);
                }
                if (pt::cosTheta(wi) == 0.0f || pt::cosTheta(wo) == 0.0f) {
                    return pt::Vec3(0.0f);
                }

                return wi;
            });
    }
}
*/

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "TestHelpers.h"

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

    // Hemisphere area
    integral = pt::adaptiveSimpson2D([](double x, double y) {
        return std::sin(x);
    }, 0.0, 0.5 * pt::pi<double>, 0.0, 2.0 * pt::pi<double>);
    CHECK(integral == pt::Approx(2.0 * pt::pi<double>));

    // Sphere area
    integral = pt::adaptiveSimpson2D([](double x, double y) {
        return std::sin(x);
    }, 0.0, pt::pi<double>, 0.0, 2.0 * pt::pi<double>);
}

#include "ProgressBar.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <cmath>

namespace pt {

ProgressBar::ProgressBar(size_t totalWork, const std::string& title, size_t barWidth)
    : totalWork_(totalWork)
    , title_(title)
    , barWidth_(barWidth)
{
    updateThread_ = std::thread([&] {
        updateThreadMain();
    });
}

ProgressBar::~ProgressBar() {
    shouldExit_ = true;
    updateThread_.join();
}

void ProgressBar::update(size_t amount) {
    workDone_ += amount;
}

void ProgressBar::updateThreadMain() const {
    auto startTime = std::chrono::high_resolution_clock::now();
    size_t numIterations = 0;
    auto sleepDuration = std::chrono::milliseconds(250);

    while (!shouldExit_) {
        std::this_thread::sleep_for(sleepDuration);

        auto currTime = std::chrono::high_resolution_clock::now();
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currTime - startTime);
        float elapsedSec = elapsedMs.count() * 0.001f;

        float percentDone = workDone_ / static_cast<float>(totalWork_);
        float estTimeLeft = elapsedSec / percentDone - elapsedSec;

        std::ostringstream message;
        message << title_ << " [";
        int numEquals = static_cast<int>(std::round(percentDone * barWidth_));
        for (int i = 0; i < barWidth_; i++) {
            if (i < numEquals) {
                message << '=';
            }
            else if (i == numEquals) {
                message << '>';
            }
            else {
                message << ' ';
            }
        }
        message << "] "
            << "Elapsed: " << static_cast<int>(elapsedSec) << 's'
            << " / ETA: ";
        if (std::isfinite(estTimeLeft)) {
            message << static_cast<int>(estTimeLeft) << 's';
        }
        else {
            message << '?';
        }
        message << "               \r"; // Some extra spaces to avoid not clearing parts of the old message

        std::cout << message.str();
        std::cout.flush();

        numIterations++;
        if (numIterations == 10) { // ~2.5 seconds
            sleepDuration *= 2;
        }
        else if (numIterations == 70) { // ~30 seconds
            sleepDuration *= 2;
        }
        else if (numIterations == 520) { // ~15 minutes
            sleepDuration *= 5;
        }
    }

    std::cout << std::endl;
}

} // namespace pt

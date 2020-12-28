#pragma once

#include <string>
#include <atomic>
#include <thread>

namespace pt {

class ProgressBar {
public:
    ProgressBar(size_t totalWork, const std::string& title, size_t barWidth = 50);
    ~ProgressBar();

    void update(size_t amount = 1);

private:
    void updateThreadMain() const;
    
    size_t totalWork_;
    std::string title_;
    size_t barWidth_;
    std::thread updateThread_;
    std::atomic<size_t> workDone_ = 0;
    std::atomic<bool> shouldExit_ = false;
};

} // namespace pt

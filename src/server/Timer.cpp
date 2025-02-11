#include "Timer.hpp"

Timer::Timer(int ms) : interval(ms) { 
    lastUpdate = std::chrono::steady_clock::now(); 
}

bool Timer::hasElapsed() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate);
    return duration.count() >= interval;
}

void Timer::reset() { 
    lastUpdate = std::chrono::steady_clock::now(); 
}

void Timer::decreaseInterval(int amount) {
    if (interval > 100) {
        interval -= amount;
    }
}

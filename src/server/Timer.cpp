#include "Timer.hpp"

Timer::Timer(int ms) : interval(ms) { 
    lastUpdate = steady_clock::now(); 
}

bool Timer::hasElapsed() {
    auto now = steady_clock::now();
    auto duration = duration_cast<std::chrono::milliseconds>(now - lastUpdate);
    return duration.count() >= interval;
}

void Timer::reset() { 
    lastUpdate = steady_clock::now(); 
}

void Timer::decreaseInterval(int amount) {
    interval -= amount;
}

void Timer::setInterval(int ms) {
    interval = ms;
}

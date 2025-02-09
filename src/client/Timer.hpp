#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer {
public:
    Timer(int ms);
    bool hasElapsed();
    void reset();

private:
    std::chrono::steady_clock::time_point lastUpdate;
    int interval;
};

#endif

#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

using namespace std::chrono;

class Timer {
    steady_clock::time_point lastUpdate;
    int interval;

    public:
        Timer(int ms);
        bool hasElapsed();
        void reset();
        void decreaseInterval(int amount);    
};

#endif

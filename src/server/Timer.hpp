#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

using namespace std::chrono;

class Timer {
    steady_clock::time_point lastUpdate;
    int interval;

    public:
        Timer(int ms=1000);
        bool hasElapsed();
        void reset();
        void decreaseInterval(int amount);  
        void setInterval(int ms);  
        int getInterval() const { return interval; }
        steady_clock::time_point getLastUpdate() const { return lastUpdate; }
};

#endif

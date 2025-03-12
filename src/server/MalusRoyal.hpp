#ifndef MALUSROYAL_HPP
#define MALUSROYAL_HPP

#include "TetraminoDisplacement.hpp"

class MalusRoyal {
    
    TetraminoDisplacement displacement;
    
    public:
        MalusRoyal(TetraminoDisplacement& displacement);
        void reverseControl();
        void blockControl();
        void increaseSpeed();
        void turnOffLight();
        void clear2x2Block();
};

#endif
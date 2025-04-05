#ifndef MALUSROYAL_HPP
#define MALUSROYAL_HPP

#include "TetraminoDisplacement.hpp"
#include "Game.hpp"

class MalusRoyal {
    
    std::shared_ptr<Game> game;
    
    public:
        MalusRoyal(std::shared_ptr<Game> game);
        void reverseControl();
        void blockControl();
        void increaseSpeed();
        void turnOffLight();
        void clear2x2Block();
};

#endif
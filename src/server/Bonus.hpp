#ifndef BONUS_HPP
#define BONUS_HPP

#include <iostream>
#include <memory>
#include "Game.hpp"

class Bonus {
    TetraminoDisplacement displacement;
    public:
        Bonus(TetraminoDisplacement& displacement);
        void MiniBlock();
        void decreaseSpeed();
};

#endif
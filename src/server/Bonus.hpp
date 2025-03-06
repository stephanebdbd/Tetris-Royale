#ifndef BONUS_HPP
#define BONUS_HPP

#include <iostream>
#include <memory>
#include "Game.hpp"

class Bonus {
    std::shared_ptr<TetraminoDisplacement> displacement;
    public:
        Bonus(std::shared_ptr<TetraminoDisplacement> displacement);
        void MiniBlock();
        void decreaseSpeed();
};

#endif
#ifndef BONUS_HPP
#define BONUS_HPP

#include <iostream>
#include <memory>
#include "Game.hpp"

class Bonus {
    //TetraminoDisplacement displacement;
    std::shared_ptr<Game> game;
    public:
        Bonus(std::shared_ptr<Game> game);
        void miniBlock();
        void decreaseSpeed();
};

#endif
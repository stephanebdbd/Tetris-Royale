#ifndef BONUS_HPP
#define BONUS_HPP

#include <iostream>
#include "Game.hpp"

class Bonus {
    Game& game;
    public:
        Bonus(Game& game);
        void MiniBlock();
        void decreaseSpeed();
};

#endif
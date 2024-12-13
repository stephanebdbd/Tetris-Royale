#pragma once

#include "../include.hpp"
#include "../Model/Game.hpp"


class LeadBoard{
    int score;
    public:
    void UpdateScore(int score);

};

class PlayerBoard{
    Game *game_;
    const char space[5] = "    ";
public:
    PlayerBoard(Game *game);
    void clear();
    void display();
};
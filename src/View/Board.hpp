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
    const float FPS = 1 / 60.f;
    //bool gridState;
    //Tetrimino Tetrimino;
public:
    PlayerBoard(Game *game);
    void UpdateBoard();
};
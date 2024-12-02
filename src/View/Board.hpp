#pragma once

#include "../include.hpp"
#include "../Model/Tetrimino.hpp"


class LeadBoard{
    int score;
    public:
    void UpdateScore(int score);

};

class PlayerBoard{
    bool gridState;
    Tetrimino Tetrimino;
public:
    void UpdateBoard();
    void RenderBoard();
};
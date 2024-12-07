#pragma once

#include "../include.hpp"
#include "utils.hpp"


class Tetrimino{
    TetriminoType type;
    Colour colour;
    std::vector<Position> blocks;
    void chooseColor();
    std::vector<Position> setTetriminoBlocks();
public:
    Tetrimino(TetriminoType type);
    std::vector<Position> getBlocks();
    Colour getColour();
};
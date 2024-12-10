#pragma once

#include "../include.hpp"
#include "utils.hpp"


class Tetrimino{
    TetriminoType type;
    Colour colour;
    void chooseColor();
public:
    Tetrimino(TetriminoType type);
    std::vector<Position> getBlocks(TetriminoType type);
    Colour getColour();
    TetriminoType getType();
    ~Tetrimino()=default;
};
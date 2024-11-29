#pragma once

#include "../include.hpp"
#include "utils.hpp"
#include "Grid.hpp"

class Tetrimino{
    Grid* grid;
    const int boxDimension = 3;
    const int amountBlocks = 4;
    std::vector<std::vector<Cell*>>* gridMatrix;
    Position upperLeft;
    TetriminoType type;
    Colour colour;
    void chooseColor();
    std::vector<Position> setTetriminoBlocks();
    Colour setColour(std::vector<Position> blocks);
    int checkColoration(Position position, Position position2, std::vector<Position>* blocksPositions);
public:
    Tetrimino(TetriminoType type, Position upperLeft, Grid* grid);
    void colorate(std::vector<Position> blocs);
    void rotate(bool clockwise);
    Colour getColour();
};
#include <iostream>
#include <vector>
#include "Grid.hpp"
#include <array>

using namespace std;

enum class TetriminoType {
    I,
    O,
    T,
    S,
    Z,
    L,
    J,
};

enum class Direction {
    LEFT,
    RIGHT,
};

class Tetrimino{
    Grid* grid;
    const int boxDimension = 3;
    const int amountBlocks = 4;
    vector<vector<Cell*>>* gridMatrix;
    Position upperLeft;
    TetriminoType type;
    Colour colour;
    vector<Position> setTetriminoBlocks(TetriminoType type);
    Colour setColour(TetriminoType type, vector<Position> blocks);
public:
    Tetrimino(TetriminoType type, Position upperLeft, Grid* grid);
    int checkColoration(Position position, Position position2, vector<Position>* blocksPositions);
    void colorate(vector<Position> blocs);
    void rotate(bool clockwise);
    Colour getColour();
    void move(Direction direction);
    void moveBlocks(Direction direction);
};
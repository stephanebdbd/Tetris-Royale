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
    vector<vector<Cell*>>* grid;
    Position upperLeft;
    TetriminoType type;
    Colour colour;
    array<Position, 4> setTetriminoBlocks(TetriminoType type);
    Colour setColour(TetriminoType type, array<Position, 4> blocks);
public:
    Tetrimino(TetriminoType type, Position upperLeft, vector<vector<Cell*>>* grid);
    void rotate(bool clockwise);
    void setPosition(Position position1, Position position2);
    Colour getColour();
    bool move(int x, int y);
};
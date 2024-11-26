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

class Tetrimino{
    TetriminoType type;
    array<Position,4> blocks;
    Colour colour;
    int rotation;
public:
    Tetrimino(TetriminoType type);
    void rotate(bool clockwise);
    bool move(int x, int y, Grid &grid);
    bool getColour();
};
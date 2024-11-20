#include <iostream>
#include <vector>
#include "grid.hpp"

enum class TetriminoType {I, O, T, S, Z, L, J};

class Tetrimino{
    TetriminoType type;
    Position blocks = new Position[4];
    int rotation;
public:
    Tetrimino();
    void rotate(bool clockwise);
    bool move(int x, int y, Grid &grid);
    ~Tetrimino();
};
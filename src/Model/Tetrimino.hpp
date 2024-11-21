#include <iostream>
#include <vector>
#include "Grid.hpp"
using namespace std;

enum class TetriminoType {I, O, T, S, Z, L, J};

class Tetrimino{
    TetriminoType type;
    array<Position,4> blocks;
    int rotation;
public:
    Tetrimino();
    void rotate(bool clockwise);
    bool move(int x, int y, Grid &grid);
};
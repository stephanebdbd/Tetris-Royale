#include <iostream>

enum class TetriminoType {I, O, T, S, Z, L, J}

class Tetrimino{
    private:
    TetriminoType type;
    int positionX;
    int positionY;
    int rotation;

    public:
    Tetrimino();
    void rotate(bool clockwise);
    bool move(int x, int y, Grid &grid)
};
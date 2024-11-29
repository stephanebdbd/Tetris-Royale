#pragma once

#include "../include.hpp"
#include "../Model/Tetrimino.hpp"
#include "../Model/Grid.hpp"

enum class Direction {Right, Left, Down};

class Controller{
    Tetrimino *tetramino_;
    Grid *grid_;

public:
    Controller(Tetrimino *Tetrimino, Grid *Grid);
    void move(Direction direction);
    void rotate(bool clockWise);
    bool canMove(Direction direction);
    void keyboardInput(int keyInput);
    ~Controller() = default;
};

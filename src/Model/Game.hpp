#include <iostream>
#include "Grid.hpp"
#include "Tetrimino.hpp"

class Game{
    Grid grid;
    Tetrimino currentTetrimino;
    Tetrimino nextTetrimino;
    int score;

    public:
    void start();
    void update();
    void UserInput();
    bool isRunning();
    void render();
};
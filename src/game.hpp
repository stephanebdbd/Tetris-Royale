#include <iostream>
#include <grid.hpp>
#include <tetrimino.hpp>
#include <game.hpp>

class Game{
    private:
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
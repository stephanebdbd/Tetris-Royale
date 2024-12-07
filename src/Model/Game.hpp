#pragma once

#include "../include.hpp"
#include "User.hpp"
#include "utils.hpp"
#include "Grid.hpp"
#include "Tetrimino.hpp"


class Game{
    Grid* grid;
    Player* player;
    Tetrimino* currentTetrimino;
    const int tetriminoSpace = 4;
    int score=0;
    int comboCount=0;
    int comboBis=0;
    bool stillPlaying = true;
public:
    Game(Player* player);
    void moveTetrimino(Direction direction);
    void rotateTetrimino();
    void pushDown();
    void updateScore(int lines, bool downBoost=false);
    bool isRunning();
    bool checkLines();
    bool checkCollision();
    void display();
    ~Game();
};
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
    bool isStillRunning=true;
    bool moved=true;
    void setHasMoved();
public:
    Game(Player* player);
    void addTetrimino(bool null=false);
    void moveTetrimino(Direction direction, bool downBoost=false);
    void rotateTetrimino();
    void updateScore(int lines, bool downBoost, Direction direction);
    bool isRunning();
    void checkLines(Direction direction, bool downBoost=false);
    bool checkCollision(Direction direction);
    bool getHasMoved();
    void display();
    ~Game();
};
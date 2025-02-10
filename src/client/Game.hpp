#ifndef GAME_HPP
#define GAME_HPP

#include "Tetramino.hpp"
#include "Timer.hpp"
#include "Score.hpp"

class Game {
public:
    Game(int gridWidth, int gridHeight);
    void run();

private:
    Grid grid;
    Tetramino currentPiece;
    Timer dropTimer;
    Score score;
    bool running;
    bool gameOver;
};

#endif

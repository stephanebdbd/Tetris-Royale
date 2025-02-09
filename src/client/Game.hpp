#ifndef GAME_HPP
#define GAME_HPP

#include "Grid.hpp"
#include "Tetramino.hpp"
#include "Timer.hpp"

class Game {
public:
    Game(int gridWidth, int gridHeight);
    void run();

private:
    Grid grid;
    Tetramino currentPiece;
    Timer dropTimer;
    bool running;
};

#endif

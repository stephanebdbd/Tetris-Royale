#ifndef GAME_HPP
#define GAME_HPP

#include "Tetramino.hpp"
#include "Timer.hpp"
#include "Score.hpp"

class Game {
    Grid grid;
    Tetramino currentPiece;
    Timer dropTimer;
    Score score;
    bool running;
    bool gameOver;

    public:
        Game(int gridWidth, int gridHeight);
        void run();
        void showGame();
        void userInput();
        void showGameOver();

        Grid& getGrid() { return grid; }
        Tetramino& getCurrentPiece() { return currentPiece; }
};

#endif

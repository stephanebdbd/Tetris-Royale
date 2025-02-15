#ifndef GAME_HPP
#define GAME_HPP

#include "Tetramino.hpp"
#include "Timer.hpp"
#include "Score.hpp"
#include "Menu.hpp"

class Game {
    Grid grid;
    Tetramino currentPiece;
    Timer dropTimer;
    Score score;
    Menu menu;
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
        std::string getMainMenu0() { return menu.getMainMenu0(); }  
        std::string getMainMenu1() { return menu.getMainMenu1(); }

};

#endif

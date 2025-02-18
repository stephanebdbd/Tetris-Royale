#ifndef GAME_HPP
#define GAME_HPP

#include "Tetramino.hpp"
#include "Timer.hpp"
#include "Score.hpp"
#include "Menu.hpp"
#include "malus.hpp"

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
        void update();
        int getNbrMalus(int nbrLineComplet)const;

        Grid& getGrid() { return grid; }
        Tetramino& getCurrentPiece() { return currentPiece; }
        json getMainMenu0() { return menu.getMainMenu0(); }  
        json getMainMenu1() { return menu.getMainMenu1(); }

};

#endif

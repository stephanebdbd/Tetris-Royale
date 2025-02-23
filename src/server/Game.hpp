#ifndef GAME_HPP
#define GAME_HPP

#include "Tetramino.hpp"
#include "Timer.hpp"
#include "Score.hpp"
#include "Menu.hpp"
#include "malus.hpp"
#include "GameMode.hpp"
#include <iostream>

class Game {
    Grid grid;
    Tetramino currentPiece;
    Timer dropTimer;
    Score score;
    Menu menu;
    std::unique_ptr<GameMode> gameMode;
    
    bool running;
    bool gameOver;

    bool needToSendGame = true;

    public:
        Game(int gridWidth, int gridHeight);
        void run();
        void showGame();
        void userInput();
        void showGameOver();
        void update();

        //Pour eviter le viole des lois de Demeter
        void moveCurrentPieceDown();
        void moveCurrentPieceRight();
        void moveCurrentPieceLeft();
        void rotateCurrentPiece();
        void dropCurrentPiece();

        Grid& getGrid() { return grid; }
        Tetramino& getCurrentPiece() { return currentPiece; }
        Score& getScore() { return score; }
        json getMainMenu0() { return menu.getMainMenu0(); }  
        json getMainMenu1() { return menu.getMainMenu1(); }

        bool getNeedToSendGame() { return needToSendGame; }
        void setNeedToSendGame(bool needToSendGame) { this->needToSendGame = needToSendGame; }

};

#endif

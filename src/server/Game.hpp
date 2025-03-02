#ifndef GAME_HPP
#define GAME_HPP

#include "Tetramino.hpp"
#include "TetraminoDisplacement.hpp"
#include "Timer.hpp"
#include "Score.hpp"
#include "Menu.hpp"
#include "malus.hpp"
#include "GameMode.hpp"
#include <iostream>

class Game {
    Grid grid;
    Score score;
    
    int linesCleared;
    bool running;
    bool gameOver;
    
    bool malus5Royal = false;
    
    TetraminoDisplacement displacement;
    
    public:
        Game(int gridWidth, int gridHeight);
        Game& operator=(const Game& game);
        void run();
        void showGame();
        void showGameOver();
        void timerHandler() { displacement.timerHandler(); }
        Grid& getGrid() { return grid; }
        void moveTetramino(const std::string& action) { displacement.keyInputGameMenu(action); }
        TetraminoDisplacement& getDisplacement() { return displacement; } 
        Score& getScore() { return score; }
        int getLinesCleared() const { return linesCleared; }
        void setNeedToSendGame(bool needToSendGame) { displacement.setNeedToSendGame(needToSendGame); }
        bool getNeedToSendGame() const { return displacement.getNeedToSendGame(); }
        Tetramino& getCurrentPiece() { return displacement.getCurrentPiece(); }
        bool getIsGameOver() const { return displacement.getIsGameOver(); }
        void setGameOver() { displacement.setGameOver(); }
        void setmalus5Royal(bool malus5Royal) { this->malus5Royal = malus5Royal; }

};

#endif

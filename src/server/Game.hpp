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
    
    bool needToSendGame = true;
    bool malus5Royal = false;
    
    TetraminoDisplacement displacement;
    
    public:
        Game(int gridWidth, int gridHeight);
        Game& operator=(const Game& game);
        void run();
        void showGame();
        void showGameOver();
        Grid& getGrid() { return grid; }
        TetraminoDisplacement& getDisplacement() { return displacement; } 
        Score& getScore() { return score; }
        int getLinesCleared() { return linesCleared; }
        bool getNeedToSendGame() { return needToSendGame; }
        void setNeedToSendGame(bool needToSendGame) { this->needToSendGame = needToSendGame; }
        void setmalus5Royal(bool malus5Royal) { this->malus5Royal = malus5Royal; }

};

#endif

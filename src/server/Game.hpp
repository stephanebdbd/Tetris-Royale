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
#include <optional>

class Game {
    Grid grid;
    Score score;
    
    int linesCleared;
    bool running;
    
    bool malus5Royal = false;
    
    TetraminoDisplacement displacement = TetraminoDisplacement(grid);
    
    public:
        Game(int gridWidth, int gridHeight, Score& score, int speed=1000);

        void run();
        void showGame();
        void showGameOver();
        void updateGame();
        Grid& getGrid() { return grid; }
        void moveTetramino(const std::string& action) { displacement.keyInputGameMenu(action); }
        TetraminoDisplacement& getDisplacement() { return displacement; }
        int getLinesCleared() const { return linesCleared; }
        void setNeedToSendGame(bool needToSendGame) { displacement.setNeedToSendGame(needToSendGame); }
        bool getNeedToSendGame() const { return displacement.getNeedToSendGame(); }
        Tetramino& getCurrentPiece() { return displacement.getCurrentPiece(); }
        bool getIsGameOver() const;
        void setGameOver();
        void setmalus5Royal(bool malus5Royal) { this->malus5Royal = malus5Royal; }
        void setSpeed(int speed);

};

#endif

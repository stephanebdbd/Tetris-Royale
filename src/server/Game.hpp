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
    
    TetraminoDisplacement displacement{grid};
    
    public:
        Game(int gridWidth, int gridHeight, int speed=1000);

        void run();
        void showGame();
        void showGameOver();
        void updateGame();
        void moveTetramino(const std::string& action) { displacement.keyInputGameMenu(action); }
        TetraminoDisplacement& getDisplacement() { return displacement; }
        Grid& getGrid() { return grid; }
        int getLinesCleared() const { return linesCleared; }
        void setNeedToSendGame(bool needToSendGame) { displacement.setNeedToSendGame(needToSendGame); }
        bool getNeedToSendGame() const { return displacement.getNeedToSendGame(); }
        json scoreToJson() const;
        json gridToJson() const;
        json tetraminoToJson() const { return displacement.tetraminoToJson(); }
        bool getIsGameOver() const;
        void setGameOver();
        void setmalus5Royal(bool malus5Royal) { this->malus5Royal = malus5Royal; }
        void setSpeed(int speed);
        int getScore() const { return score.getScore(); }

        //Bonus Royal
        void applyMiniTetraminoBonus(){displacement.applyMiniTetraminoBonus();}
        void setSpeedBonusMalus(bool bonus) { displacement.setSpeedBonusMalus(bonus); }
        void applySpeedBonusMalus(int speed) {displacement.applySpeedBonusMalus(speed);}

        //Malus Royal
        void setCommandIsReversed(bool malus) { displacement.setCommandIsReversed(malus); }
        void setBlockCommand(bool malus) { displacement.setBlockCommand(malus); }
        void setlightBlocked(bool block) { displacement.setlightBlocked(block); }
        void random2x2MaskedBlock() { displacement.random2x2MaskedBlock(); }
};

#endif
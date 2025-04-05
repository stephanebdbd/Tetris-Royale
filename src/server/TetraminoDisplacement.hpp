#ifndef TETRAMINODISPLACEMENT_HPP
#define TETRAMINODISPLACEMENT_HPP

#include <iostream>
#include "Timer.hpp"
#include "Tetramino.hpp"

class TetraminoDisplacement {
    Grid& grid;
    
    Tetramino currentPiece;
    Tetramino nextPiece;
    
    Timer dropTimer;
    bool blockCommand = false;
    bool lightisBlocked = false;
    bool gameOver = false;
    bool needToSendGame = true;
    bool SpeedBonusMalus = false;
    bool commandIsBlocked = false;
    Timer bonusCounter1;

    std::string ch= "";
    bool commandIsReversed = false;
    bool inverseCommand = false;
    int counterMalus1 = -1;
    int counterMalus2 = -1;
    bool test = false;
    int speedtest = 0;
public:
    TetraminoDisplacement(Grid& grid, int speed=1000);
    void keyInputGameMenu(const std::string& action);
    void moveCurrentPieceDown();
    void moveCurrentPieceRight();
    void moveCurrentPieceLeft();
    void rotateCurrentPiece();
    void dropCurrentPiece();
    void update();
    void drawPiece();
    void setNeedToSendGame(bool needToSendGame);
    bool getNeedToSendGame() const;
    bool getIsGameOver() const;
    void setGameOver();
    Grid& getGrid() { return grid; }
    void setCurrentPiece(std::array<std::array<char, 4>, 4> shape) { 
        this->currentPiece.setCurrentShape(shape); }
    Tetramino& getCurrentPiece() { return currentPiece; }
    Tetramino& getNextPiece() { return nextPiece; }
    void setSpeed(int newSpeed);
    void setSpeedBonusMalus(bool bonus) { SpeedBonusMalus = bonus; }
    //void setEnter(int enter) { ch = enter; }
    void setBlockCommand(bool block);
    void setlightBlocked(bool block);
    void randomPosition();
    void random2x2MaskedBlock();
    const std::array<std::array<char, 4>, 4>& getCurrentShape() const {
        return currentPiece.getCurrentShape();
    }
    
    void applyMiniTetraminoBonus(){
        currentPiece.applyMiniTetraminoBonus();
    }

    void applySpeedBonusMalus(int speed);

    //Malus Royal
    void applyMalus1();
    void setCommandIsReversed(bool malus) { commandIsReversed = malus; }
};

#endif
#ifndef TETRAMINODISPLACEMENT_HPP
#define TETRAMINODISPLACEMENT_HPP

#include <iostream>
#include <atomic>
#include "Timer.hpp"
#include "Tetramino.hpp"

class TetraminoDisplacement {
    std::shared_ptr<Grid> grid;
    
    Tetramino currentPiece;
    Timer dropTimer;
    bool gameOver=false;
    bool needToSendGame = true;
    bool commandisBlocked;
    bool lightisBlocked;
    bool bonus1Royal = false;
    int ch;
public:
    TetraminoDisplacement(std::shared_ptr<Grid>grid);
    TetraminoDisplacement(const TetraminoDisplacement& other) = default;
    TetraminoDisplacement& operator=(const TetraminoDisplacement& displacement);
    void keyInputGameMenu(const std::string& action);
    void moveCurrentPieceDown();
    void moveCurrentPieceRight();
    void moveCurrentPieceLeft();
    void rotateCurrentPiece();
    void dropCurrentPiece();
    void update();
    void drawPiece();
    void setNeedToSendGame(bool needToSendGame);
    bool getNeedToSendGame() const { return needToSendGame; }
    bool getIsGameOver() const { return gameOver; }
    void setGameOver() { this->gameOver = false; }
    std::shared_ptr<Grid> getGrid() { return grid; }
    void setCurrentPiece(std::array<std::array<char, 4>, 4> shape) { currentPiece.setCurrentShape(shape); }
    Tetramino& getCurrentPiece() { return currentPiece; }
    void setSpeed(int amount) { dropTimer.decreaseInterval(amount); }
    void setBonus1Royal(bool bonus) { bonus1Royal = bonus; }
    void setEnter(int enter) { ch = enter; }
    void setBlockCommand(bool block);
    void setlightBlocked(bool block);
    void randomPosition();
    void random2x2MaskedBlock();
};

#endif
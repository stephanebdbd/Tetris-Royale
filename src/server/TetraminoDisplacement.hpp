#ifndef TETRAMINODISPLACEMENT_HPP
#define TETRAMINODISPLACEMENT_HPP

#include <iostream>
#include <mutex>
#include "Timer.hpp"
#include "Tetramino.hpp"

class TetraminoDisplacement {
    std::shared_ptr<Grid> grid;
    
    Tetramino currentPiece;
    Timer dropTimer;
    bool commandisBlocked;
    bool lightisBlocked;
    mutable std::mutex mutex;
    bool gameOver=false;
    bool needToSendGame = true;
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
    bool getNeedToSendGame() const;
    bool getIsGameOver() const;
    void setGameOver();
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
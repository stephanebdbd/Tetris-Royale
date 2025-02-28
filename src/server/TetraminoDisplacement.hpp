#ifndef TETRAMINODISPLACEMENT_HPP
#define TETRAMINODISPLACEMENT_HPP

#include <iostream>
#include "Timer.hpp"
#include "Tetramino.hpp"

class TetraminoDisplacement {
    Tetramino currentPiece;
    Grid grid;
    Timer dropTimer;
    bool needToSendGame;
    bool gameOver;
    bool commandisBlocked;
    bool lightisBlocked;
    int ch;
    bool bonus1Royal = false;
public:
    TetraminoDisplacement(Grid& grid, bool& needToSendGame, bool& gameOver);
    void keyInputGameMenu(const std::string& action);
    void moveCurrentPieceDown();
    void moveCurrentPieceRight();
    void moveCurrentPieceLeft();
    void rotateCurrentPiece();
    void dropCurrentPiece();
    void timerHandler();
    void manageUserInput();
    void setNeedToSendGame(bool needToSendGame);
    void drawPiece();
    Grid& getGrid() { return grid; }
    void setCurrentPiece(std::array<std::array<char, 4>, 4> shape) { currentPiece.setCurrentShape(shape); }
    void setSpeed(int amount) { dropTimer.decreaseInterval(amount); }
    void setBonus1Royal(bool bonus) { bonus1Royal = bonus; }
    void setEnter(int enter) { ch = enter; }
    void setBlockCommand(bool block);
    void setlightBlocked(bool block);
    void randomPosition();
    void random2x2MaskedBlock();
};

#endif

#endif
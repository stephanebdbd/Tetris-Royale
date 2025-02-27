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
    Tetramino& getCurrentPiece() { return currentPiece; }
};

#endif
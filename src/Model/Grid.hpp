#pragma once

#include "../include.hpp"
#include "utils.hpp"
#include "Tetrimino.hpp"



class Grid{
    static constexpr int width=12;
    static constexpr int height=25;
    const int amountBlocks = 4;
    Position upperLeft;
    int boxDimension = 3;
    std::vector<std::vector<Cell*>> gridMatrix;
    Tetrimino* currentTetrimino;
    Colour* currentColour;
    TetriminoType* currentType;
    std::vector<Position>* currentBlocks;
    int checkColoration(Position position, Position position2, std::vector<Position>* newBlocks);
public:
    Grid();
    void setBoxDimension();
    void addTetrimino(TetriminoType type);
    void moveTetrimino(Direction direction);
    void rotateTetrimino();
    void moveTetrimino(Direction direction); 
    void moveToTheSides(Direction direction);
    Colour Grid::setTetriminoColour();
    void makeFall();                         
    bool checkCollision();
    void checkLines();
    void colorate();
    bool isInTetrimino(Position position, std::vector<Position>* newBlocks=nullptr);
    ~Grid();
};

class Cell{
    static constexpr int width=12;
    static constexpr int height=25;
    static constexpr int tetriminoSpace = 5;
    bool isColoured = false;
    bool isOutline = false;
    Colour colour = Colour::WHITE;
    Position position;
    void setOutline();
public:
    Cell(int x, int y);
    void setColour(Colour newColour);
    void setdefaultColour();
    void setPosition(Position newPosition);
    bool getIsColoured();
    Position getPosition();
    Colour getColour();
};
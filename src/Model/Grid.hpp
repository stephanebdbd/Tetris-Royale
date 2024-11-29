#pragma once

#include "../include.hpp"
#include "utils.hpp"
#include "Tetrimino.hpp"



class Grid{
    static constexpr int width=12;
    static constexpr int height=25;
    std::vector<std::vector<Cell*>> gridMatrix;
    Tetrimino* CurrentTetrimino;
public:
    Grid();
    void addTetrimino(TetriminoType type, Position upperLeft);
    std::vector<std::vector<Cell*>>* getGrid();
    void moveTetrimino(Direction direction);
    void rotateTetrimino(bool clockwise);
    void checkLines();                          // A implémenter
    void moveTetrimino(Direction direction);    // A implémenter
    void moveToTheSides(Direction direction);   // A implémenter
    void makeFall();                            // A implémenter
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
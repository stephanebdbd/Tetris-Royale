#pragma once
#include <iostream>
#include "Tetrimino.hpp"
#include <vector>
#include <string>


class Grid{
    static constexpr int width=12;
    static constexpr int height=25;
    vector<vector<Cell*>> gridMatrix;
    Tetrimino* CurrentTetrimino;
public:
    Grid();
    void addTetrimino(TetriminoType type, Position upperLeft);
    vector<vector<Cell*>>* getGrid();
    void moveTetrimino(Direction direction);
    void rotateTetrimino(bool clockwise);
    void checkLines();                          // A implémenter
    void moveTetrimino(Direction direction);    // A implémenter
    void moveToTheSides(Direction direction);   // A implémenter
    void makeFall();                            // A implémenter
    ~Grid();
};


enum class Colour{
    BLACK = '⬛',    // Outline colour
    WHITE = '⬜',    // Default colour
    ORANGE = '🟧',
    BLUE = '🟦',
    YELLOW = '🟨',
    GREEN = '🟩',
    RED = '🟥',
    PURPLE = '🟪',
    BROWN = '🟫',
};


struct Position{
    int x;
    int y;
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
#ifndef GRID_HPP
#define GRID_HPP


#include <iostream>
#include "Tetrimino.hpp"
#include <vector>
#include <string>


class Grid{
    static constexpr int width=12;
    static constexpr int height=25;
    Grid* grid;
    vector<vector<Cell*>> gridMatrix;
public:
    Grid();
    bool addTetramino(TetriminoType type, Position upperLeft);
    void setPositions(Position position1, Position position2);
    vector<vector<Cell*>>* getGrid();
    ~Grid();
};


enum class Colour{
    BLACK = '⬛',
    ORANGE = '🟧',
    BLUE = '🟦',
    YELLOW = '🟨',
    GREEN = '🟩',
    RED = '🟥',
    PURPLE = '🟪',
    BROWN = '🟫',
};

enum class Outline{
    SIDES = '│',
    BOTTOM = '─',
    ANGLE_DOWN_LEFT = '└',
    ANGLE_DOWN_RIGHT = '┘',
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
    Outline outline;
    Colour colour = Colour::BLACK;
    Position position;
    void setOutline();
public:
    Cell(int x, int y);
    void setColour(Colour colour);
    void setdefaultColour();
    void setPosition(int x, int y);
    bool getIsColoured();
    bool getIsOutline();
    int getPositionX();
    int getPositionY();
};

#endif GRID_HPP
#ifndef GRID_HPP
#define GRID_HPP


#include <iostream>
#include "Tetrimino.hpp"
#include <vector>
#include <string>


class Grid{
    static constexpr int width=10;
    static constexpr int height=20;
    std::string outlineCharacters = "│└─┘│";
public:
    Grid();
    bool addTetramino(Tetrimino& piece);
    void display();
    ~Grid();
};

#endif // GRID_HPP

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


struct Position{
    int x;
    int y;
};

class Cell{
    bool isColoured;
    Colour colour;
    Position position;
public:
    Cell(int x, int y, Colour colour=Colour::BLACK, bool isColoured=false);
    void setColour(Colour colour);
    void setdefaultColour();
    bool isColoured();
    int getPositionX();
    int getPositionY();
};
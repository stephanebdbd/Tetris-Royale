#pragma once

#include "../include.hpp"
#include "utils.hpp"
#include "Tetrimino.hpp"


class Cell{
    static constexpr int width=12;
    static constexpr int height=25;
    static constexpr int tetriminoSpace = 4;
    bool isColoured = false;
    bool isOutline = false;
    Colour colour = Colour::WHITE;
    Position position;
    void setOutline();
public:
    Cell(int x, int y);
    void setColour(Colour newColour);
    void setdefaultColour();
    bool getIsColoured();
    Position getPosition();
    Colour getColour();
    void display();
};

class Grid{
    struct LinesStatus {
        int full;
        std::vector<int> coloured;
    };
    static constexpr int width=12;
    static constexpr int height=25;
    const int amountBlocks = 4;
    const int tetriminoSpace = 4;
    Position upperLeft;
    int boxDimension;
    std::vector<std::vector<Cell*>> gridMatrix;

    Tetrimino* currentTetrimino;
    Colour currentColour;
    TetriminoType currentType;
    std::vector<Position> currentBlocks;

    void setBoxDimension();
    void setTetriminoColour();
    bool checkColoration(Position position, std::vector<Position> newBlocks);
    void colorate();
    void exchangeColors(int tmp, int y);
    Position getMovement(Direction direction);
    bool getIsInGrid(Position position);
    bool isInTetrimino(Position position, std::vector<Position> otherBlocks={});
public:
    Grid();
    void addTetrimino(Tetrimino* tetrimino);
    void moveTetrimino(Direction direction);
    int checkLines();
    void rotateTetrimino();
    bool checkCollision(Direction direction);
    bool isTetriminoPlaced();
    bool isGameOver();
    void display();
    ~Grid();
};
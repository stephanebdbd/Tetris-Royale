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
    static constexpr int width=12;
    static constexpr int height=25;
    const int amountBlocks = 4;
    const int tetriminoSpace = 4;
    Position upperLeft;
    int boxDimension = 3;
    std::vector<std::vector<Cell*>> gridMatrix;

    Tetrimino* currentTetrimino;
    Colour currentColour; //
    TetriminoType currentType; //
    std::vector<Position> currentBlocks;

    void setBoxDimension();
    void setTetriminoColour();
    int checkColoration(Position position, Position position2, std::vector<Position> newBlocks);
    void colorate();
    void exchangeColors(int tmp, int y);
    Position getMovement(Direction direction);
public:
    Grid();
    void addTetrimino(Tetrimino* tetrimino);
    void moveTetrimino(Direction direction);
    void moveToTheSides(Direction direction);
    void goDown();
    void rotateTetrimino();
    bool isInTetrimino(Position position, std::vector<Position> newBlocks={});
    bool checkCollision(Direction direction);
    void checkLines(int* lines);
    bool isTetriminoPlaced();
    bool isGameOver();
    void display();
    ~Grid();
};
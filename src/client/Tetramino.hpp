#ifndef TETRAMINO_HPP
#define TETRAMINO_HPP

#include "Grid.hpp"
#include <array>
#include <cstdlib>
#include <ctime>
#include "Cell.hpp"

class Tetramino {
public:
    Tetramino(int startX, int startY, int gridWidth, int gridHeight);
    void draw() const;
    bool canRotate(const Grid &grid);
    void rotate();
    bool canMoveDown(const Grid &grid) const;
    void moveDown(Grid &grid);
    bool canMove(const Grid &grid, int dx, int dy) const;
    void moveLeft(Grid &grid);
    void moveRight(Grid &grid);
    void fixToGrid(Grid &grid, bool &gameOver);
    void reset(int startX, int startY);
    int chooseColor(char shapeSymbols) const;
    void colorOn(int color) const;
    void colorOff(int color) const;
    int getColor() const { return color; } 
    static void initializeColors();


private:
    Coord position;
    void initializeShapes();
    void selectRandomShape();

    std::array<std::array<char, 4>, 4> currentShape;
    std::vector<std::array<std::array<char, 4>, 4>> shapes;
    char shapeSymbols;
    int color;
    int gridWidth, gridHeight;
};

#endif

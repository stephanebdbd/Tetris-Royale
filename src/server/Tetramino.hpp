#ifndef TETRAMINO_HPP
#define TETRAMINO_HPP

#include "Grid.hpp"
#include <iostream>
#include <array>
#include <cstdlib>
#include <ctime>
#include <nlohmann/json.hpp> 

class Tetramino {
    Coord position;
    char shapeSymbols;
    int color;
    int gridWidth, gridHeight;

    std::array<std::array<char, 4>, 4> currentShape;
    std::vector<std::array<std::array<char, 4>, 4>> shapes;

    void initializeShapes();
    void selectRandomShape();

    public:
        Tetramino(int startX, int startY, int gridWidth, int gridHeight);
        void draw() const;
        void moveDown(Grid &grid);
        void moveLeft(Grid &grid);
        void moveRight(Grid &grid);
        void rotate();
        void fixToGrid(Grid &grid, bool &gameOver);
        bool canRotate(const Grid &grid);
        bool canMoveDown(const Grid &grid) const;
        bool canMove(const Grid &grid, int dx, int dy) const;
        void dropTetrimino(Grid &grid);
        void reset(int startX, int startY);
        void colorOn(int color) const;
        void colorOff(int color) const;
        void initializeColors() const;
        int chooseColor(char shapeSymbols) const;
        
        int getColor() const { return color; } 
        nlohmann::json tetraminoToJson() const;
};

#endif

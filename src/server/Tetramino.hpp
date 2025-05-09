#ifndef TETRAMINO_HPP
#define TETRAMINO_HPP

#include "Grid.hpp"
#include "Color.hpp"
#include <vector>
#include <array>
#include <memory>
#include "../common/json.hpp"

class Tetramino {
    Coord position;
    char shapeSymbols;
    int gridWidth, gridHeight;
    int dimension;
    std::array<std::array<char, 4>, 4> currentShape;
    std::vector<std::array<std::array<char, 4>, 4>> shapes;

    void initializeShapes();
    void selectRandomShape();

    bool bonusOneBlock = false;
    int counteurOneBlock = 1;
    bool lightBlocked = false;

    public:
        Tetramino(int startX, int startY, int gridWidth, int gridHeight);
        void draw() const;
        void moveDown(Grid& grid);
        void moveLeft(Grid& grid);
        void moveRight(Grid& grid);
        void rotate(const Grid& grid);
        void fixToGrid(Grid& grid, bool &gameOver);
        bool canRotate(const Grid& grid);
        bool canMove(const Grid& grid, int dx, int dy) const;
        void dropTetrimino(Grid& grid);
        void reset(int startX, int startY);
        void setCurrentShape(std::array<std::array<char, 4>, 4> shape);
        void arrangeShape();
        char getShapeSymbols() const { return shapeSymbols; }
        Color chooseColor(char shapeSymbol) const;
        
        json tetraminoToJson(bool isNext) const;
        const std::array<std::array<char, 4>, 4>& getCurrentShape() const;
        
        void applyMiniTetraminoBonus();
        void setlightBlocked(bool block) {lightBlocked = block;}
};

#endif
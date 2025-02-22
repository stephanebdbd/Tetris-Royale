#ifndef TETRAMINO_HPP
#define TETRAMINO_HPP

#include "Grid.hpp"
#include "Color.hpp"
#include <array>
#include "../common/json.hpp"

using json = nlohmann::json; 

class Tetramino {
    Coord position;
    char shapeSymbols;
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
        void rotate(const Grid &grid);
        void fixToGrid(Grid &grid, bool &gameOver);
        bool canRotate(const Grid &grid);
        bool canMoveDown(const Grid &grid) const;
        bool canMove(const Grid &grid, int dx, int dy) const;
        void dropTetrimino(Grid &grid);
        void reset(int startX, int startY);
        Color chooseColor(char shapeSymbol) const;
        
        json tetraminoToJson() const;
};

#endif
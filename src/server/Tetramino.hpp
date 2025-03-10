#ifndef TETRAMINO_HPP
#define TETRAMINO_HPP

#include "Grid.hpp"
#include "Color.hpp"
#include <array>
#include <vector>
#include <memory>
#include "../common/json.hpp"

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
        void moveDown(std::shared_ptr<Grid> grid);
        void moveLeft(std::shared_ptr<Grid> grid);
        void moveRight(std::shared_ptr<Grid> grid);
        void rotate(const std::shared_ptr<Grid> grid);
        void fixToGrid(std::shared_ptr<Grid> grid, bool &gameOver);
        bool canRotate(const std::shared_ptr<Grid> grid);
        bool canMoveDown(const std::shared_ptr<Grid> grid) const;
        bool canMove(const std::shared_ptr<Grid> grid, int dx, int dy) const;
        void dropTetrimino(std::shared_ptr<Grid> grid);
        void reset(int startX, int startY);
        void setCurrentShape(std::array<std::array<char, 4>, 4> shape) { currentShape = shape; };
        Color chooseColor(char shapeSymbol) const;
        
        json tetraminoToJson() const;
};

#endif
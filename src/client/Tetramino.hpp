#ifndef TETRAMINO_HPP
#define TETRAMINO_HPP

#include "Grid.hpp"
#include <array>
#include <cstdlib>
#include <ctime>

class Tetramino {
public:
    Tetramino(int startX, int startY, int gridWidth, int gridHeight);
    void draw() const;
    bool canMoveDown(const Grid &grid) const;
    void moveDown(Grid &grid);
    bool canMove(const Grid &grid, int dx, int dy) const;
    void moveLeft(Grid &grid);
    void moveRight(Grid &grid);
    void fixToGrid(Grid &grid);
    void reset(int startX, int startY);

private:
    Coord position;
    void initializeShapes();
    void selectRandomShape();
    void clear() const;

    std::array<std::array<char, 4>, 4> currentShape;
    std::vector<std::array<std::array<char, 4>, 4>> shapes;
    int gridWidth, gridHeight;
};

#endif

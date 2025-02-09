#ifndef GRID_HPP
#define GRID_HPP

#include <vector>
#include "Cell.hpp"

class Grid {
public:
    Grid(int width, int height);
    void draw() const;
    void markCell(int x, int y, char symbol);
    bool isCellOccupied(int x, int y) const;
    bool isLineComplete(int y) const;
    void clearLine(int y);
    int clearFullLines();
    int getWidth() const;
    int getHeight() const;

private:
    int width, height;
    std::vector<std::vector<Cell>> cells;
};

#endif

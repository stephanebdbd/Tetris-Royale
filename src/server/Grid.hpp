#ifndef GRID_HPP
#define GRID_HPP

#include <vector>
#include "Cell.hpp"

class Grid {
    int width, height;
    std::vector<std::vector<Cell>> cells;

    public:
        Grid(int width, int height);
        void draw();
        void markCell(int x, int y, char symbol, int color);
        void clearLine(int y);
        void applyGravity(); // TODO
        int clearFullLines();

        bool isCellOccupied(int x, int y) const;
        bool isLineComplete(int y) const;

        int getWidth() const { return width; }
        int getHeight() const { return height; }
};

#endif

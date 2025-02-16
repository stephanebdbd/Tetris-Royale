#ifndef GRID_HPP
#define GRID_HPP

#include <vector>
#include "Cell.hpp"
#include <nlohmann/json.hpp>
#include <ncurses.h>

using json = nlohmann::json;

class Grid {
    int width, height;
    std::vector<std::vector<Cell>> cells;

    public:
        Grid(int width, int height);

        void draw();
        void markCell(int x, int y, char symbol, int color);
        void clearLine(int y);
        void applyGravity();
        int clearFullLines();
        json gridToJson() const;

        bool isCellOccupied(int x, int y) const;
        bool isLineComplete(int y) const;

        int getWidth() const { return width; }
        int getHeight() const { return height; }

        Cell& getCell(int x, int y)  { return cells[y][x]; }
};


#endif

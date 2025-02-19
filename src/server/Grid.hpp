#ifndef GRID_HPP
#define GRID_HPP

#include "Cell.hpp"
#include "../common/json.hpp"
#include <vector>

using json = nlohmann::json;

class Grid {
    int width, height;
    std::vector<std::vector<Cell>> cells;

    public:
        Grid(int width, int height);

        void draw();
        void markCell(int x, int y, int color);
        void clearLine(int y);
        void applyGravity();
        int clearFullLines();
        json gridToJson() const;
        int heightPieces();
        void piecesUp(int nbrOffset);

        bool isCellOccupied(int x, int y) const;
        bool isLineComplete(int y) const;
        bool isLineEmpty(int y) const;

        int getWidth() const { return width; }
        int getHeight() const { return height; }

        Cell& getCell(int x, int y)  { return cells[y][x]; }
};

#endif
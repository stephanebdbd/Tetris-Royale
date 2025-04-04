#ifndef GRID_HPP
#define GRID_HPP

#include "Cell.hpp"
#include "Color.hpp"
#include "../common/json.hpp"
#include <vector>
#include <iostream>

class Grid {
    int width, height;
    std::vector<std::vector<Cell>> cells;
    int lightBlocked = false;

    public:
        Grid(int width, int height);

        void draw();
        void markCell(int x, int y, const Color& color);
        void clearLine(int y);
        void applyGravity();
        int clearFullLines();
        void clearCell(int x, int y);
        json gridToJson() const;
        int heightPieces();
        void piecesUp(int nbrOffset);
        void setLightBlocked(bool block) { lightBlocked = block; }
        bool isCellOccupied(int x, int y, std::vector<int> xPositions, std::vector<int> yPositions) const;
        bool isLineComplete(int y) const;
        bool isLineEmpty(int y) const;
        

        int getWidth() const { return width; }
        int getHeight() const { return height; }
        Color getColor(int x, int y) const { return cells[y][x].getColor(); }
        Cell& getCell(int x, int y)  { return cells[y][x]; }
};

#endif
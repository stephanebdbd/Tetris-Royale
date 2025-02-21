#ifndef CELL_HPP
#define CELL_HPP

#include "Color.hpp"

// Coordonnée d'une cellule
struct Coord {
    int x, y;
};

// Classe représentant une cellule de la grille
class Cell { 
        bool occupied;
        Color color;

    public:
        bool isOccupied() const { return occupied; }
        //char getSymbol() const { return symbol; }
        Color getColor() const { return color; }

        void setOccupied(bool value) { occupied = value; }
        void setColor(const Color& newColor) { color = newColor; }
        Cell() : occupied(false), color(Type::NONE) {}
};

#endif
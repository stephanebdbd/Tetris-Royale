#ifndef CELL_HPP
#define CELL_HPP

// Coordonnée d'une cellule
struct Coord {
    int x, y;
};

// Classe représentant une cellule de la grille
class Cell { 
        bool occupied;
        int color;

    public:
        bool isOccupied() const { return occupied; }
        //char getSymbol() const { return symbol; }
        int getColor() const { return color; }

        void setOccupied(bool value) { occupied = value; }
        //void setSymbol(char value) { symbol = value; }
        void setColor(int value) { color = value; }
        Cell() : occupied(false) {}
};

#endif
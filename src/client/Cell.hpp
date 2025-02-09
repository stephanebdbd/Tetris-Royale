#ifndef CELL_HPP
#define CELL_HPP

#include <ncurses.h>

class Cell {
public:
    Cell();
    void setOccupied(bool occupied);
    bool isOccupied() const;
    void setSymbol(char symbol);
    char getSymbol() const;

private:
    bool occupied;  // Indique si la cellule est occupée par un Tétramino
    char symbol;    // Le symbole du Tétramino (par exemple, 'X' pour un bloc)
};

#endif

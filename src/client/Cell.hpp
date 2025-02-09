#ifndef CELL_HPP
#define CELL_HPP

#include <ncurses.h>

// Coordonnée d'une cellule
struct Coord {
    int x, y;
};

// Classe représentant une cellule de la grille
class Cell {
public:
    bool occupied;
    char symbol;
    Cell() : occupied(false), symbol(' ') {}
};

#endif

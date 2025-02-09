#include "Grid.hpp"
#include <ncurses.h>

Grid::Grid(int w, int h) : width(w), height(h), cells(h, std::vector<Cell>(w)) {}

int Grid::getWidth() const { return width; }
int Grid::getHeight() const { return height; }
// Marquer une cellule comme occupée
void Grid::markCell(int x, int y, char symbol) {
    if (y >= 0 && y < height && x >= 0 && x < width) {
        cells[y][x].occupied = true;
        cells[y][x].symbol = symbol;
    }
}
// Vérifier si une cellule est occupée
bool Grid::isCellOccupied(int x, int y) const {
    if (cells[y][x].occupied || x < 1 || x >= width + 1 || y >= height) {
        return true;
    }
    return false;
}
void Grid::draw() const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            mvaddch(y, x, cells[y][x].symbol);
        }
    }
    
    // Dessiner les murs
    for (int y = 0; y <= height; ++y) {
        mvaddch(y, 0, '|'); // Mur gauche
        mvaddch(y, width + 1, '|'); // Mur droit
    }
    for (int x = 0; x <= width + 1; ++x) {
        mvaddch(height, x, '-'); // Mur bas
    }
}
    // Vérifier si une ligne est complète
bool Grid::isLineComplete(int y) const {
    for (int x = 0; x < width; ++x) {
        if (!cells[y][x].occupied) {
            return false; // Si une cellule est vide, la ligne n'est pas complète
        }
    }
    return true; // Si toutes les cellules sont occupées, la ligne est complète
}



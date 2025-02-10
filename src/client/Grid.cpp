#include "Grid.hpp"
#include <ncurses.h>
#include <iostream>

Grid::Grid(int w, int h) : width(w), height(h), cells(h, std::vector<Cell>(w + 1)) {}

int Grid::getWidth() const { return width; }
int Grid::getHeight() const { return height; }
// Marquer une cellule comme occupée
void Grid::markCell(int x, int y, char symbol, int color) {
    if (y >= 0 && y < height && x >= 1 && x <= width + 1) {
        cells[y][x].occupied = true;
        cells[y][x].symbol = symbol;
        cells[y][x].color = color;
    }
}
// Vérifier si une cellule est occupée
bool Grid::isCellOccupied(int x, int y) const {
    if (cells[y][x].occupied || x < 1 || x > width + 1 || y >= height) {
        return true;
    }
    return false;
}
void Grid::draw() {
    for (int y = 0; y < height; ++y) {
        for (int x = 1; x <= width; ++x) {
            int color = cells[y][x].color;
            if (cells[y][x].occupied) {
                attron(COLOR_PAIR(color));
                mvaddch(y, x, cells[y][x].symbol);
                attroff(COLOR_PAIR(color));
            }
            else {
                mvaddch(y, x, ' ');
            }
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
    for (int x = 1; x < width + 1; ++x) {
        if (!cells[y][x].occupied) {
            return false; // Si une cellule est vide, la ligne n'est pas complète
        }
    }
    return true; // Si toutes les cellules sont occupées, la ligne est complète
}


void Grid::clearLine(int y) {
    // Déplacer toutes les lignes au-dessus de cette ligne d'une case vers le bas
    for (int i = y; i > 0; --i) {
        for (int x = 1; x <= width; ++x) {  
            cells[i][x] = cells[i - 1][x]; // Copier la ligne du dessus
        }
    }
    for (int x = 1; x <= width; ++x) {
        mvaddch(y, x, ' '); // Effacer visuellement la ligne supprimée
    }
    refresh();
}

int Grid::clearFullLines() {
    int lines = 0;
    for (int y = 0; y < height; ++y) {
        if (isLineComplete(y)) {
            lines++;
            clearLine(y); // Supprimer la ligne et déplacer les autres
        }
    }
    return lines;
}




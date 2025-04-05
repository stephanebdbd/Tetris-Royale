#include "Grid.hpp"
#include "../common/jsonKeys.hpp"  

#include <ncurses.h>

Grid::Grid(int w, int h) : width(w), height(h), cells(h, std::vector<Cell>(w + 1)) {std::cout << "Grid created at : " << this << std::endl;}

// Marquer une cellule comme occupée
void Grid::markCell(int x, int y, const Color& color) {
    if ( !cells[y][x].isOccupied() && y >= 0 && y < height && x >= 1 && x <= width + 1) {
        cells[y][x].setOccupied(true);
        cells[y][x].setColor(color);
    }
}

// Vérifier si une cellule est occupée
bool Grid::isCellOccupied(int x, int y) const {
    if (x < 1 || x > width + 1 || y >= height) {
        std::cerr << "Cellule hors des limites de la grille. x : " << x << " et y : " << y << std::endl;
        return true; // Hors des limites de la grille
    }
    return cells[y][x].isOccupied(); // Vérifier si la cellule est occupée
}

void Grid::draw() {
    if(lightBlocked) return;
    
    for (int y = 0; y < height; ++y) {
        for (int x = 1; x <= width; ++x) {
            Color color = cells[y][x].getColor();
            if (cells[y][x].isOccupied()) {
                color.activate();
                int id = color.getId();
                mvprintw(y, x, "%d",  id);
                color.deactivate();
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
    for (int x = 1; x <= width; ++x) {
        if (!cells[y][x].isOccupied()) {
            return false; // Si une cellule est vide, la ligne n'est pas complète
        }
    }
    return true; // Si toutes les cellules sont occupées, la ligne est complète
}

void Grid::clearLine(int y) {
    for (int x = 1; x <= width; ++x) {
        cells[y][x].setOccupied(false);
        cells[y][x].setColor(Color(Type::NONE));
    }
}

void Grid::clearCell(int x, int y) {
    cells[y][x].setOccupied(false);
    cells[y][x].setColor(Color(Type::NONE));
}

void Grid::applyGravity() {
    for (int y = height - 1; y > 0; --y) {
        for (int x = 1; x <= width; ++x) {
            // si la cellule actuelle est vide et la cellule au-dessus est occupée
            if (!cells[y][x].isOccupied() && cells[y - 1][x].isOccupied()) {
                // Déplacer la cellule occupée vers le bas (donc dans la cellule actuelle)
                cells[y][x] = cells[y - 1][x];
                // Vider la cellule d'origine (donc la cellule au-dessus)
                cells[y - 1][x].setOccupied(false);
                cells[y - 1][x].setColor(Color(Type::NONE));
            }
        }
    }
}

int Grid::clearFullLines() {
    int linesCleared = 0;
    for (int y = height - 1; y >= 0; --y) { // Parcours du bas vers le haut
        if (isLineComplete(y)) {
            linesCleared++;
            clearLine(y);
            applyGravity();
            y++; // reverifier la ligne actuelle car elle a été déplacée vers le bas (gravité)
        }
    }
    return linesCleared;
}


json Grid::gridToJson() const {
    json gridJson;
    gridJson[jsonKeys::WIDTH] = width;
    gridJson[jsonKeys::HEIGHT] = height;
    gridJson[jsonKeys::CELLS] = json::array();
    gridJson[jsonKeys::LIGHT_GRID] = lightBlocked;

    for (int y = 0; y < height; ++y) {
        json row = json::array();
        for (int x = 1; x <= width; ++x) {
            json cell;
            cell[jsonKeys::OCCUPIED] = cells[y][x].isOccupied();
            cell[jsonKeys::COLOR] = cells[y][x].getColor().getId();
            row.push_back(cell);
        }
        gridJson[jsonKeys::CELLS].push_back(row);
    }
    return gridJson;
}


bool Grid::isLineEmpty(int y) const {
    for (int x = 1; x <= width; ++x) {
        if (cells[y][x].isOccupied()) {
            return false; 
        }
    }
    return true; 
}


int Grid::heightPieces(){
    for(int y = height - 1; y > 0; --y){
        if(isLineEmpty(y)) return y+1;
    }
    return 1;
}


void Grid::piecesUp(int nbrOffset){

    int h= heightPieces();

    for (int y = h; y < height; ++y) {
        for (int x = 1; x <= width; ++x) {  
            cells[y - nbrOffset][x] = cells[y][x];
            cells[y][x].setOccupied(false);
            cells[y][x].setColor(Color(Type::NONE));
        }
    }
}
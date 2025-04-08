#include "Grid.hpp"
#include "../common/jsonKeys.hpp"  

#include <ncurses.h>

Grid::Grid(int w, int h) : width(w), height(h), cells(h, std::vector<Cell>(w + 1)) {std::cout << "Grid created at : " << this << std::endl;}

// Marquer une cellule comme occupée
void Grid::markCell(int x, int y, const Color& color) {
    if (!cells[y][x].isOccupied() && y >= 0 && y < height && x >= 1 && x <= width + 1) {
        cells[y][x].setOccupied(true);
        cells[y][x].setColor(color);
    }
}

// Vérifier si une cellule est occupée
bool Grid::isCellOccupied(int x, int y, std::vector<int> xPositions, std::vector<int> yPositions) const {
    if (x < 1 || x > width + 1 || y >= height) {
        std::cerr << "Cellule hors des limites de la grille. x : " << x << " et y : " << y << std::endl;
        return true; // Hors des limites de la grille
    }

    if (cells[y][x].isOccupied()){
        for (auto& xPos : xPositions) {
            for (auto& yPos : yPositions){
                if (x == xPos && y == yPos)
                    return false; // La cellule est occupée par la même pièce
            }
        }
    }
    return cells[y][x].isOccupied();
}

void Grid::draw() {
    if (lightBlocked) return;
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
    for (int x = 1; x <= width; ++x)
        clearCell(x, y);
}

void Grid::clearCell(int x, int y) {
    cells[y][x].setOccupied(false);
    cells[y][x].setColor(Color(Type::NONE));
}

void Grid::exchangeState(int x, int y, int y1) {
    Color color = cells[y1][x].getColor();
    bool occupied = cells[y1][x].isOccupied();
    cells[y][x].setColor(color);
    cells[y][x].setOccupied(occupied);
}

void Grid::applyGravity(std::vector<int> clearLines) {
    int clearCells = 0, size = clearLines.size(), lines = 1;
    int start = clearLines[0], end = 0;
    for (int y = start; y >= 0; --y) {
        clearCells = 0;
        while ((lines < size) && ((y == clearLines[lines]) || (y - lines == clearLines[lines]))) 
            lines++;
        if (y - lines >= 0){
            for (int x = 1; x <= width; ++x) {
                if (!cells[y][x].isOccupied() && !cells[y - 1][x].isOccupied()) 
                    clearCells++;
                exchangeState(x, y, y - lines);
            }
        }
        else if (((clearCells == width) && (lines == size)) || (y - lines < 0)){
            start = (y - lines < 0) ? y - 1 : y - lines - 1;
            if (y - lines < 0)
                clearLine(y);
            end = start - lines + 1;
            break;
        }
    }
    
    for (int y = start; (y >= end) && (y >= 0); --y)
        clearLine(y);
}

int Grid::clearFullLines() {
    std::vector<int> linesToClear;
    for (int y = height - 1; y >= 0; --y) { // Parcours du bas vers le haut
        if (isLineComplete(y)){
            clearLine(y);
            linesToClear.push_back(y);
        }
    }
    if (!linesToClear.empty())
        applyGravity(linesToClear);
    return linesToClear.size();
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
#include "Tetramino.hpp"
#include "../common/jsonKeys.hpp"

#include <ncurses.h>
#include <stdlib.h>

Tetramino::Tetramino(int startX, int startY, int w, int h) 
    : position{startX, startY}, gridWidth(w), gridHeight(h) {
    initializeShapes();
    selectRandomShape(); // Choisir une forme aléatoire au départµ
}


void Tetramino::initializeShapes() {
    // Définir les formes des Tetraminos
    shapes = {
        // Forme I
        {{{'#', '#', '#', '#'},
          {' ', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '}}},
        
        // Forme L
        {{{' ', ' ', '#', ' '},
          {'#', '#', '#', ' '},
          {' ', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '}}},
        
        // Forme J
        {{{'#', ' ', ' ', ' '},
          {'#', '#', '#', ' '},
          {' ', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '}}},
        // Forme T
        {{{' ', '#', ' ', ' '},
          {'#', '#', '#', ' '},
          {' ', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '}}},
        // Forme O
        {{{' ', ' ', ' ', ' '},
          {' ', '#', '#', ' '},
          {' ', '#', '#', ' '},
          {' ', ' ', ' ', ' '}}},
        // Forme Z
        {{{'#', '#', ' ', ' '},
          {' ', '#', '#', ' '},
          {' ', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '}}},
        // Forme S
        {{{' ', '#', '#', ' '},
          {'#', '#', ' ', ' '},
          {' ', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '}}}
    };
};

// Sélectionner une forme aléatoire
void Tetramino::selectRandomShape() {
    srand(time(0));
    int index = rand() % shapes.size(); // Sélectionner un index aléatoire parmi les formes disponibles
    currentShape = shapes[index];
    switch (index) {
        case 0: shapeSymbols = 'I'; break; // I - Cyan
        case 1: shapeSymbols = 'L'; break; // L - Orange
        case 2: shapeSymbols = 'J'; break; // J - Bleu
        case 3: shapeSymbols = 'T'; break; // T - Magenta
        case 4: shapeSymbols = 'O'; break; // O - Jaune
        case 5: shapeSymbols = 'Z'; break; // Z - Rouge
        case 6: shapeSymbols = 'S'; break; // S - Vert
        default:shapeSymbols  = ' '; break;
    }
}

void Tetramino::moveDown(Grid& grid) {
    std::cout << "Tetramino::moveDown of grid : " << &grid << std::endl;
    if (canMoveDown(grid)) {
        position.y++;
    }
}

// Vérifie si la pièce peut descendre
bool Tetramino::canMoveDown(const Grid& grid) const {
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (currentShape[y][x] != ' ') {
                if (position.y + y + 1 >= gridHeight || grid.isCellOccupied(position.x + x, position.y + y + 1)) {
                    return false;
                }
            }
        }
    }
    return true;
}

void Tetramino::moveLeft(Grid& grid) {
    if (canMove(grid, -1, 0)) { // Vérifie si on peut bouger d'une case à gauche
        position.x--; // Déplacer la pièce à gauche
    }
}

void Tetramino::moveRight(Grid& grid) {
    if (canMove(grid, 1, 0)) { // Vérifie si on peut bouger d'une case à droite
        position.x++; // Déplacer la pièce à droite
    }
}

bool Tetramino::canMove(const Grid& grid, int dx, int dy) const {
    for (int y = 0; y < 4; ++y) { 
        for (int x = 0; x < 4; ++x) { 
                int newX = position.x + x + dx; 
                int newY = position.y + y + dy;
                if (newX < 1 || newX >= grid.getWidth() + 1 || newY >= grid.getHeight() || grid.isCellOccupied(newX, newY)) {
                    if (currentShape[y][x] != ' '){
                        return false; // Collision détectée
                    }
                }
        }
    }
    return true; // Aucun obstacle, déplacement possible
}

void Tetramino::dropTetrimino(Grid& grid) {
    while (canMoveDown(grid)) {
        moveDown(grid);
    }
}

void Tetramino::rotate(const Grid& grid) {
    if (!canRotate(grid)) {
        return;
    }
    // Créer une nouvelle matrice pour la forme après rotation
    std::array<std::array<char, 4>, 4> newShape = currentShape;

    // Effectuer la rotation de 90° à droite
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            newShape[x][3 - y] = currentShape[y][x];
        }
    }
    // Mettre à jour la forme actuelle après la rotation
    currentShape = newShape;
}

bool Tetramino::canRotate(const Grid& grid) {
    // Créer une copie de la forme actuelle pour vérifier la rotation
    std::array<std::array<char, 4>, 4> tempShape = currentShape;
    
    // Effectuer la rotation sur la copie
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            tempShape[x][3 - y] = currentShape[y][x];
        }
    }

    // Vérifier si la nouvelle forme (après rotation) entre en collision ou sort des limites
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            int newX = position.x + x;
            int newY = position.y + y;
            if (tempShape[y][x] != ' ') {
                if (newX < 1 || newX >= grid.getWidth() + 1 || newY >= grid.getHeight() || grid.isCellOccupied(newX, newY)) {
                    return false;
                }
            }
        }
    }
    return true; // La rotation est possible
}

void Tetramino::draw() const {
    Color color = chooseColor(shapeSymbols);
    color.activate();
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (currentShape[y][x] != ' ') {
                mvaddch(position.y + y, position.x + x, currentShape[y][x]);
            }
        }
    }
    color.deactivate();
}

Color Tetramino::chooseColor(char shapeSymbol) const {
    return Color::fromShapeSymbol(std::string(1, shapeSymbol));
}


// Fixer la pièce à sa position 
void Tetramino::fixToGrid(Grid& grid, bool &gameOver) {
    Color cellColor = chooseColor(shapeSymbols);
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (currentShape[y][x] != ' ') {
                int gridX = position.x + x;
                int gridY = position.y + y;
                if (gridY <= 1) { // Si la pièce est en dehors de la grille
                    gameOver = true; // Déclencher le game over
                    return;
                }
                grid.markCell(gridX, gridY, cellColor);
            }
        }
    }
}

json Tetramino::tetraminoToJson() const {
    json tetraminoJson;

    tetraminoJson[jsonKeys::X] = position.x;
    tetraminoJson[jsonKeys::Y] = position.y;

    json shapeJson = json::array();
    for (const auto& row : currentShape) {
        json rowJson = json::array();
        for (char cell : row) {
            rowJson.push_back(std::string(1, cell));
        }
        shapeJson.push_back(rowJson);
    }
    tetraminoJson[jsonKeys::SHAPE] = shapeJson;
    tetraminoJson[jsonKeys::SHAPE_SYMBOL] = shapeSymbols;

    return tetraminoJson;
}

// Générer un nouveau Tetramino
void Tetramino::reset(int startX, int startY) {
    position = {startX, startY};
    selectRandomShape(); // Sélectionner une nouvelle forme aléatoire
}
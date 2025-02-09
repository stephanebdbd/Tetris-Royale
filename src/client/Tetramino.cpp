#include "Tetramino.hpp"

Tetramino::Tetramino(int startX, int startY, int w, int h) 
    : position{startX, startY}, gridWidth(w), gridHeight(h) {
    initializeShapes();
    selectRandomShape(); // Choisir une forme aléatoire au départ
}

void Tetramino::initializeShapes() {
    // Définir les formes des Tetraminos
    shapes = {
        // Forme I
        {{{' ', '#', ' ', ' '},
          {' ', '#', ' ', ' '},
          {' ', '#', ' ', ' '},
          {' ', '#', ' ', ' '}}},
        
        // Forme L
        {{{' ', ' ', ' ', ' '},
          {'#', '#', '#', ' '},
          {'#', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '}}},
        
        // Forme J
        {{{' ', ' ', ' ', ' '},
          {'#', '#', '#', ' '},
          {' ', ' ', '#', ' '},
          {' ', ' ', ' ', ' '}}},
        // Forme T
        {{{' ', ' ', ' ', ' '},
          {'#', '#', '#', ' '},
          {' ', '#', ' ', ' '},
          {' ', ' ', ' ', ' '}}},
        // Forme O
        {{{' ', ' ', ' ', ' '},
          {'#', '#', ' ', ' '},
          {'#', '#', ' ', ' '},
          {' ', ' ', ' ', ' '}}},
        // Forme Z
        {{{' ', ' ', ' ', ' '},
          {'#', '#', ' ', ' '},
          {' ', '#', '#', ' '},
          {' ', ' ', ' ', ' '}}},
        // Forme S
        {{{' ', ' ', ' ', ' '},
          {' ', '#', '#', ' '},
          {'#', '#', ' ', ' '},
          {' ', ' ', ' ', ' '}}}
    };
};
// Sélectionner une forme aléatoire
void Tetramino::selectRandomShape() {
    srand(time(0));
    int index = rand() % shapes.size(); // Sélectionner un index aléatoire parmi les formes disponibles
    currentShape = shapes[index];
}

// Vérifie si la pièce peut descendre
bool Tetramino::canMoveDown(const Grid &grid) const {
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

void Tetramino::moveDown(Grid &grid) {
    if (canMoveDown(grid)) {
        clear();
        position.y++;
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
void Tetramino::moveLeft(Grid &grid) {
    if (canMove(grid, -1, 0)) { // Vérifie si on peut bouger d'une case à gauche
        clear();
        position.x -= 1; // Déplacer la pièce à gauche
    }
}
void Tetramino::moveRight(Grid &grid) {
    if (canMove(grid, 1, 0)) { // Vérifie si on peut bouger d'une case à droite
        clear();
        position.x += 1; // Déplacer la pièce à droite
    }
}

void Tetramino::draw() const {
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (currentShape[y][x] != ' ') {
                mvaddch(position.y + y, position.x + x, currentShape[y][x]);
            }
        }
    }
}

bool Tetramino::canRotate(const Grid &grid) {
    // Créer une copie de la forme actuelle pour vérifier la rotation
    std::array<std::array<char, 4>, 4> tempShape = currentShape;
    
    // Effectuer la rotation sur la copie
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            tempShape[x][3 - y] = currentShape[y][x];
        }
    }

    // Vérifier si la nouvelle forme (après rotation) entre en collision
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (tempShape[y][x] != ' ' && grid.isCellOccupied(position.x + x, position.y + y)) {
                return false; // Il y a une collision
            }
        }
    }

    return true; // La rotation est possible
}

void Tetramino::rotate() {
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

void Tetramino::clear() const {
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (currentShape[y][x] != ' ') {
                mvaddch(position.y + y, position.x + x, ' '); // Efface l'ancienne position
            }
        }
    }
}
// Fixer la pièce à sa position et vérifier les lignes complètes
void Tetramino::fixToGrid(Grid &grid) {
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (currentShape[y][x] != ' ') {
                grid.markCell(position.x + x, position.y + y, currentShape[y][x]);
            }
        }
    }
    //grid.clearFullLines(); // Vérifier et supprimer les lignes complètes TODO
}
// Générer un nouveau Tetramino
void Tetramino::reset(int startX, int startY) {
    position = {startX, startY};
    selectRandomShape(); // Sélectionner une nouvelle forme aléatoire
}
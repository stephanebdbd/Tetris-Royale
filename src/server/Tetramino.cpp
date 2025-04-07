#include "Tetramino.hpp"
#include "../common/jsonKeys.hpp"

#include <ncurses.h>
#include <stdlib.h>

Tetramino::Tetramino(int startX, int startY, int w, int h) 
    : position{startX, startY}, gridWidth(w), gridHeight(h), dimension(4) {
    initializeShapes();
    selectRandomShape(); // Choisir une forme aléatoire au départµ
}


void Tetramino::initializeShapes() {
    // Définir les formes des Tetraminos
    shapes = {
        // Forme I
        {{{' ', ' ', ' ', ' '},
          {'#', '#', '#', '#'},
          {' ', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '}}},
        
        // Forme L
        {{{' ', ' ', ' ', ' '},
          {' ', ' ', '#', ' '},
          {'#', '#', '#', ' '},
          {' ', ' ', ' ', ' '}}},
        
        // Forme J
        {{{' ', ' ', ' ', ' '},
          {'#', ' ', ' ', ' '},
          {'#', '#', '#', ' '},
          {' ', ' ', ' ', ' '}}},
        // Forme T
        {{{'#', '#', '#', ' '},
          {' ', '#', ' ', ' '},
          {' ', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '}}},
        // Forme O
        {{{'#', '#', ' ', ' '},
          {'#', '#', ' ', ' '},
          {' ', ' ', ' ', ' '},
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
          {' ', ' ', ' ', ' '}}},
        
        // Forme 1x1 (bonus)
        {{{'#', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '},
          {' ', ' ', ' ', ' '}}}
    };
};

// Sélectionner une forme aléatoire
void Tetramino::selectRandomShape() {
    srand(time(0));
    if (bonusOneBlock){
        currentShape = shapes[7]; // prends la pièce d'un bloc 
        dimension = 1;
        counteurOneBlock += 1;
        if (counteurOneBlock == 2){
            bonusOneBlock = false;
            counteurOneBlock = 0;
        }
        return;
    }
    int index = rand() % (shapes.size()-1); // Sélectionner un index aléatoire parmi les formes disponibles
    currentShape = shapes[index];
    switch (index) {
        case 0:
            dimension = 4;
            shapeSymbols = 'I';
            break; // I - Cyan
        case 1:
            dimension = 3;
            shapeSymbols = 'L';
            break; // L - Orange
        case 2:
            dimension = 3;
            shapeSymbols = 'J';
            break; // J - Bleu
        case 3:
            dimension = 3;
            shapeSymbols = 'T';
            break; // T - Magenta
        case 4:
            dimension = 2;
            shapeSymbols = 'O';
            break; // O - Jaune
        case 5:
            dimension = 3;
            shapeSymbols = 'Z';
            break; // Z - Rouge
        case 6:
            dimension = 3;
            shapeSymbols = 'S';
            break; // S - Vert
        default:
            shapeSymbols = ' '; // Forme inconnue
            break;
    }
}

void Tetramino::moveDown(Grid& grid) {
    std::cout << "Tetramino::moveDown of grid : " << &grid << std::endl;
    if (canMove(grid, 0, 1)) { // Vérifie si on peut bouger d'une case vers le bas
        position.y++;
    }
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
    std::vector<int> xPositions;
    std::vector<int> yPositions;
    for (int y = 0; y < dimension; ++y) {
        for (int x = 0; x < dimension; ++x) {
            if (currentShape[y][x] != ' ' && x < 1 && x >= gridWidth + 1) {
                xPositions.push_back(position.x + x + dx);
                yPositions.push_back(position.y + y + dy);
            }
        }
    }

    for (int y = 0; y < dimension; ++y) {
        for (int x = 0; x < dimension; ++x) {
            if (currentShape[y][x] != ' ') {
                int newX = position.x + x + dx;
                int newY = position.y + y + dy;
                if (newX < 1 || newX >= gridWidth + 1 || newY >= gridHeight || grid.isCellOccupied(newX, newY, xPositions, yPositions)) {
                    return false; // Collision ou hors des limites
                }
            }
        }
    }
    return true; // Aucun obstacle, déplacement possible
}

void Tetramino::dropTetrimino(Grid& grid) {
    while (canMove(grid, 0, 1)) { // Vérifie si la pièce peut descendre
        moveDown(grid);
    }
}

void Tetramino::rotate(const Grid& grid) {
    if (!canRotate(grid))
        return; // Vérifie si la rotation est possible avant de l'effectuer
    
        // Créer une nouvelle matrice pour la forme après rotation
    std::array<std::array<char, 4>, 4> newShape = currentShape;
    // Effectuer la rotation de 90° à droite
    for (int y = 0; y < dimension; ++y) {
        for (int x = 0; x < dimension; ++x) {
            newShape[y][x] = currentShape[x][dimension - 1 - y];
        }
    }
    // Mettre à jour la forme actuelle après la rotation
    currentShape = newShape;
}

bool Tetramino::canRotate(const Grid& grid) {
    Coord tempPosition = position; // Conserver la position actuelle
    arrangeShape();

    // Créer une copie de la forme actuelle pour vérifier la rotation
    std::array<std::array<char, 4>, 4> tempShape = currentShape;
    std::vector<int> xPositions; 
    std::vector<int> yPositions;
    // Effectuer la rotation sur la copie
    for (int y = 0; y < dimension; ++y) {
        for (int x = 0; x < dimension; ++x) {
            tempShape[x][dimension - 1 - y] = currentShape[y][x];
            if (currentShape[y][x] != ' ' && x < 1 && x >= gridWidth + 1) {
                xPositions.push_back(position.x + x);
                yPositions.push_back(position.y + y);
            }
        }
    }

    // Vérifier si la nouvelle forme (après rotation) entre en collision ou sort des limites
    for (int y = 0; y < dimension; ++y) {
        for (int x = 0; x < dimension; ++x) {
            if (tempShape[y][x] != ' ') {
                int newX = position.x + x;
                int newY = position.y + y;
                if (newX < 1 || newX >= gridWidth + 1 || newY >= gridHeight || grid.isCellOccupied(newX, newY, xPositions, yPositions)) {
                    // Rétablir la position d'origine si la rotation n'est pas possible
                    position = tempPosition;                   
                    return false;
                }
            }
        }
    }
    return true; // La rotation est possible
}

void Tetramino::setCurrentShape(std::array<std::array<char, 4>, 4> shape) {
    for (const auto& row : currentShape) {
        for (char cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    } 
    this->currentShape = shape;
    for (const auto& row : currentShape) {
        for (char cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    } 
}

void Tetramino::arrangeShape() {
    int direction = ((position.x < (gridWidth + 1) / 2) > 0) ? 1 : -1;
    while ((position.x < 1) || (position.x + dimension > gridWidth + 1))
        position.x += direction; // Ajuster la position de la pièce pour qu'elle reste dans les limites de la grille
}

void Tetramino::draw() const {
    Color color = chooseColor(shapeSymbols);
    color.activate();
    for (int y = 0; y < dimension; ++y) {
        for (int x = 0; x < dimension; ++x) {
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
    for (int y = 0; y < dimension; ++y) {
        for (int x = 0; x < dimension; ++x) {
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

const std::array<std::array<char, 4>, 4>& Tetramino::getCurrentShape() const {
    return currentShape;
}

json Tetramino::tetraminoToJson(bool isNext) const {
    json tetraminoJson;
    if (!isNext) {
        tetraminoJson[jsonKeys::X] = position.x;
        tetraminoJson[jsonKeys::Y] = position.y;
    }
    else {
        tetraminoJson[jsonKeys::X] = 13;
        tetraminoJson[jsonKeys::Y] = 4;
    }

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

    tetraminoJson[jsonKeys::LIGHT_TETRA] = lightBlocked;


    return tetraminoJson;
}

// Générer un nouveau Tetramino
void Tetramino::reset(int startX, int startY) {
    position = {startX, startY};
    selectRandomShape(); // Sélectionner une nouvelle forme aléatoire
}

void Tetramino::applyMiniTetraminoBonus(){
    bonusOneBlock = true;
}
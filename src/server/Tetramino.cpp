#include "Tetramino.hpp"

Tetramino::Tetramino(int startX, int startY, int w, int h) 
    : position{startX, startY}, gridWidth(w), gridHeight(h) {
    initializeShapes();
    selectRandomShape(); // Choisir une forme aléatoire au départµ
}

void Tetramino::initializeColors() const {
    start_color();
    use_default_colors(); // Garde le fond du terminal
    init_pair(1, COLOR_RED, -1);    // Z - Rouge
    init_pair(2, COLOR_YELLOW, -1); // O - Jaune
    init_pair(3, COLOR_GREEN, -1);  // S - Vert
    init_pair(4, COLOR_CYAN, -1);   // I - Cyan
    init_pair(5, COLOR_BLUE, -1);   // J - Bleu
    init_pair(6, COLOR_MAGENTA, -1);// T - Magenta
    init_pair(7, COLOR_WHITE, -1); // L - Blanc et pas Orange car pas de couleur orange (max 8 couleur dont noir et blanc)
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
        {{{' ', ' ', ' ', '#'},
          {' ', '#', '#', '#'},
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
    color = chooseColor(shapeSymbols);
}

void Tetramino::moveDown(Grid &grid) {
    if (canMoveDown(grid)) {
        position.y++;
    }
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

void Tetramino::moveLeft(Grid &grid) {
    if (canMove(grid, -1, 0)) { // Vérifie si on peut bouger d'une case à gauche
        position.x--; // Déplacer la pièce à gauche
    }
}

void Tetramino::moveRight(Grid &grid) {
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

void Tetramino::dropTetrimino(Grid &grid) {
    while (canMoveDown(grid)) {
        moveDown(grid);
    }
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

bool Tetramino::canRotate(const Grid &grid) {
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
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (currentShape[y][x] != ' ') {
                int color = chooseColor(shapeSymbols);
                colorOn(color);
                mvaddch(position.y + y, position.x + x, currentShape[y][x]);
                colorOff(color);
            }
        }
    }
}

int Tetramino::chooseColor(char shapeSymbol) const {
    switch (shapeSymbol) {
        case 'Z': return 1; // Rouge
        case 'O': return 2; // Jaune
        case 'S': return 3; // Vert
        case 'I': return 4; // Cyan
        case 'J': return 5; // Bleu
        case 'T': return 6; // Magenta
        case 'L': return 7; // Blanc / Orange
        default: return 0;  // Aucune couleur
    }
}

void Tetramino::colorOn(int color) const {
    initializeColors();
    attron(COLOR_PAIR(color));
}

void Tetramino::colorOff(int color) const {
    attroff(COLOR_PAIR(color));
}

// Fixer la pièce à sa position 
void Tetramino::fixToGrid(Grid &grid, bool &gameOver) {
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (currentShape[y][x] != ' ') {
                int gridX = position.x + x;
                int gridY = position.y + y;
                if (gridY <= 1) { // Si la pièce est en dehors de la grille
                    gameOver = true; // Déclencher le game over
                    return;
                }
                grid.markCell(gridX, gridY, currentShape[y][x], color); // Marquer la cellule comme occupée
            }
        }
    }
}

json Tetramino::tetraminoToJson() const {
    json tetraminoJson;

    tetraminoJson["x"] = position.x;
    tetraminoJson["y"] = position.y;

    json shapeJson = json::array();
    for (const auto& row : currentShape) {
        json rowJson = json::array();
        for (char cell : row) {
            rowJson.push_back(std::string(1, cell));
        }
        shapeJson.push_back(rowJson);
    }

    tetraminoJson["shape"] = shapeJson;
    tetraminoJson["symbol"] = std::string(1, shapeSymbols);

    return tetraminoJson;
}

// Générer un nouveau Tetramino
void Tetramino::reset(int startX, int startY) {
    position = {startX, startY};
    selectRandomShape(); // Sélectionner une nouvelle forme aléatoire
}
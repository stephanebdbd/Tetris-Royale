#include <vector>
#include <ncurses.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <array>
#include <iostream>

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

// Classe représentant la grille de jeu
class Grid {
private:
    int width, height;
    std::vector<std::vector<Cell>> cells;
public:
    Grid(int w, int h) : width(w), height(h), cells(h, std::vector<Cell>(w)) {}
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Marquer une cellule comme occupée
    void markCell(int x, int y, char symbol) {
        if (y >= 0 && y < height && x >= 0 && x < width) {
            cells[y][x].occupied = true;
            cells[y][x].symbol = symbol;
        }
    }

    // Vérifier si une cellule est occupée
    bool isCellOccupied(int x, int y) const {
        if (cells[y][x].occupied) {
            return true;
        }
        
    }

    void draw() const {
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
    bool isLineComplete(int y) const {
        for (int x = 0; x < width; ++x) {
            if (!cells[y][x].occupied) {
                return false; // Si une cellule est vide, la ligne n'est pas complète
            }
        }
        return true; // Si toutes les cellules sont occupées, la ligne est complète
    }

    // Supprimer une ligne complète et décaler les lignes au-dessus
    void clearLine(int y) {
        // Déplacer toutes les lignes au-dessus de cette ligne d'une case vers le bas
        for (int i = y; i > 0; --i) {
            for (int x = 0; x < width; ++x) {
                cells[i][x] = cells[i - 1][x]; // Copier les données de la ligne du dessus
            }
        }
        // Vider la première ligne (celle qui est devenue vide après le décalage)
        for (int x = 0; x < width; ++x) {
            cells[0][x].occupied = false;
            cells[0][x].symbol = ' ';
        }
    }

    // Vérifier et supprimer les lignes complètes
    void clearFullLines() {
        for (int y = 0; y < height; ++y) {
            if (isLineComplete(y)) {
                printw("Ligne %d complète!\n", y);
                clearLine(y); // Supprimer la ligne et déplacer les autres
            }
        }
    }

};

// Classe représentant un Tetramino avec toutes ses formes possibles
class Tetramino {
private:
    Coord position;
    std::array<std::array<char, 4>, 4> currentShape; // Forme actuelle
    std::vector<std::array<std::array<char, 4>, 4>> shapes; // Liste de toutes les formes possibles
    int gridWidth, gridHeight;

    void initializeShapes() {
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
    }

    // Sélectionner une forme aléatoire
    void selectRandomShape() {
        srand(time(0));
        int index = rand() % shapes.size(); // Sélectionner un index aléatoire parmi les formes disponibles
        currentShape = shapes[index];
    }

public:
    Tetramino(int startX, int startY, int w, int h) 
        : position{startX, startY}, gridWidth(w), gridHeight(h) {
        initializeShapes();
        selectRandomShape(); // Choisir une forme aléatoire au départ
    }

    // Vérifie si la pièce peut descendre
    bool canMoveDown(const Grid &grid) {
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

    void moveDown(const Grid& grid) {
        if (canMoveDown(grid)) {
            clear();
            position.y++;
        }
    }

    void moveLeft(const Grid& grid) {
        if (canMove(grid, -1, 0)) { // Vérifie si on peut bouger d'une case à gauche
            clear();
            position.x -= 1; // Déplacer la pièce à gauche
        }
    }

    void moveRight(const Grid& grid) {
        if (canMove(grid, 1, 0)) { // Vérifie si on peut bouger d'une case à droite
            clear();
            position.x += 1; // Déplacer la pièce à droite
        }
    }

    bool canMove(const Grid& grid, int dx, int dy) const {
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

    void draw() const {
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                if (currentShape[y][x] != ' ') {
                    mvaddch(position.y + y, position.x + x, currentShape[y][x]);
                }
            }
        }
    }

    void clear() const {
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                if (currentShape[y][x] != ' ') {
                    mvaddch(position.y + y, position.x + x, ' '); // Efface l'ancienne position
                }
            }
        }
    }


    // Fixer la pièce à sa position
    // Fixer la pièce à sa position et vérifier les lignes complètes
    void fixToGrid(Grid &grid) {
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                if (currentShape[y][x] != ' ') {
                    grid.markCell(position.x + x, position.y + y, currentShape[y][x]);
                }
            }
        }
        grid.clearFullLines(); // Vérifier et supprimer les lignes complètes
    }


    // Générer un nouveau Tetramino
    void reset(int startX, int startY) {
        position = {startX, startY};
        selectRandomShape(); // Sélectionner une nouvelle forme aléatoire
    }
};

// Classe gérant le timer
class Timer {
private:
    std::chrono::steady_clock::time_point lastUpdate;
    int interval;
public:
    Timer(int ms) : interval(ms) { lastUpdate = std::chrono::steady_clock::now(); }
    bool hasElapsed() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate);
        return duration.count() >= interval;
    }
    void reset() { lastUpdate = std::chrono::steady_clock::now(); }
};

// Classe principale gérant le jeu
class Game {
private:
    Grid grid;
    Tetramino currentPiece;
    Timer dropTimer;
    bool running;
public:
    Game(int gridWidth, int gridHeight) 
        : grid(gridWidth, gridHeight), currentPiece(gridWidth / 2, 0, gridWidth, gridHeight), 
          dropTimer(1000), running(true) {}

    void run() {
        initscr();
        noecho();
        curs_set(0);
        nodelay(stdscr, TRUE); // Permet à getch de ne pas bloquer l'exécution

        while (running) {
            grid.draw();
            currentPiece.draw();

            // Si le timer a expiré, tenter de descendre la pièce
            if (dropTimer.hasElapsed()) {
                if (currentPiece.canMoveDown(grid)) {
                    currentPiece.moveDown(grid);
                } else {
                    // Fixer la pièce sur la grille et générer une nouvelle pièce
                    currentPiece.fixToGrid(grid);
                    currentPiece.reset(grid.getWidth() / 2, 0); // Réinitialiser la position de la pièce
                }
                dropTimer.reset();
            }

            int ch = getch(); // Lire une entrée utilisateur sans bloquer

            // Déplacer le Tetramino en fonction de la touche
            if (ch == 27) { // Touche ESC, qui marque le début d'une séquence
                ch = getch(); // Lire le prochain caractère
                if (ch == 91) { // Code de séquence des flèches
                    ch = getch(); // Lire la touche de direction
                    switch (ch) {
                        case 65: // Flèche haut
                            break; // Pas de mouvement vers le haut pour l'instant
                        case 66: // Flèche bas
                            currentPiece.moveDown(grid); // Déplacer vers le bas
                            break;
                        case 67: // Flèche droite
                            currentPiece.moveRight(grid); // Déplacer vers la droite
                            break;
                        case 68: // Flèche gauche
                            currentPiece.moveLeft(grid); // Déplacer vers la gauche
                            break;
                    }
                }
            }

            // Quitter le jeu si la touche 'q' est pressée
            if (ch == 'q') running = false;
        }
        endwin();
    }
};

int main() {
    Game game(10, 20);
    game.run();
    return 0;
}
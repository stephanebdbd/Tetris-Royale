#include "Game.hpp"

Game::Game(int gridWidth, int gridHeight) 
    : grid(gridWidth, gridHeight), 
      currentPiece(gridWidth / 2, 0, gridWidth, gridHeight), 
      dropTimer(1000), 
      score(gridWidth + 5, 2), // Position du score à droite de la grille
      running(true) {}

void Game::run() {
    initscr();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE); // Permet à getch de ne pas bloquer l'exécution
    while (running) {
        grid.draw();
        currentPiece.draw();
        score.display(); 
        // Si le timer a expiré, tenter de descendre la pièce
        if (dropTimer.hasElapsed()) {
            if (currentPiece.canMoveDown(grid)) {
                currentPiece.moveDown(grid);
            } else {
                // Fixer la pièce sur la grille et générer une nouvelle pièce
                currentPiece.fixToGrid(grid);

                // Vérifier combien de lignes ont été complétées
                int linesCleared = grid.clearFullLines();
                score.addScore(linesCleared); // Mise à jour du score
                
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
                        if (currentPiece.canRotate(grid)) {
                            currentPiece.rotate();
                        }
                        break;
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

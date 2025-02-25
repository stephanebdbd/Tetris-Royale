#include "Game.hpp"

#include <ncurses.h>

Game::Game(int gridWidth, int gridHeight) //ajouter ce parametre apres , std::unique_ptr<GameMode> gameMode 
    : grid(gridWidth, gridHeight),
      currentPiece(gridWidth / 2, 0, gridWidth, gridHeight), 
      dropTimer(1000), 
      score(gridWidth + 5, 2), // Position du score à droite de la grille
      running(true),
      gameOver(false)
      //gameMode(gameMode) il faut l ajouter apres
      {}

void Game::run() {
    initscr();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE); // Permet à getch de ne pas bloquer l'exécution
    keypad(stdscr, TRUE);  // Active la gestion des touches fléchées

    while (running && !gameOver) { 
        erase(); // Efface uniquement le contenu sans supprimer l'affichage
        showGame();

        // Si le timer a expiré, tenter de descendre la pièce
        if (dropTimer.hasElapsed()) {
            if (currentPiece.canMoveDown(grid)) {
                currentPiece.moveDown(grid);
            } else {
                currentPiece.fixToGrid(grid, gameOver);
                if (!gameOver) { 
                    int linesCleared = grid.clearFullLines();
                    
                    //gameMode.feautureMode(*this, linesCleared); il ne faut pas le supprimer

                    score.addScore(linesCleared);

                    dropTimer.decreaseInterval(5); // Diminue le temps d'attente entre chaque chute
                    currentPiece.reset(grid.getWidth() / 2, 0);
                }
                else {
                    running = false;
                }
            }
            dropTimer.reset();
        }
    

        userInput();
    }

    showGameOver();
   
}

void Game::showGame() {
    
    grid.draw();
    currentPiece.draw();
    score.display();
    
}

void Game::userInput() {
    int ch = getch();
    if (ch == KEY_UP) { currentPiece.rotate(grid); }
    if (ch == KEY_DOWN) { currentPiece.moveDown(grid); }
    if (ch == KEY_RIGHT) { currentPiece.moveRight(grid); }
    if (ch == KEY_LEFT) { currentPiece.moveLeft(grid); }
    if (ch == ' ') { currentPiece.dropTetrimino(grid); }
    if (ch == 'q') running = false;
    
}

void Game::showGameOver() {
    // Affichage immédiat du Game Over
    erase();
    mvprintw(grid.getHeight() / 2, grid.getWidth() / 2 - 5, "GAME OVER");
    refresh();
    nodelay(stdscr, FALSE); 
    getch(); 

    endwin(); // Restaure le terminal à son état initial
    
}

void Game::update() {
    if (!gameOver) {
        if (dropTimer.hasElapsed()) {
            needToSendGame = true;
            if (currentPiece.canMoveDown(grid)) {
                currentPiece.moveDown(grid);
            } 
            else {
                currentPiece.fixToGrid(grid, gameOver);
                if (!gameOver) { 
                    int linesCleared = grid.clearFullLines();
                    score.addScore(linesCleared);
                    dropTimer.decreaseInterval(5);
                    currentPiece.reset(grid.getWidth() / 2, 0);
                }
            }
            dropTimer.reset();
        }
    }
}



void Game::moveCurrentPieceRight() {
    currentPiece.moveRight(grid);
}

void Game::moveCurrentPieceLeft() {
    currentPiece.moveLeft(grid);
}

void Game::moveCurrentPieceDown() {
    currentPiece.moveDown(grid);
}

void Game::rotateCurrentPiece() {
    currentPiece.rotate(grid);
}

void Game::dropCurrentPiece() {
    currentPiece.dropTetrimino(grid);
}

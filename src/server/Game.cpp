#include "Game.hpp"

#include <ncurses.h>

Game::Game(int gridWidth, int gridHeight) //ajouter ce parametre apres , std::unique_ptr<GameMode> gameMode 
    : grid(gridWidth, gridHeight), 
      score(gridWidth + 5, 2), // Position du score à droite de la grille
      running(true),
      gameOver(false), 
      displacement(grid, needToSendGame, gameOver)
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

        displacement.timerHandler();
        if (!gameOver){
            linesCleared = grid.clearFullLines();
            //gameMode.feautureMode(*this, linesCleared); il ne faut pas le supprimer
            score.addScore(linesCleared);
        }
        else {
            running = false;
        }
        displacement.manageUserInput();
    }

    showGameOver();
   
}

void Game::showGame() {
    
    grid.draw();
    displacement.drawPiece();
    score.display();
    
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
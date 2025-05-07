#include "Game.hpp"
#include <ncurses.h>

Game::Game(int gridWidth, int gridHeight, int speed)
    : grid(gridWidth, gridHeight), 
      score(gridWidth+5, 2),
      running(true)
      {
    displacement.setSpeed(speed);
}

void Game::run() {
    initscr();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE); // Permet à getch de ne pas bloquer l'exécution
    keypad(stdscr, TRUE);  // Active la gestion des touches fléchées
    bool gameOver = displacement.getIsGameOver();
    while (running && !gameOver) { 
        erase(); // Efface uniquement le contenu sans supprimer l'affichage
        showGame();

        displacement.update();
        if (!gameOver){
            linesCleared = grid.clearFullLines();
            score.addScore(linesCleared);
        }
        else {
            running = false;
        }
        gameOver = displacement.getIsGameOver();
    }

    showGameOver();
   
}

void Game::updateGame() {
    displacement.update();
    linesCleared = grid.clearFullLines();
    score.addScore(linesCleared);
}

void Game::showGame() {
    if(malus5Royal){
        Timer malusCounter(5000);
        if(malusCounter.hasElapsed()){
            malus5Royal = false;
        }
        malusCounter.reset();
    }
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

void Game::setGameOver() {
    displacement.setGameOver();
    this->showGameOver();
}

void Game::setSpeed(int speed) {
    displacement.setSpeed(speed);
}

bool Game::getIsGameOver() const {
    return displacement.getIsGameOver();
}

json Game::gridToJson() const {
    return grid.gridToJson();
}

json Game::scoreToJson() const {
    return score.scoreToJson();
}
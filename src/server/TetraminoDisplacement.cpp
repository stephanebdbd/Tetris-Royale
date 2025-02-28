#include "TetraminoDisplacement.hpp"

TetraminoDisplacement::TetraminoDisplacement(Grid& grid, bool& needToSendGame, bool& gameOver)
    : grid(grid), currentPiece(grid.getWidth() / 2, 0, grid.getWidth(), grid.getHeight()), 
      dropTimer(1000), 
      needToSendGame(needToSendGame), gameOver(gameOver) {}

void TetraminoDisplacement::keyInputGameMenu(const std::string& action) {
    if (action == "right") { 
        moveCurrentPieceRight();
    }
    else if (action == "left") { 
        moveCurrentPieceLeft();
    }
    else if (action == "up") { 
        rotateCurrentPiece();
    }
    else if (action == "down"){
        moveCurrentPieceDown();
    }
    else if(action == "drop") { // space
        dropCurrentPiece();
    }
}

void TetraminoDisplacement::moveCurrentPieceRight() {
    currentPiece.moveRight(grid);
}

void TetraminoDisplacement::moveCurrentPieceLeft() {
    currentPiece.moveLeft(grid);
}

void TetraminoDisplacement::moveCurrentPieceDown() {
    currentPiece.moveDown(grid);
}

void TetraminoDisplacement::rotateCurrentPiece() {
    currentPiece.rotate(grid);
}

void TetraminoDisplacement::dropCurrentPiece() {
    currentPiece.dropTetrimino(grid);
}

void TetraminoDisplacement::timerHandler() {
    if (dropTimer.hasElapsed() && !gameOver) { // Si le timer a expiré, tenter de descendre la pièce
        this->setNeedToSendGame(true);
        if (currentPiece.canMoveDown(grid)) {
            currentPiece.moveDown(grid);
        } else {
            currentPiece.fixToGrid(grid, gameOver);
            if (!gameOver) { 
                if(bonus1Royal){
                    Timer bonusCounter(10000);
                    if(bonusCounter.hasElapsed()){
                        bonus1Royal = false;
                        setSpeed(30);
                    }
                    bonusCounter.reset();
                }
                else{
                    dropTimer.decreaseInterval(5); // Diminue le temps d'attente entre chaque chute
                }
                
                currentPiece.reset(grid.getWidth() / 2, 0);
            }
        }
        dropTimer.reset();
    }
}

void TetraminoDisplacement::manageUserInput() {
    ch = getch();
    if (ch == KEY_UP) { currentPiece.rotate(grid); }
    if (ch == KEY_DOWN) { currentPiece.moveDown(grid); }
    if (ch == KEY_RIGHT) { currentPiece.moveRight(grid); }
    if (ch == KEY_LEFT) { currentPiece.moveLeft(grid); }
    if (ch == ' ') { currentPiece.dropTetrimino(grid); }
    if (ch == 'q') gameOver = true;
}

void TetraminoDisplacement::setNeedToSendGame(bool needToSendGame) {
    this->needToSendGame = needToSendGame;
}

void TetraminoDisplacement::drawPiece() {
    currentPiece.draw();
}


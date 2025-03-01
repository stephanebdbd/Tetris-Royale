#include "TetraminoDisplacement.hpp"

TetraminoDisplacement::TetraminoDisplacement(Grid& grid, bool& needToSendGame, bool& gameOver)
    : grid(grid), needToSendGame(needToSendGame), gameOver(gameOver),
    currentPiece(grid.getWidth() / 2, 0, grid.getWidth(), grid.getHeight()), 
    dropTimer(1000), 
    commandisBlocked(false), lightisBlocked(false) {}

TetraminoDisplacement& TetraminoDisplacement::operator=(const TetraminoDisplacement& displacement) {
    if(this != &displacement) {
        this->grid = displacement.grid;
        this->needToSendGame = displacement.needToSendGame;
        this->gameOver = displacement.gameOver;
        this->currentPiece = displacement.currentPiece;
        this->dropTimer = displacement.dropTimer;
        this->commandisBlocked = displacement.commandisBlocked;
        this->lightisBlocked = displacement.lightisBlocked;
        this->ch = displacement.ch;
        this->bonus1Royal = displacement.bonus1Royal;
    }
    return *this;
}

void TetraminoDisplacement::keyInputGameMenu(const std::string& action) {
    if (action == "right") { 
        moveCurrentPieceRight();
        setNeedToSendGame(true);
    }
    else if (action == "left") { 
        moveCurrentPieceLeft();
        setNeedToSendGame(true);
    }
    else if (action == "up") { 
        rotateCurrentPiece();
        setNeedToSendGame(true);
    }
    else if (action == "down"){
        moveCurrentPieceDown();
        setNeedToSendGame(true);
    }
    else if(action == "drop") { // space
        dropCurrentPiece();
        setNeedToSendGame(true);
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
    if(commandisBlocked) return;

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
    if(lightisBlocked) return;
    currentPiece.draw();
}

void TetraminoDisplacement::setBlockCommand(bool block) {
    commandisBlocked = block;
}
void TetraminoDisplacement::setlightBlocked(bool block){
    lightisBlocked = block;
    grid.setLightBlocked(block);
}
void TetraminoDisplacement::random2x2MaskedBlock(){
    int x = rand()%(grid.getWidth() -1);
    int y = grid.heightPieces() + rand()%(grid.getHeight() -1);
    for(int i = 0; i<2 ;i++){
        for(int j = 0; j<2 ;j++){
            int x_clean = x+i;
            int y_clean = y+j;
            grid.clearCell(x_clean, y_clean);
            grid.markCell(x_clean, y_clean, grid.getColor(x_clean, y_clean - 1));
        }
        
    }
    grid.applyGravity();

}
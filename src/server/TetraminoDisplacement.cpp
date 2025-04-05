#include "TetraminoDisplacement.hpp"

TetraminoDisplacement::TetraminoDisplacement(Grid& grid, int speed)
    : grid(grid),
    currentPiece(grid.getWidth() / 2, 0, grid.getWidth(), grid.getHeight()), 
    dropTimer(speed), 
    commandisBlocked(false), lightisBlocked(false) {
        std::cout << "TetraminoDisplacement created." << std::endl;
}

void TetraminoDisplacement::keyInputGameMenu(const std::string& action) {
    std::cout << "Action reçue : " << action << std::endl;
    if(commandisBlocked) return;
    if (action == "right") { 
        moveCurrentPieceRight();
        setNeedToSendGame(true);
    }
    else if (action == "left") { 
        moveCurrentPieceLeft();
        setNeedToSendGame(true);
    }
    else if (action == "up") {
        if ((currentPiece.getShapeSymbols() == 'O')) // On vérifie le timer si la forme est carrée
            update();
        else {
            rotateCurrentPiece();
            setNeedToSendGame(true);
        }
    }
    else if (action == "down"){
        moveCurrentPieceDown();
        setNeedToSendGame(true);
    }
    else if (action == "drop") { // space
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

void TetraminoDisplacement::update() {
    if (dropTimer.hasElapsed() && !gameOver) { // Si le timer a expiré, tenter de descendre la pièce
        this->setNeedToSendGame(true);
        if (currentPiece.canMove(grid, 0, 1)) {
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

void TetraminoDisplacement::setNeedToSendGame(bool needToSendGame) {
    this->needToSendGame = needToSendGame;
}

bool TetraminoDisplacement::getNeedToSendGame() const {
    return needToSendGame;
}

void TetraminoDisplacement::setGameOver() {
    gameOver = true;
}

bool TetraminoDisplacement::getIsGameOver() const{
    return gameOver;
}

void TetraminoDisplacement::setSpeed(int newSpeed) {
    dropTimer.setInterval(newSpeed);
    dropTimer.reset();
}
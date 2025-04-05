#include "TetraminoDisplacement.hpp"

TetraminoDisplacement::TetraminoDisplacement(Grid& grid, int speed)
    : grid(grid),
    currentPiece(grid.getWidth() / 2, 0, grid.getWidth(), grid.getHeight()), 
    nextPiece(grid.getWidth() / 2, 0, grid.getWidth(), grid.getHeight()),
    dropTimer(speed), 
    blockCommand(false), lightisBlocked(false){
}

void TetraminoDisplacement::keyInputGameMenu(const std::string& action) {
    if(blockCommand) {
        if(counterMalus2 == 1){
            commandIsBlocked = false;
            blockCommand = false;
            counterMalus2 = -1;
        }
        else
            return;
    }

    std::string newAction = action;
    if(inverseCommand){
        if(counterMalus1 == 3){
            setCommandIsReversed(false);
            ch = "";
            counterMalus1 = -1;
            inverseCommand = false;
        }
        else{
            applyMalus1();
            newAction = ch;
        }
    }

    if (newAction == "right") { 
        moveCurrentPieceRight();
        setNeedToSendGame(true);
    }
    else if (newAction == "left") { 
        moveCurrentPieceLeft();
        setNeedToSendGame(true);
    }
    else if (newAction == "up") { 
        rotateCurrentPiece();
        setNeedToSendGame(true);
    }
    else if (newAction == "down"){
        moveCurrentPieceDown();
        setNeedToSendGame(true);
    }
    else if (newAction == "drop") { // space
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
        if (currentPiece.canMoveDown(grid)) {
            currentPiece.moveDown(grid);
        } else {
            currentPiece.fixToGrid(grid, gameOver);
            if (!gameOver) { 
                dropTimer.decreaseInterval(5); // Diminue le temps d'attente entre chaque chute
                //currentPiece.reset(grid.getWidth() / 2, 0);
                if(commandIsReversed){
                    inverseCommand = true;
                    counterMalus1 += 1;
                } 

                if(commandIsBlocked) {
                    blockCommand = true; 
                    counterMalus2 += 1;
                    
                }
                
                currentPiece = nextPiece; // La pièce actuelle devient la prochaine
                nextPiece.reset(grid.getWidth() / 2, 0); 
            }
        }
        dropTimer.reset();
        if(SpeedBonusMalus){
            if(bonusCounter1.hasElapsed()){
                SpeedBonusMalus = false;
                applySpeedBonusMalus(-speedtest);
                test = true;
                speedtest = 0;
            }
        }
        
        if(lightisBlocked){

            if(bonusCounter1.hasElapsed()){
                setlightBlocked(false);
            }
            //bonusCounter1.reset();
        }
        


    }
}

void TetraminoDisplacement::drawPiece() {
    //if(lightisBlocked) return;
    currentPiece.draw();
}

void TetraminoDisplacement::setBlockCommand(bool block) {
    commandIsBlocked = block;
}
void TetraminoDisplacement::setlightBlocked(bool block){
    //isLight = block;
    bonusCounter1.setInterval(10000);
    bonusCounter1.reset();
    
    lightisBlocked = block;
    grid.setlightBlocked(block);
    currentPiece.setlightBlocked(block);

}
void TetraminoDisplacement::random2x2MaskedBlock(){
    int x = 1 + rand()%(grid.getWidth() - 1);
    int y = grid.heightPieces() + rand()%(grid.getHeight() - grid.heightPieces()-1);
    for(int i = 0; i<2 ;i++){
        for(int j = 0; j<2 ;j++){
            int x_clean = x+i;
            int y_clean = y+j;
            grid.clearCell(x_clean, y_clean);
        }
        
    }
    //grid.applyGravity();

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
    //if (gameOver)
        //std::cout << "Appel de getIsGameOver() -> gameOver = " << gameOver << std::endl;
    return gameOver;
}

void TetraminoDisplacement::setSpeed(int newSpeed) {
    dropTimer.setInterval(newSpeed);
    dropTimer.reset();
}

void TetraminoDisplacement::applySpeedBonusMalus(int speed){
    bonusCounter1.setInterval(10000);
    bonusCounter1.reset();
    int firstSpeed = dropTimer.getInterval();
    dropTimer.setInterval(firstSpeed + speed);
    dropTimer.reset();
    speedtest = speed;
}

void TetraminoDisplacement::applyMalus1(){
    srand(time(0));
    int nbre = rand() % 5;

    switch (nbre) {
        case 0: ch = "up"; break; 
        case 1: ch = "down"; break; 
        case 2: ch = "right"; break; 
        case 3: ch = "left"; break; 
        case 4: ch = "drop"; break;
        default : break;
    }
}
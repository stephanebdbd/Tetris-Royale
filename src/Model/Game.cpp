#include "Game.hpp"

Game::Game(Player* player) : player{player} {
    srand(static_cast<unsigned>(time(nullptr)));
    grid = new Grid();
    currentTetrimino = new Tetrimino(static_cast<TetriminoType>(rand() % 7));
    grid->addTetrimino(currentTetrimino);
    score = 0;
}

void Game::addTetrimino() {
    srand(static_cast<unsigned>(time(nullptr)));
    currentTetrimino = new Tetrimino(static_cast<TetriminoType>(rand() % 7));
    grid->addTetrimino(currentTetrimino);
}

void Game::moveTetrimino(Direction direction, bool downBoost) {
    if (!checkCollision(direction)){
        grid->moveTetrimino(direction);
        checkLines(downBoost);
        setHasMoved();
    }
}

void Game::rotateTetrimino() {
    grid->rotateTetrimino();
    setHasMoved();
}

void Game::checkLines(bool downBoost) {
    int lines = 0;
    grid->checkLines(&lines);
    updateScore(lines, downBoost);
}

bool Game::checkCollision(Direction direction) {
    return grid->checkCollision(direction);
}

void Game::updateScore(int lines, bool downBoost) {
    comboCount += (lines == 0) ? 0 : 1;
    if (downBoost)
        score += 1 + comboCount;
    if (lines==0)
        score += 5 + comboCount;
    else {
        comboBis += 5;
        comboBis *= (lines-1);
        int combos = 25 * (lines-1) + comboBis;
        score += (75+comboCount) * lines + combos;
    }
}

bool Game::isRunning() {
    if (!isStillRunning)
        return false;
    if (grid->isGameOver()){
        isStillRunning = false;
        delete currentTetrimino;
        return false;
    }
    else if (grid->isTetriminoPlaced()){
        delete currentTetrimino;
        addTetrimino();
        setHasMoved();
    }
    return true;
    
}

void Game::display(){
    grid->display();
    std::cout << "Score: " << score << std::endl;
}

bool Game::getHasMoved(){ 
    if (moved){
        moved = false;
        return true;
    }
    return false;
}

void Game::setHasMoved(){
    moved = true;
}

Game::~Game(){
    delete currentTetrimino;
    grid->~Grid();
    std::cout << score << std::endl;
}
#include "Game.hpp"

Game::Game(Player* player) : player{player} {
    srand(static_cast<unsigned>(time(nullptr)));
    grid = new Grid();
    currentTetrimino = new Tetrimino(static_cast<TetriminoType>(rand() % 7));
    grid->addTetrimino(currentTetrimino);
    score = 0;
}

void Game::addTetrimino(bool null) {
    srand(static_cast<unsigned>(time(nullptr)));
    currentTetrimino = nullptr;
    if (!null)
        currentTetrimino = new Tetrimino(static_cast<TetriminoType>(rand() % 7));
    grid->addTetrimino(currentTetrimino);
}

void Game::moveTetrimino(Direction direction, bool downBoost) {
    if (!checkCollision(direction)){
        grid->moveTetrimino(direction);
        if (grid->isTetriminoPlaced()) 
            checkLines(direction, downBoost);
        updateScore(0, downBoost, false, direction);
        //setHasMoved();
    }
    isRunning();
}

void Game::rotateTetrimino() {
    grid->rotateTetrimino();
    //setHasMoved();
}

void Game::checkLines(Direction direction, bool downBoost) {
    int lines = grid->checkLines();
    updateScore(lines, downBoost, true, direction);
}

bool Game::checkCollision(Direction direction) {
    return grid->checkCollision(direction);
}

void Game::updateScore(int lines, bool downBoost, bool tetriminoPlaced, Direction direction) {
    if (direction == Direction::DOWN){
        comboCount += (lines == 0) ? 0 : 1;
        if (downBoost)
            score += 1 + comboCount;
        else
            score += 1;
        if (tetriminoPlaced && lines == 0)
            score += 5 + comboCount;
        else if (tetriminoPlaced && lines > 0){
            comboBis += 5;
            comboBis *= (lines-1);
            int combos = 25 * (lines-1) + comboBis;
            score += (75+comboCount) * lines + combos;
        }
    }
}

bool Game::isRunning() {
    if (!isStillRunning)
        return false;
    if (grid->isTetriminoPlaced()){
        if (grid->isGameOver()){
            addTetrimino(true);
            isStillRunning = false;
            return false;
        }
        addTetrimino();
        //setHasMoved();
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
}
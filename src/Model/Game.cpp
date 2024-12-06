#include "Game.hpp"

Game::Game() {
    grid = new Grid();
    currentTetrimino = new Tetrimino(static_cast<TetriminoType>(rand() % 7)); 
    score = 0;
}

void Game::moveTetrimino(Direction direction) {
    grid->moveTetrimino(direction);
}

void Game::rotateTetrimino() {
    grid->rotateTetrimino();
}

void Game::pushDown(){
    // if (time)
    // moveTetrimino(Direction::DOWN);
    // updateScore(0, true)

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
    if (!stillPlaying) 
        return stillPlaying;
    stillPlaying = checkLines();
    return stillPlaying;
}

bool Game::checkLines() {
    if (checkCollision()) {
        stillPlaying = false;
        return false;
    }
    int lines = 0;
    grid->checkLines(&lines);
    updateScore(lines);
    return true;
}

bool Game::checkCollision() {
    std::vector<Position>* blocks = currentTetrimino->getBlocks();
    for (auto block : *blocks) {
        if (block.y < tetriminoSpace)
            return true;
    }
    return false;
}

void Game::display(){
    grid->display();
}

Game::~Game(){
    delete currentTetrimino;
    grid->~Grid();
    std::cout << score << std::endl;
}
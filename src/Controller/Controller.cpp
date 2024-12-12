#include "Controller.hpp"


Controller::Controller(Game* game)
    : game(game) {}

// Fonction pour traiter les entrées clavier et réagir en conséquence
void Controller::processKeyInput(char keyCode) {
    if (keyCode == LEFT)
        game->moveTetrimino(Direction::LEFT);  
    else if (keyCode == RIGHT)
        game->moveTetrimino(Direction::RIGHT);  
    else if (keyCode == DOWN)
        game->moveTetrimino(Direction::DOWN, true); 
    else if (keyCode == UP)
        game->rotateTetrimino();  
}

Controller::~Controller() {};
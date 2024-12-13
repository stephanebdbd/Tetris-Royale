#include "Controller.hpp"


Controller::Controller(Game* game)
    : game(game) {}

// Fonction pour traiter les entrées clavier et réagir en conséquence
void Controller::processKeyInput(int keyCode) {
    if (keyCode == KEY_LEFT)
        game->moveTetrimino(Direction::LEFT);  
    else if (keyCode == KEY_RIGHT)
        game->moveTetrimino(Direction::RIGHT);  
    else if (keyCode == KEY_DOWN)
        game->moveTetrimino(Direction::DOWN, true); 
    else if (keyCode == KEY_UP)
        game->rotateTetrimino();  
}
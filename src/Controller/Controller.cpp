#include "Controller.hpp"


Controller::Controller(Game* game)
    : game(game) {}


// Fonction pour traiter les entrées clavier et réagir en conséquence
void Controller::processKeyInput(std::string keyCode) {
    if (keyCode == "q")
        game->moveTetrimino(Direction::LEFT);  
    else if (keyCode == "d")
        game->moveTetrimino(Direction::RIGHT);  
    else if (keyCode == "s")
        game->moveTetrimino(Direction::DOWN); 
    else if (keyCode == "z")
        game->rotateTetrimino();  
}

Controller::~Controller() {};
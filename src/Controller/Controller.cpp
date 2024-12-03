#include "Controller.hpp"   


Controller::Controller(Grid* grid)
    : grid_(grid) {}

Controller::~Controller() {
    //pass
}

char Controller::getKeyInput(){
    std::cin >> key;
    while (key != 'q' && key != 's' && key != 'd');
    return key;
}


// Fonction pour traiter les entrées clavier et réagir en conséquence
void Controller::processKeyInput(char keyInput) {
    switch (keyCode) {
        case 'q': 
            grid_->moveTetrimino(Direction::LEFT);  
            break;
        case 'd':
            grid_->moveTetrimino(Direction::RIGHT);  
            break;
        case 's':
            grid_->moveTetrimino(Direction::DOWN);  
            break;
        default:
            break;
    }
}

int mai



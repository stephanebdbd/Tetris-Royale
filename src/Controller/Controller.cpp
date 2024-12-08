#include "Controller.hpp"


Controller::Controller(Game* game)
    : game(game) {}


// Fonction pour traiter les entrées clavier et réagir en conséquence
void Controller::processKeyInput(int keyCode) {
    switch (keyCode) {
        case ALLEGRO_KEY_LEFT: 
            game->moveTetrimino(Direction::LEFT);  
            printf("moved\n");
            break;
        case ALLEGRO_KEY_RIGHT:
            game->moveTetrimino(Direction::RIGHT);  
            printf("moved\n");
            break;
        case ALLEGRO_KEY_DOWN:
            while(keyCode == ALLEGRO_KEY_DOWN)
                game->moveTetrimino(Direction::DOWN, true); 
            printf("moved\n");
            break;
        case ALLEGRO_KEY_UP: 
            game->rotateTetrimino();  
            printf("moved\n");
            break;
        default:
            break;
    }
}

Controller::~Controller() {};
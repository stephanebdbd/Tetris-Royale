#include "Controller.hpp"
#include <cstdlib>  
#include <ctime> 


Controller::Controller(Grid* grid, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_TIMER* timer)
    : grid_(grid), eventQueue_(queue) {}

Controller::~Controller() {};

// Méthode pour gérer les événements (entrées clavier)
void Controller::handleEvents() {
    ALLEGRO_EVENT event;
    while (al_get_next_event(eventQueue_, &event)) {
        switch (event.type) {
            case ALLEGRO_EVENT_KEY_DOWN: {
                processKeyInput(event.keyboard.keycode);
                break;
            }
            
            default:
                break;
        }
    }
}

// Fonction pour traiter les entrées clavier et réagir en conséquence
void Controller::processKeyInput(int keyCode) {
    switch (keyCode) {
        case ALLEGRO_KEY_LEFT: 
            grid_->moveTetrimino(Direction::LEFT);  
            break;
        case ALLEGRO_KEY_RIGHT:
            grid_->moveTetrimino(Direction::RIGHT);  
            break;
        case ALLEGRO_KEY_DOWN:
            grid_->moveTetrimino(Direction::DOWN);  
            break;
        case ALLEGRO_KEY_UP: 
            grid_->rotateTetrimino();  
            break;
        case ALLEGRO_KEY_Q:
            //ici il faut qu'on gere le quitte du jeu apres dans gamer 
            break;
        default:
            break;
    }
}



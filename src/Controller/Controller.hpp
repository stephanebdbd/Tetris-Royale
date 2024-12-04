#pragma once

#include "../include.hpp"
#include "../Model/Grid.hpp"
#include <allegro5/allegro.h>


class Controller {
    Grid* grid_;  // Référence à la grille de jeu
    ALLEGRO_EVENT_QUEUE* eventQueue_; 
    

public:
    Controller(Grid* grid, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_TIMER* timer);            //constructeur
    void processKeyInput(int keyCode);                                                   // Traite les entrées clavier
    void handleEvents();
    ~Controller()                                                            = default;  // Destructeur
};
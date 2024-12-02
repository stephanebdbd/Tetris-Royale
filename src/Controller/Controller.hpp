#pragma once

#include "../include.hpp"
#include "../Model/Grid.hpp"
#include "../Model/Tetrimino.hpp"


class Controller {
    Grid* grid_;  // Référence à la grille de jeu
    ALLEGRO_EVENT_QUEUE* eventQueue_; 
    void processKeyInput(int keyCode);  // Traite les entrées clavier

public:
    Controller(Grid* grid, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_TIMER* timer);
    void handleEvents();
    ~Controller();  // Destructeur
};

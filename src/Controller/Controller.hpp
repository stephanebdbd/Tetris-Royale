#pragma once

#include "../include.hpp"
#include "../Model/Game.hpp"


class Controller {
    Game* game;  // Référence à la grille de jeu
public:
    Controller(Game* game);            //constructeur
    void processKeyInput(int keyCode);
    void* keyboardListener(void* arg);
    ~Controller()=default;  // Destructeur
};
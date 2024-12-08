#pragma once

#include "../include.hpp"
#include "../Model/Game.hpp"


class Controller {
    Game* game;  // Référence à la grille de jeu
public:
    Controller(Game* game);            //constructeur
    void handleEvents();
    void processKeyInput(int keyCode);                                                   // Traite les entrées clavier
    void MouseMove(Position position);                                                  // Traite les entrées souris
    void MouseClick(Position position);
    void must_init(bool test, const char *description);
    void must_init(void *test, const char *description);
    ~Controller();  // Destructeur
};
#pragma once

#include "../include.hpp"
#include "../Model/Game.hpp"


class Controller {
    Game* game;  // Référence à la grille de jeu
public:
    Controller(Game* game);            //constructeur
    void handleEvents();
    void processKeyInput(std::string keyCode);                                                   // Traite les entrées clavier
    //void MouseMove(Position position);                                                  // Traite les entrées souris
    //void MouseClick(Position position);
    ~Controller();  // Destructeur
};
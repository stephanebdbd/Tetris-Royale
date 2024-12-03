#pragma once

#include "../include.hpp"
#include "../Model/Grid.hpp"
#include "../Model/Tetrimino.hpp"


class Controller {
    Grid* grid_;  // Référence à la grille de jeu
    
public:
    Controller(Grid* grid);
    char *getKeyInput();// Récupère les entrées clavier
    void processKeyInput(const char* keyInput);  // Traite les entrées clavier
    ~Controller();  // Destructeur
};

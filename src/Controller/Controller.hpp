#pragma once

#include "../include.hpp"
#include "../Model/Game.hpp"


class Controller {
    Game* game;  // Référence à la grille de jeu
    ALLEGRO_EVENT event;
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *queue;
    int downCounter=0;
public:
    Controller(Game* game);            //constructeur
    void handleEvents();
    void processKeyInput(int keyCode);                                                   // Traite les entrées clavier
    void updateKeyState();
    void MouseMove(Position position);                                                  // Traite les entrées souris
    void MouseClick(Position position);
    void must_init(bool test, const char *description);
    void must_init(void *test, const char *description);
    ~Controller();  // Destructeur
};
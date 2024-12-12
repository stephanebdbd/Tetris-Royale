/*#include "include.hpp"
#include "Model/Game.hpp"
#include "Controller/Controller.hpp"
#include "Model/User.hpp"
#include "View/Board.hpp"

void clear() {
    if (std::system("clear") != 0)
        perror("std::system(\"clear\")");
}

int main(int , char ** ) {
    clear();

    std::string username, password;
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;

    Player player{username, password};
    Game game{&player};
    Controller controller{&game};
    PlayerBoard playerBoard{&game};

    clear();


    std::string input;

    while (game.isRunning()) {
        playerBoard.display();
        std::cout << "Enter a key: ";
        std::cin >> input;
        controller.processKeyInput(input);
    }

    playerBoard.display();
    std::cout << "Game Over!" << std::endl;

    return 0;
}*/
#include "include.hpp"
#include "Model/Game.hpp"
#include "Controller/Controller.hpp"
#include "Model/User.hpp"
#include "View/Board.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>

void clear() {
    if (std::system("clear") != 0)
        perror("std::system(\"clear\")");
}

void must_init(bool test, const char *description) {
    if (test) return;
    std::cerr << "couldn't initialize " << description << '\n';
    exit(1);
}

int main() {
    clear();
    if (!al_init()) {
        std::cerr << "Erreur d'initialisation d'Allegro." << std::endl;
        return -1;
    }

    must_init(al_install_keyboard(), "keyboard");
    if (!al_install_keyboard()) {
        std::cerr << "Erreur : le clavier n'a pas pu être initialisé." << std::endl;
        return -1;
    }

    must_init(al_init_primitives_addon(), "primitives");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 2); // Timer pour descendre les Tetriminos
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    must_init(timer, "timer");
    must_init(event_queue, "event_queue");

    al_start_timer(timer);
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    std::string username, password;
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;

    Player player{username, password};
    Game game{&player};
    Controller controller{&game};
    PlayerBoard playerBoard{&game};

    // Bouce principale du jeu
    while (game.isRunning()) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
        //le probleme maintenant ca vient d'ici c'est que quand tu appuyeras sur une touche 
        //ca rentfre pas dans cette conditon la du coup c'est elle qui gere les touches 
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            std::cout<<"touche presse "<<std::endl;

            if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                game.moveTetrimino(Direction::LEFT);
            } else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                game.moveTetrimino(Direction::RIGHT);
            } else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {

                game.moveTetrimino(Direction::DOWN);
            
            }
        //ici elle fonctionne tres bien , tu peux commenter down , et decommenter right ou bien left pour voir comment ca fonctionne 
        }
        if (ev.type == ALLEGRO_EVENT_TIMER) {
            // Descente automatique des Tetriminos 
            game.moveTetrimino(Direction::DOWN);
            //game.moveTetrimino(Direction::LEFT);
            //game.moveTetrimino(Direction::RIGHT);

            playerBoard.display();
            
        }
    }

    playerBoard.display();
    std::cout << "Game Over!" << std::endl;

    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);

    return 0;
}

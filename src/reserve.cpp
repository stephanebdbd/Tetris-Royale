#include "include.hpp"
#include "Model/Game.hpp"
#include "Controller/Controller.hpp"
#include "Model/User.hpp"
#include "View/Board.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>


void clear() {
    if (std::system("clear") != 0) {
        perror("std::system(\"clear\")");
    }
}


void must_init(bool test, const char *description) {
    if (!test) {
        std::cerr << "Couldn't initialize " << description << '\n';
        exit(EXIT_FAILURE);
    }
}


void must_init(void *test, const char *description) {
    must_init(test != nullptr, description);
}


int main(int /* argc */, char ** /* argv */) {
    srand(static_cast<unsigned>(time(nullptr)));

    // Initialisation Allegro
    must_init(al_init(), "Allegro");
    must_init(al_install_keyboard(), "Keyboard");

    // Création du timer et de la file d'événements
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "Timer");
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    must_init(queue, "Event Queue");

    // Enregistrement des sources d'événements
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    // Demander les informations utilisateur
    std::string username, password;
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;

    // Initialisation du jeu
    Player player{username, password};
    Game game(&player);
    Controller controller(&game);
    PlayerBoard playerBoard(&game);

    // Démarrage du timer
    al_start_timer(timer);

    if (game.isRunning()) {std::cout << "Game is running" << std::endl;}

    while (game.isRunning()){
      ALLEGRO_EVENT event;

      // Attente et gestion des événements
      al_wait_for_event(queue, &event);

      switch (event.type) {
          case ALLEGRO_EVENT_KEY_DOWN:
              std::cout << "yes" << std::endl;
              controller.processKeyInput(event.keyboard.keycode);
              //playerBoard.display();
              break;
            
          //case ALLEGRO_EVENT_TIMER: 
              // Mise à jour de l'affichage
              //playerBoard.display();
              //break;

          default:
              break;
        }
    }

    // Nettoyage des ressources Allegro
    al_destroy_event_queue(queue);
    al_destroy_timer(timer);

    return 0;
}

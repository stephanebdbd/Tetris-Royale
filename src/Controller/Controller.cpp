#include "Controller.hpp"


Controller::Controller(Game* game)
    : game(game) {
    srand(static_cast<unsigned>(time(nullptr)));

    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_install_mouse(), "mouse");

    timer = al_create_timer(1.0 / 60.0);
    queue = al_create_event_queue();

    must_init(timer, "timer");
    must_init(queue, "queue");

    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    al_start_timer(timer);
}

// Méthode pour gérer les événements (entrées clavier)
void Controller::handleEvents() {
    while (game->isRunning()) {
        al_wait_for_event(queue, nullptr);
        while (al_get_next_event(queue, &event)) {
            switch (event.type) {
                case ALLEGRO_EVENT_KEY_DOWN:
                    processKeyInput(event.keyboard.keycode);
                    break;
                /*case ALLEGRO_EVENT_MOUSE_AXES:
                    //mouseMove({static_cast<float>(event.mouse.x),
                    //                  static_cast<float>(event.mouse.y)});
                    break;
                case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                    //mouseClick({static_cast<float>(event.mouse.x),
                    //                   static_cast<float>(event.mouse.y)});
                    break;*/
                default: 
                    downCounter++;
                    if (downCounter % 45 == 0)
                        game->moveTetrimino(Direction::DOWN);
                    break;
            }
        }
    }
}

// Fonction pour traiter les entrées clavier et réagir en conséquence
void Controller::processKeyInput(int keyCode) {
    switch (keyCode) {
        case ALLEGRO_KEY_LEFT: 
            game->moveTetrimino(Direction::LEFT);  
            break;
        case ALLEGRO_KEY_RIGHT:
            game->moveTetrimino(Direction::RIGHT);  
            break;
        case ALLEGRO_KEY_DOWN:
            while(keyCode == ALLEGRO_KEY_DOWN)
                game->pushDown(); 
            break;
        case ALLEGRO_KEY_UP: 
            game->rotateTetrimino();  
            break;
        default:
            break;
    }
}

void Controller::must_init(bool test, const char *description) {
    if (test) {
        return;
    }

    std::cerr << "couldn't initialize " << description << '\n';
    exit(1);
}

void Controller::must_init(void *test, const char *description) {
    must_init(test != nullptr, description);
}

Controller::~Controller() {
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
};
#include "include.hpp"
#include "Model/Game.hpp"
#include "Controller/Controller.hpp"
#include "Model/User.hpp"
#include "View/Board.hpp"

void clear() {
    if (std::system("clear") != 0)
        perror("std::system(\"clear\")");
}

void must_init(bool test, const char *description) {
    if (test) {
        return;
    }

    std::cerr << "couldn't initialize " << description << '\n';
    exit(1);
}

void must_init(void *test, const char *description) {
    must_init(test != nullptr, description);
}

int main(int /* argc */, char ** /* argv */) {
  clear();
  
  srand(static_cast<unsigned>(time(nullptr)));

  must_init(al_init(), "allegro");
  ALLEGRO_EVENT event;
  ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
  must_init(timer, "timer");
  ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
  must_init(queue, "queue");
  int downCounter = 0;

  must_init(al_install_keyboard(), "keyboard");

  al_register_event_source(queue, al_get_timer_event_source(timer));
  al_register_event_source(queue, al_get_keyboard_event_source());

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

  al_start_timer(timer);

  playerBoard.display();

  while (game.isRunning()) {
    al_wait_for_event(queue, nullptr);
    while (al_get_next_event(queue, &event)){
        switch (event.type) {
            case ALLEGRO_EVENT_KEY_DOWN:{
                controller.processKeyInput(event.keyboard.keycode);
                if (game.getHasMoved())
                    playerBoard.display();
                break;
              }
            /*case ALLEGRO_EVENT_MOUSE_AXES:
                //mouseMove({static_cast<float>(event.mouse.x),
                //                  static_cast<float>(event.mouse.y)});
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                //mouseClick({static_cast<float>(event.mouse.x),
                //                   static_cast<float>(event.mouse.y)});
                break;*/
            default: {
                downCounter++;
                if (downCounter % 45 == 0){
                    game.moveTetrimino(Direction::DOWN);
                    playerBoard.display();
                }
                break;
              }
            }
        }
  }
  return 0;
}

/*

*/
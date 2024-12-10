#include "include.hpp"
#include "Model/Game.hpp"
#include "Controller/Controller.hpp"
#include "Model/User.hpp"
#include "View/Board.hpp"

void clear() {
    if (std::system("clear") != 0)
        perror("std::system(\"clear\")");
}

int main(int /* argc */, char ** /* argv */) {
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
        /*if (game.getHasMoved())*/ playerBoard.display();
        std::cout << "Enter a key: ";
        std::cin >> input;
        controller.processKeyInput(input);
    }

    playerBoard.display();
    std::cout << "Game Over!" << std::endl;

    return 0;
}
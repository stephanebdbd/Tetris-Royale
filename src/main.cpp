#include "Controller/Controller.hpp"
#include "Model/Game.hpp"
#include "View/Board.hpp"
#include "Model/utils.hpp"
#include "include.hpp"

void setTerminalMode(bool enable) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    if (enable) {
        term.c_lflag |= (ICANON | ECHO); // Activer mode canonique et écho
    } else {
        term.c_lflag &= ~(ICANON | ECHO); // Désactiver mode canonique et écho
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

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

    bool usernameDone = true, passwordDone = true;
    std::string username, password; char c;

    setTerminalMode(false);
    
    std::cout << "Username: ";
    c = std::getchar();
    while (usernameDone) {
        username += c;
        std::cout << c;
        c = std::getchar();
        usernameDone = (c != '\n');
    }
    std::cout << std::endl;

    std::cout << "Password: ";
    c = std::getchar();
    while (passwordDone) {
        password += c;
        std::cout << "*";
        c = std::getchar();
        passwordDone = (c != '\n');
    }
    std::cout << std::endl;


    Player player{username, password};
    Game game{&player};
    Controller controller{&game};
    PlayerBoard playerBoard{&game};
    int counter = 0;

    // Bouce principale du jeu
    while (game.isRunning()) { 
        if (counter % 30 == 0){
            game.moveTetrimino(Direction::DOWN);
        }
        else if ((c = std::getchar()) != EOF) {
            if (c == MOVE) {
                if (std::getchar() == '['){
                    c = std::getchar();
                    controller.processKeyInput(c);
                }
            }
            else if (c == ESC)
                game.setIsRunning(false);
        }
        if (game.getHasMoved())
            playerBoard.display();
        counter++;
    }

    if (game.getHasMoved()) playerBoard.display();
    std::cout << "Game Over!" << std::endl;

    setTerminalMode(true);

    std::cout << username << " " << password << std::endl;

    return 0;
}
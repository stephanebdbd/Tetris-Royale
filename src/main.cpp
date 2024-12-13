#include "Controller/Controller.hpp"
#include "Model/Game.hpp"
#include "View/Board.hpp"
#include "Model/utils.hpp"
#include "include.hpp"

void clearScreen() {
    if (std::system("clear") != 0)
        perror("std::system(\"clear\")");
}

bool getCanGoDown(std::chrono::time_point<std::chrono::system_clock> start){
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return (duration.count() % 1000) == 0;
}

void getUserData(std::string &str, bool &b, int &buffer, int i) {
    while (b) {
        buffer = getch();
        if (buffer == ERR) {
            perror("getch");
            return;
        }
        if (buffer == ERASE && (str.size() > 0)) {
            str.pop_back();
            printw("\b \b");
        }
        else if (isascii(buffer) && (buffer != '\n') && (buffer != ERASE)) {
            str += buffer;
            if (i == 0) printw("%d", buffer);
            else printw("*");
        }
        b = buffer != '\n';
        if (!b && str.size() == 0) b = true;
    }
}

int main() {
    struct boolNString {
        bool b = true;
        std::string s = "";
    };

    initscr();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    cbreak();
    noecho();

    enableColors();

    boolNString userData[2]; int buffer, i = 0;

    for (auto &u:userData){
        if (i == 0) printw("Username: ");
        else printw("\nPassword: ");
        getUserData(u.s, u.b, buffer, i);
    }


    Player player{userData[0].s, userData[1].s};
    Game game{&player};
    Controller controller{&game};
    PlayerBoard playerBoard{&game};

    auto start = std::chrono::system_clock::now();

    playerBoard.display();

    // Bouce principale du jeu
    while (game.isRunning()) {
        while (getCanGoDown(start)) {
            buffer = getch();
            if (buffer == ERR) {
                perror("getch");
                game.setIsRunning(false);
                playerBoard.display();
                return 1;
            }
            if (buffer == ESC)
                game.setIsRunning(false);
            else
             controller.processKeyInput(buffer);
        }
        game.moveTetrimino(Direction::DOWN);
        if (game.getHasMoved())
            playerBoard.display();
        refresh();
    }

    if (game.getHasMoved()) playerBoard.display();
    printw("Game Over!\n");

    endwin();

    return 0;
}
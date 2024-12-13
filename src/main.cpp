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
        if (buffer == KEY_BACKSPACE && (str.size() > 0)) {
            str.pop_back();
            printw("\b \b");
            refresh();
        }
        else if (isascii(buffer) && (buffer != '\n') && (buffer != KEY_BACKSPACE)) {
            str += static_cast<char>(buffer);
            if (i == 0) printw("%c", buffer);
            else printw("*");
            refresh();
        }
        b = (buffer != '\n');
        if (str.size() == 0 && !b) b = true;
    }
}

int main() {
    struct boolNString {
        bool b = true;
        std::string s = "";
    };

    clearScreen();

    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();

    enableColors();

    boolNString userData[2]; int buffer, i = 0;

    for (auto &u:userData){
        if (i == 0) printw("Username: ");
        else printw("\nPassword: ");
        refresh();
        getUserData(u.s, u.b, buffer, i);
        i++;
    }

    nodelay(stdscr, TRUE);

    Player player{userData[0].s, userData[1].s};
    Game game{&player};
    Controller controller{&game};
    PlayerBoard playerBoard{&game};

    auto start = std::chrono::system_clock::now();

    playerBoard.display();
    refresh();
    buffer = 0;
    
    // Bouce principale du jeu
    while (game.isRunning()) {
        buffer = getch();
        if (buffer == ESC)
            game.setIsRunning(false);
        else if (!getCanGoDown(start))
            controller.processKeyInput(buffer);
        else
            game.moveTetrimino(Direction::DOWN);
        if (game.getHasMoved())
            playerBoard.display();
        refresh();
    }

    if (game.getHasMoved()) playerBoard.display();
    printw("\nGame Over!\n");
    refresh();
    getch();
    napms(1000);
    endwin();

    return 0;
}
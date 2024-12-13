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

void getUserData(std::string &str, bool &b, int &buffer) {
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
            printw("%c", buffer);
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


    //clearScreen();
    initscr();
    if (stdscr == nullptr) {
        std::cerr << "Erreur d'initialisation de ncurses." << std::endl;
        return 1;
    }
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    if (setlocale(LC_ALL, "en_US.UTF-8") == nullptr) {
        std::cerr << "Erreur de configuration de la locale." << std::endl;
        endwin();
        return 1;
    }
    
    // Vérifier si le terminal supporte UTF-8
    if (!has_colors()) {
        printw("Le terminal ne supporte pas les couleurs.");
        refresh();
        getch();
        endwin();
        return 1;
    }

    start_color();
    
    printw("Emoji 🟫 affiché avec printw.\n");
    printw("blabla");
    refresh();
    getch();
    endwin();
    /*
    boolNString userData[2]; int buffer, i = 0;

    
    for (auto &u:userData){
        if (i == 0) printw("Username: ");
        else printw("\nPassword: ");
        getUserData(u.s, u.b, buffer);
    }


    Player player{userData[0].s, userData[1].s};
    Game game{&player};
    Controller controller{&game};
    PlayerBoard playerBoard{&game};

    auto start = std::chrono::system_clock::now();

    //playerBoard.display();

    // Bouce principale du jeu
    while (game.isRunning()) {
        if (getCanGoDown(start))
            game.moveTetrimino(Direction::DOWN);
        else {
            buffer = getch();
            if (buffer == ERR) {
                perror("getch");
                game.setIsRunning(false);
                //playerBoard.display();
                return 1;
            }
            if (buffer == ESC)
                game.setIsRunning(false);
            else
             controller.processKeyInput(buffer);
        }
        //if (game.getHasMoved())
            //playerBoard.display();
    }

    //if (game.getHasMoved()) playerBoard.display();
    printw("Game Over!\n");

    */
    //endwin();

    return 0;
}
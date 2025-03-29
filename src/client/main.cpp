#include "Client.hpp"
#include "SFMLGame.hpp"
#include <ncurses.h>
#include <iostream>
#include <fstream>

void lancerModeTerminal(Client& client) {
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    client.run();

    endwin();
}

void lancerModeGraphique(Client& client) {
    SFMLGame game(client);
    game.run();
}

int main() {
    std::ofstream serverLog("client.log");
    std::cout.rdbuf(serverLog.rdbuf());
    std::cerr.rdbuf(serverLog.rdbuf());

    int choix;
    std::cin >> choix;

    Client client("127.0.0.1", 12345);

    if (choix == 1) {
        lancerModeTerminal(client);
    } else if (choix == 2) {
        lancerModeGraphique(client);
    } else {
        std::cout << "Choix invalide.\n";
    }

    return 0;
}

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

    int choix;
    std::cout << "Choisissez le mode d'affichage :\n";
    std::cout << "      1. Mode Terminal\n";
    std::cout << "      2. Mode Graphique\n";
    std::cout << "Entrez votre choix (1 ou 2) : ";
    std::cin >> choix;

    auto client = std::make_unique<Client>("127.0.0.1", 12345);

    if (choix == 1) {
        std::ofstream serverLog("client.log");
        std::cout.rdbuf(serverLog.rdbuf());
        std::cerr.rdbuf(serverLog.rdbuf());
        
        lancerModeTerminal(*client);

    } else if (choix == 2) {

        lancerModeGraphique(*client);
        
    } else {
        std::cout << "Choix invalide.\n";
    }

    return 0;
}

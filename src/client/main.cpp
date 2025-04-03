#include "Client.hpp"
#include <ncurses.h>
#include <iostream>
#include <fstream>

int main() {

    std::ofstream clientLog("client.log"); // Créer un fichier de log
    // Rediriger std::cout et std::cerr vers le fichier log
    std::cout.rdbuf(clientLog.rdbuf());
    std::cerr.rdbuf(clientLog.rdbuf());

    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); 
    
    Client client("127.0.0.1", 12345);

    client.run();
    
    delwin(stdscr);
    endwin();

    return 0;
}

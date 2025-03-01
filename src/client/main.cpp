#include "Client.hpp"
#include <ncurses.h>
#include <iostream>
#include <fstream>

int main() {

    std::ofstream serverLog("client.log"); // Créer un fichier de log
    // Rediriger std::cout et std::cerr vers le fichier log
    std::cout.rdbuf(serverLog.rdbuf());
    std::cerr.rdbuf(serverLog.rdbuf());

    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); 
    
    Client client("127.0.0.1", 12345);

    client.run();
    
    return 0;
}

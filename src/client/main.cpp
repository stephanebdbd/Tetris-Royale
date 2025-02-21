#include "Client.hpp"
#include <ncurses.h>

int main() {
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); 
    
    Client client("127.0.0.1", 12345);

    client.run();
    
    return 0;
}

#include "Board.hpp"


void clearScreen() {
    std::system("clear");
}

PlayerBoard::PlayerBoard(Game *game) : game_(game){};

void PlayerBoard::display(){
    clearScreen();
    game_.display();
}

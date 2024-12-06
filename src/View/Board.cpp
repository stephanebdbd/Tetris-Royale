#include "Board.hpp"
#include <cstdlib>

using namespace std;

void clearScreen() {
    system("clear");
}

PlayerBoard::PlayerBoard(Game *game) : game_(game){};

void PlayerBoard(){
    clearscreen();
    game_.dispaly();
}

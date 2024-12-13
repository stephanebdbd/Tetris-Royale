#include "Board.hpp"


PlayerBoard::PlayerBoard(Game *game) : game_(game){};

void PlayerBoard::clear() {
    ::clear();
}

void PlayerBoard::display(){
    clear();
    printw("%sTetris Royale\n", space);
    game_->display();
    refresh();
}
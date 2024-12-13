#include "Board.hpp"


PlayerBoard::PlayerBoard(Game *game) : game_(game){};

void PlayerBoard::clear() {
    if (std::system("clear") != 0)
        perror("std::system(\"clear\")");
}

void PlayerBoard::display(){
    clear();
    printw("%sTetris Royale\n", space);
    game_->display();
}

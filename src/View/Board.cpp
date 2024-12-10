#include "Board.hpp"


PlayerBoard::PlayerBoard(Game *game) : game_(game){};

void PlayerBoard::clear() {
    if (std::system("clear") != 0)
        perror("std::system(\"clear\")");
}

void PlayerBoard::display(){
    clear();
    std::cout << space << "Tetris Royale" << std::endl;
    game_->display();
}

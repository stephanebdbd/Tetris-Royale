#include "Bonus.hpp"

Bonus::Bonus(Game& game) : game(game){}

void Bonus::MiniBlock(){
    std::array<std::array<char, 4>, 4> miniBlock = {{' ', ' ', ' ', ' '},
                                                    {' ', '#', ' ', ' '},
                                                    {' ', ' ', ' ', ' '},
                                                    {' ', ' ', ' ', ' '}};

    game.setcurrentshape(miniBlock);
}

void Bonus::decreaseSpeed(){
    game.setSpeed(10);
}
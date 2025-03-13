#include "Game.hpp"


int main() {
    Score score(15, 2);
    Game game(10, 20, score);
    game.run();
    return 0;
}
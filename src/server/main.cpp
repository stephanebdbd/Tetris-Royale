#include <vector>
#include <ncurses.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <array>
#include <iostream>


#include "Game.hpp"


int main() {
    Game game(10, 20);
    game.run();
    return 0;
}
#include "malus.hpp"
#include <iostream>
#include <cstdlib>  // Pour rand() et srand()
#include <ctime> // pour time()
#include <ncurses.h>
#include <Grid.hpp>

using namespace std ;
Malus::Malus(int nbrLine): nbrLine(nbrLine) { line = std::vector<char>(10, '#');}

void Malus:: createMalus(){
    std::srand(std::time(0));
    int nbrbloc = std::rand() % 10;
    line[nbrbloc] = ' ';
    for(int i = 0 ; i < nbrLine  ; i++){
        malus.push_back(line);
    }
}


void Malus:: sendMalus(Grid &grid){
    initscr(); 
    start_color();
    use_default_colors();
    init_pair(20, 244, -1);

    for(int y = 0; y < nbrLine; ++y){
        for(int x = 0; x < 10; ++x){
            if (malus[y][x] =! ' '){
                grid.markCell(x, y, malus[y][x], 20)
            }
        }
    }


    /*initscr(); 
    start_color();
    use_default_colors();
    //init_color(COLOR_BLACK, 500, 500, 500);
    init_pair(1, 244, -1);
    attron(COLOR_PAIR(1));
    std::vector<std::vector<char> > f = getMalus();
    for (const auto& ligne : f) { // Parcourt chaque ligne
        for (char c : ligne) {
            printw("%c ", c);
        }
        printw("\n"); 
    }
    //attroff (COLOR_PAIR(1));
    refresh();*/
}



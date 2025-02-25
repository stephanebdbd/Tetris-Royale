#include "malus.hpp"
#include <iostream>
#include <cstdlib>  // Pour rand() et srand()
#include <ctime> // pour time()
#include <ncurses.h>

using namespace std ;
Malus::Malus(int nbrLine): nbrLine(nbrLine) {
    line = std::vector<char>(10, '#');
    createMalus();
}

void Malus:: createMalus(){
    std::srand(std::time(0));
    int nbrbloc = std::rand() % 10;
    line[nbrbloc] = ' ';
    for(int i = 0 ; i < nbrLine  ; i++){
        malus.push_back(line);
    }
}


void Malus:: sendMalus(Grid &grid){
    
    int height = grid.getHeight();
    int width = grid.getWidth();
    grid.piecesUp(nbrLine);

    for(int y = 0; y < nbrLine ; y++){
        for(int x = 0; x < 10; x++){
            
            if (malus[y][x] != ' '){
                
                grid.markCell(x+1, (height - 1) - y, 9);
            }
        }
    }

}

void Malus::reverseControl(){
    
}



#include "ClassicMode.hpp"

ClassicMode::ClassicMode(){}

void ClassicMode::featureMode(Game& game){

    int linesCleared = game.getLinesCleared();
    int nbrMalus = getNbrMalus(linesCleared); // nombre du malus à envoyer

    if(nbrMalus > 0){
        Malus malus(nbrMalus);
        malus.sendMalus(game.getGrid());
    }
}


int ClassicMode::getNbrMalus(int nbrLineComplet) const{
    switch(nbrLineComplet){
        case 2 : return 1; break;
        case 3 : return 2; break;
        case 4 : return 4; break;
        default : return 0; break;
        
    }
}
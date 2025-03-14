#include "ClassicMode.hpp"

ClassicMode::ClassicMode(){}

void ClassicMode::featureMode(std::shared_ptr<Game> game, int malusOrBonus){
    int linesCleared = getNbrMalus(malusOrBonus); // nombre du malus à envoyer
    if(linesCleared > 0){
        Malus malus(linesCleared);
        malus.sendMalus(game->getGrid());
    }
}


int ClassicMode::getNbrMalus(int linesCleared) const{
    switch(linesCleared){
        case 2 :
            return 1;
        case 3 :
            return 2;
        case 4 :
            return 4;
        default :
            return 0;
    }
}
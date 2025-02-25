#include "DuelMode.hpp"
#include "Malus.hpp"

DuelMode::DuelMode(){}

void DuelMode::useMalus(int nbrMalus){
    Malus malus(nbrMalus);
    
    Grid& grid = playerGrids[chosenPlayer];
    malus.sendMalus(grid);
}
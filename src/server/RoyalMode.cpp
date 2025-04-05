#include "RoyalMode.hpp"
#include "MalusRoyal.hpp"
#include "Bonus.hpp"

void RoyalMode::featureMode(std::shared_ptr<Game> game, int malusOrBonus){
    if (malusOrBonus < 6) {
        MalusRoyal malus(game);
        choiceMalus(malusOrBonus, malus);
    }
    else {
        Bonus bonus(game);
        choiceBonus(malusOrBonus, bonus);
        
    }
    
}

void RoyalMode::choiceMalus(int nbre, MalusRoyal malus){
    switch(nbre){
        case 1:
            malus.reverseControl();
            break;
        case 2:
            malus.blockControl();
            break;
        case 3:
            malus.increaseSpeed();
            break;
        case 4:
            malus.clear2x2Block(); 
            break;
        case 5:
            malus.turnOffLight();
            break;
        default: 
            break;
    }

}

void RoyalMode::choiceBonus(int nbre, Bonus bonus){
    switch(nbre){
        case 6:
            bonus.decreaseSpeed(); 
            break;
        case 7:
            bonus.miniBlock();
            break;
        
        default:
            break;
    }
}
#include "RoyalMode.hpp"
#include "MalusRoyal.hpp"
#include "Bonus.hpp"

RoyalMode::RoyalMode() : energie(0){}

void RoyalMode::feautureMode(Game& game){

    MalusRoyal malus(game.getDisplacement());
    Bonus bonus(game.getDisplacement());

    int linesCleared = game.getLinesCleared();
    energie += 20 * linesCleared;
    if(energie >= 100){
        if(chosenMalus){
            choiceMalus(nbre);
        }

        if(chosenBonus){
            choiceBonus(nbre);
        }

    }

    applyMalusBonus();
    
}

void RoyalMode::choiceMalus(int nbre){
    switch(nbre){
        case 1: {
            decreaseEnergie(60);
            malus1counter = 3;
            break;
        }
        case 2: {
            malus.blockControl();
            decreaseEnergie(50);
            break;
        }
        case 3: {
            malus.increaseSpeed();
            decreaseEnergie(30);
            break;
        }
        case 4: {
            malus.sendLight(); 
            decreaseEnergie(40);
            break;
        }
        case 5: { 
            malus.turnOffLight();
            decreaseEnergie(70);
            break;
        }
        default: break;
    }
    setMalus(false);

}

void RoyalMode::choiceBonus(int nbre){
    switch(nbre){
        case 1: {
            bonus.decreaseSpeed(); 
            decreaseEnergie(30);
            break;
        }
        case 2: {
            decreaseEnergie(35);
            bonus2counter = 2;
            break;
            
        }
        default: break;
    }
    setBonus(false);

}

void RoyalMode::applyMalusBonus(){
    if (malus1counter > 0){
        malus.reverseControl();
        decreaseCounter(malus1counter);
    }

    if(bonus2counter > 0){
        bonus.MiniBlock();
        decreaseCounter(bonus2counter);
    }
}



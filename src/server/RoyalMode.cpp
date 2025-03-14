#include "RoyalMode.hpp"
#include "MalusRoyal.hpp"
#include "Bonus.hpp"

RoyalMode::RoyalMode() : energie(0){}

void RoyalMode::featureMode(std::shared_ptr<Game> game, int malusOrBonus){

    std::cout << "Malus or Bonus : " << malusOrBonus << std::endl;
    
    MalusRoyal malus(game->getDisplacement());
    Bonus bonus(game->getDisplacement());

    int linesCleared = game->getLinesCleared();
    energie += 20 * linesCleared;
    if(energie >= 100){
        if(chosenMalus){
            choiceMalus(nbre, malus);
        }

        if(chosenBonus){
            choiceBonus(nbre, bonus);
        }

    }

    applyMalusBonus(malus, bonus);
    
}

void RoyalMode::choiceMalus(int nbre, MalusRoyal malus){
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
            malus.clear2x2Block(); 
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

void RoyalMode::choiceBonus(int nbre, Bonus bonus){
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

void RoyalMode::applyMalusBonus(MalusRoyal malus, Bonus bonus){
    if (malus1counter > 0){
        malus.reverseControl();
        decreaseCounter(malus1counter);
    }

    if(bonus2counter > 0){
        bonus.MiniBlock();
        decreaseCounter(bonus2counter);
    }
}



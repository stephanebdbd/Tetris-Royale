#ifndef ROYALMODE_HPP
#define ROYALMODE_HPP

#include <iostream>
#include "GameMode.hpp"
#include "Game.hpp"


class RoyalMode : public GameMode{
    
    int energie;
    bool chosenMalus = false;
    bool chosenBonus = false;
    int nbre = 10;
    int malus1counter = 0;
    int bonus2counter = 0;

    public:
        RoyalMode();
        void feautureMode() override;
        void choiceMalus(int nbre);
        void choiceBonus(int nbre);
        void setMalus(bool malus){chosenMalus = malus;};
        void setBonus(bool bonus){chosenBonus = bonus;};
        void setNbre(int nbre){this->nbre = nbre;};
        void decreaseEnergie(int amount){energie -= amount;};
        void decreaseCounter(int counter){counter--;};
        
};

#endif
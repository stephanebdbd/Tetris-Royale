#ifndef ROYALMODE_HPP
#define ROYALMODE_HPP

#include <iostream>
#include "GameMode.hpp"
#include "MalusRoyal.hpp"
#include "Bonus.hpp"
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
        void applyMalusBonus(MalusRoyal malus, Bonus bonus);
        void featureMode(Game& game) override;
        void choiceMalus(int nbre, MalusRoyal malus);
        void choiceBonus(int nbre, Bonus bonus);
        void setMalus(bool malus){chosenMalus = malus;}
        void setBonus(bool bonus){chosenBonus = bonus;}
        void setNbre(int nbre){this->nbre = nbre;}
        void decreaseEnergie(int amount){energie -= amount;}
        void decreaseCounter(int counter){counter--;}
        GameModeName getNameMode() override { return GameModeName::Royal_Competition; }
};

#endif
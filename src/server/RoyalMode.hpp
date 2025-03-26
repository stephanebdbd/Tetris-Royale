#ifndef ROYALMODE_HPP
#define ROYALMODE_HPP

#include <iostream>
#include "GameMode.hpp"
#include "MalusRoyal.hpp"
#include "Bonus.hpp"
#include "Game.hpp"


class RoyalMode : public GameMode{
    public:
        RoyalMode()=default;
        void featureMode(std::shared_ptr<Game> game, int malusOrBonus) override;
        void choiceMalus(int nbre, MalusRoyal malus);
        void choiceBonus(int nbre, Bonus bonus);
        GameModeName getNameMode() override {return GameModeName::Royal_Competition;}
};

#endif
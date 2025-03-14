#ifndef CLASSICMODE_HPP
#define CLASSICMODE_HPP

#include <iostream>
#include "GameMode.hpp"
#include "Game.hpp"
#include "malus.hpp"



class ClassicMode : public GameMode{

    public:
        ClassicMode();
        void featureMode(std::shared_ptr<Game> game, int malusOrBonus) override;
        GameModeName getNameMode() override { return GameModeName::Classic; }
        int getNbrMalus(int nbrLineComplet) const;
};

#endif
#ifndef CLASSICMODE_HPP
#define CLASSICMODE_HPP

#include <iostream>
#include "GameMode.hpp"
#include "Game.hpp"


class ClassicMode : public GameMode{
    

    public:
        ClassicMode();
        void feautureMode(Game& game) override;
        //void useMalus(int nbrMalus) override;
        //void choosePlayer();
        //bool acceptChosenPlayer(int chosenPlayer);
        //int enterPlayer();
        int getNbrMalus(int nbrLineComplet) const;
};

#endif
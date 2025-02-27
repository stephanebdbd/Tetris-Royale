#ifndef ROYALMODE_HPP
#define ROYALMODE_HPP

#include <iostream>
#include "GameMode.hpp"
#include "Game.hpp"


class RoyalMode : public GameMode{
    
    int energie;

    public:
        RoyalMode();
        void feautureMode() override;
        //void useMalus(int nbrMalus) override;
        //void useBonus() override;
        //int enterPlayer();
};

#endif
#ifndef GAMEMODE_HPP
#define GAMEMODE_HPP

#include <iostream>
#include "Game.hpp"


class GameMode {

    public:
        GameMode();
        virtual void feautureMode(Game& game) {};
        virtual void useMalus(int nbrMalus){};
        virtual void useBonus(){};
        std::string getNameMode() const {return NameMode;}

};

#endif
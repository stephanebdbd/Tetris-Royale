#ifndef GAMEMODE_HPP
#define GAMEMODE_HPP

#include <iostream>
#include "Game.hpp"

enum GameModeName{
    Endless,
    Duel,
    Classic,
    Royal_Competition,
};

class GameMode {
    
    std::vector<Game> games;
    public:
        GameMode();
        virtual void feautureMode(Game& game) {};
        virtual void useMalus(int nbrMalus){};
        virtual void useBonus(){};
        virtual GameModeName getNameMode();

};

#endif
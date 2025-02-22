#ifndef GAMEMODE_HPP
#define GAMEMODE_HPP

#include <iostream>
#include "Game.hpp"


class GameMode {
    std::string NameMode;

    public:
        GameMode(std::string NameMode) : NameMode(NameMode) {}
        virtual void startGame(Game& game) {};
        virtual void applyMalus(){};
        virtual void applyBonus(){};
        std::string getNameMode() const {return NameMode;}

};

#endif
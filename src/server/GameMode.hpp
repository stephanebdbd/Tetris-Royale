#ifndef GAMEMODE_HPP
#define GAMEMODE_HPP

#include <iostream>
#include <memory>

class Game;

enum GameModeName{
    Endless,
    Duel,
    Classic,
    Royal_Competition,
};

class GameMode {
    
    public:
        GameMode() = default;
        virtual void featureMode(std::shared_ptr<Game> game) = 0;
        //virtual void useMalus(int nbrMalus) = 0;
        virtual GameModeName getNameMode() = 0;

};

#endif
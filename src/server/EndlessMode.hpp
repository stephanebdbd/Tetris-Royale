#ifndef ENDLESSMODE_HPP
#define ENDLESSMODE_HPP

#include <iostream>
#include "GameMode.hpp"
#include "Game.hpp"


class EndlessMode : public GameMode{
    public:
        EndlessMode() : GameMode("EndlessMode"){};
        
};

#endif
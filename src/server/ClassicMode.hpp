#ifndef CLASSICMODE_HPP
#define CLASSICMODE_HPP

#include <iostream>
#include "GameMode.hpp"
#include "Game.hpp"


class ClassicMode : public GameMode{
    //playerGrids est un tableau de grille de jeu il faut l ajouter apres
    public:
        ClassicMode();
        void startGame(Game& game) override;
        void applyMalus() override;
        void ClassicMode::chooseCible();
        bool ClassicMode::acceptPlayerCible(int cible);
        int enterCible();
        int getNbrMalus(int nbrLineComplet) const;
};

#endif
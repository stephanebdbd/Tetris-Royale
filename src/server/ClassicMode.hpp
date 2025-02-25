#ifndef CLASSICMODE_HPP
#define CLASSICMODE_HPP

#include <iostream>
#include "GameMode.hpp"
#include "Game.hpp"
#include "Player.hpp"


class ClassicMode : public GameMode{
    //playerGrids est un tableau de grille de jeu il faut l ajouter apres
    std::map<int, Grid>& playerGrids;
    std::vector<Player> players;

    public:
        ClassicMode();
        void feautureMode(Game& game) override;
        void useMalus(int nbrMalus) override;
        void initializePlayerGrids(std::map<int, Grid>& idPlayerGrids){playerGrids = idPlayerGrids};
        void initializePlayers(std::vector<Player> listPlayers;){players = listPlayers};
        void choosePlayer();
        bool acceptChosenPlayer(int cible);
        int enterPlayer();
        int getNbrMalus(int nbrLineComplet) const;
};

#endif
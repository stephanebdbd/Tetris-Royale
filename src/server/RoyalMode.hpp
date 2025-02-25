#ifndef ROYALMODE_HPP
#define ROYALMODE_HPP

#include <iostream>
#include "GameMode.hpp"
#include "Game.hpp"
#include "Player.hpp"


class RoyalMode : public GameMode{
    //playerGrids est un tableau de grille de jeu il faut l ajouter apres
    std::map<int, Grid>& playerGrids;
    std::vector<Player> players;

    public:
        RoyalMode();
        void feautureMode(Game& game) override;
        void useMalus(int nbrMalus) override;
        void useBonus() override;
        void initializePlayerGrids(std::map<int, Grid>& idPlayerGrids){playerGrids = idPlayerGrids};
        void initializePlayers(std::vector<Player> listPlayers;){players = listPlayers};
        void choosePlayer();
        bool acceptChosenPlayer(int cible);
        int enterPlayer();
};

#endif
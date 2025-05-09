#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include "../common/ConstantsTextures.hpp"

struct Textures {
    sf::Texture connexion, grid, game, settings, 
                chat, ranking, logoConnexion, logoNotification,
                logoSettings, logoTeams, logoRanking,
                logoChat, logoMain, logoExit, logoAddFriend,
                logoFrindsRequest, mode, player, viewer, 
                plus, esc, P_O, playerClicked, accept, rejoindre, teams, team, manageTeam, friends, addFriend;

    Textures() = default;
    ~Textures() = default;

    void loadTexture(sf::Texture& texture, const std::string& filePath);
    void loadTextures();
};

#endif 

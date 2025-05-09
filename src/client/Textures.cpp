#include "Textures.hpp"

void Textures::loadTexture(sf::Texture& texture, const std::string& filePath) {
    if (!texture.loadFromFile(filePath)) {
        std::cerr << "Erreur: Impossible de charger la texture depuis " << filePath << std::endl;
    }
}

void Textures::loadTextures() {
    loadTexture(connexion, ConnexionBackGround);
    loadTexture(chat, ChatBackGround);
    loadTexture(ranking, RankingBackGround);
    loadTexture(logoConnexion, LogoBackGround);
    loadTexture(logoNotification, LogoNotification);
    loadTexture(logoSettings, LogoSettings);
    loadTexture(logoTeams, LogoTeams);
    loadTexture(logoRanking, LogoRanking);
    loadTexture(logoMain, LogoMain);
    loadTexture(logoExit, LogoExit);
    loadTexture(logoAddFriend, LogoAddFriend);
    loadTexture(mode, backgroundMode);
    loadTexture(player, LogoRemote);
    loadTexture(viewer, LogoViewer);
    loadTexture(plus, LogoPLus);
    loadTexture(esc, LogoEsc);
    loadTexture(P_O, LogoP_O);
    loadTexture(playerClicked, LogoThreePoint);
    loadTexture(accept, LogoAccept);
    loadTexture(rejoindre, backgroundRejoindre);
    loadTexture(teams, backgroundTeams);
    loadTexture(friends, backgroundFriends);
    loadTexture(addFriend, backgroundAddFriend);
    loadTexture(manageTeam, backgroundManageTeam);
    loadTexture(team, backgroundTeam);
    loadTexture(medals[0], medal1);
    loadTexture(medals[1], medal2);
    loadTexture(medals[2], medal3);
}

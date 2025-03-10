#include "Menu.hpp"
#include "../common/jsonKeys.hpp"  


json Menu::getMainMenu0() const {
    json menu = {
        {jsonKeys::TITLE, "Bienvenue dans Tetris Royal !"},
        {jsonKeys::OPTIONS, {
            {"1. ", "Se connecter"},
            {"2. ", "Créer un compte"},
            {"3. ", "Quitter"}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getMainMenu1() const {
    json menu = {
        {jsonKeys::TITLE, "Menu principal"},
        {jsonKeys::OPTIONS, {
            {"1. ", "Jouer"},
            {"2. ", "Amis"},
            {"3. ", "Classement"},
            {"4. ", "Chat"},
            {"5. ", "Retour"}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getRegisterMenu1() const {
    json menu = {
        {jsonKeys::TITLE, "Création de compte - Etape 1"},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer votre pseudo", ":"},

        }},
        {jsonKeys::INPUT, ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getRegisterMenuFailed() const {
    json menu = {
        {jsonKeys::TITLE, "Création de compte - Etape 1"},
        {jsonKeys::OPTIONS, {
            {"Pseudo déjà utilisé ! \n Veuillez ", ""},
            {"insérer", " votre pseudo : "},
        }},
        {jsonKeys::INPUT, ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getRegisterMenu2() const {
    json menu = {
        {jsonKeys::TITLE, "Création de compte - Etape 2"},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer votre mot de passe", ":"},
        }},
        {jsonKeys::INPUT, ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLoginMenu1() const {
    json menu = {
        {jsonKeys::TITLE, "Connexion au compte - Etape 1"},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer votre pseudo", ":"},

        }},
        {jsonKeys::INPUT, ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLoginMenuFailed1() const {
    json menu = {
        {jsonKeys::TITLE, "Connexion au compte - Etape 1"},
        {jsonKeys::OPTIONS, {
            {"Aucun identifiant n'a été trouvé ! \n Veuillez ", ""},
            {"insérer", " votre pseudo : "},
        }},
        {jsonKeys::INPUT, ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLoginMenu2() const {
    json menu = {
        {jsonKeys::TITLE, "Connexion au compte  - Etape 2"},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer votre mot de passe", ":"},

        }},
        {jsonKeys::INPUT, ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLoginMenuFailed2() const {
    json menu = {
        {jsonKeys::TITLE, "Connexion au compte - Etape 2"},
        {jsonKeys::OPTIONS, {
            {"Mot de passe incorrect ! \n Veuillez ", ""},
            {"insérer", " votre mot de passe : "},
        }},
        {jsonKeys::INPUT, ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getJoinOrCreateGame() const {
    json menu = {
        {jsonKeys::TITLE, "Rejoindre ou créer une partie"},
        {jsonKeys::OPTIONS, {
            {"1. ", "créer"},
            {"2. ", "rejoindre"},
            {"3. ", "Retour"}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getChatMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Menu du chat"},
        {jsonKeys::OPTIONS, {
            {"1. ", "Créer une Room"},
            {"2. ", "Rejoindre une Room"},
            {"3. ", "Lister mes Rooms"},
            {"4. ", "private chat"},
            {"5. ", "Retour"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getCreateChatRoomMenu() const {
    json menu = {
        {"title", "Créer une Room"},
        {"options", {
            {"Veuillez insérer le nom de la Room", ":"},
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getJoinChatRoomMenu(const std::vector<std::string>& chatRooms) const {
    json menu = {
        {"title", "Liste des Rooms"},
        {"options", json::array()},
        {"input", "Tapez 'join.nomRoom' pour rejoindre une Room."}
    };

    int index = 1;
    for (const auto& room : chatRooms) {
        menu["options"].push_back(std::to_string(index) + ". " + room);  // ✅ Ajout en tant que chaîne et non objet
        index++;
    }

    return menu.dump() + "\n";  // Affichage formaté
}

json Menu::getListChatRoomsMenu(const std::vector<std::string>& chatRooms) const {
    json menu = {
        {"title", "Liste des Rooms"},
        {"options", json::array()},
        {"input", "Tapez 'request' pour consulter les demandes d'amis."}
    };

    int index = 1;
    for (auto& room : chatRooms) {
        menu["options"].push_back(std::to_string(index) + ". " + room);  // ✅ Ajout en tant que chaîne et non objet
        index++;
    }
    return menu.dump() + "\n";  // Affichage formaté
}

/*
json Menu::getRequestChatRoomMenu(const std::vector<std::string>& chatRooms) const {
    json menu = {
        {"title", "Liste des demandes :"},
        {"options", json::array()},
        {"input", "Tapez 'accept.nomRoom' ou 'reject.nomRoom' pour accepter ou refuser une demande de chat."}
    };
    return menu.dump() + "\n";  // Affichage formaté
}
*/

json Menu::getFriendMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Gestion des amis"},
        {jsonKeys::OPTIONS, {
            {"1. ", "Ajouter un ami"},
            {"2. ", "liste des amis"},
            {"3. ", "liste des demandes d'amis"},
            {"4. ", "Retour"}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}
json Menu::getFriendListMenu(const std::vector<std::string>& friends) const {
    json menu = {
        {"title", "Liste de vos amis"},
        {"options", json::array()},
        {"input", "Tapez 'del.pseudo' pour supprimer un ami ou 'del.all' pour tout supprimer:    "}
    };

    int index = 1;
    for (const auto& user : friends) {
        menu["options"].push_back(std::to_string(index) + ". " + user);
        index++;
    }

    return menu.dump() + "\n";  // Affichage formaté
}


json Menu::getAddFriendMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Ajouter un ami"},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer l'ID de l'ami à ajouter", ":"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

/*json Menu::getRemoveFriendMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Supprimer un ami"},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer l'ID de l'ami à supprimer", ":"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}*/

json Menu::displayMessage(const std::string& message) const {
    json menu = {
        {"title", message},
        {"options", {
            {"", "pour retourner au menu principal appyer sur q"},
            {"2. ", "Annuler"}
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getRequestsListMenu(const std::vector<std::string>& pendingRequests) const {
    json menu = {
        {"title", "Liste des demandes d'amis: "},
        {"options", json::array()},
        {"input", "Tapez 'accept.pseudo' ou 'reject.pseudo' pour accepter ou refuser une demande d'amitie :    "}
    };

    int index = 1;
    for (const auto& user : pendingRequests) {
        menu["options"].push_back(std::to_string(index) + ". " + user);  
        index++;
    }

    return menu.dump() + "\n";
}


json Menu::getRankingMenu(const std::vector<std::pair<std::string, int>>& ranking) const {
    json menu = {
        {jsonKeys::TITLE, "Classement des meilleurs joueurs"},
        {jsonKeys::OPTIONS, json::object()},  // car avec array la key commence a 0
        {jsonKeys::INPUT, "Appuyez sur la touche \"1\" pour revenir au menu principal : "}
    };

    // Ajoute les 10 meilleurs joueurs
    int limit = std::min(10, static_cast<int>(ranking.size()));
    for (int i = 0; i < limit; ++i) {
        std::string key = "    " + std::to_string(i + 1) + ". ";
        std::string value = ranking[i].first + " - " + std::to_string(ranking[i].second);
        menu[jsonKeys::OPTIONS][key] = value;
    }
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getGameOverMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Game Over"},
        {jsonKeys::OPTIONS, {
            {"1. ", "Rejouer"},
            {"2. ", "Retour au menu principal"}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getGameModeMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Game Mode"},
        {jsonKeys::OPTIONS, {
            {"1. ", "Endless"},
            {"2. ", "Duel"},
            {"3. ", "Classic"},
            {"4. ", "Royal Competition"}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLobbyMenu1() const {
    json menu = {
        {jsonKeys::TITLE, "Configuration des paramètres de la partie:"},
        {jsonKeys::OPTIONS, {
            {"\\<number> ", "Nombre maximal de joueurs"}
        }},

        {jsonKeys::INPUT, "entrez le paramètre: "}
        
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLobbyMenu2(int maxPlayers, const std::string& mode, int amountOfPlayers) const {
    
    json menu = {
        {jsonKeys::TITLE, "La salle d'invitation et d'attente:"},
        {jsonKeys::OPTIONS, {
            {"Game Mode: ", mode},
            {"Joueurs Maximum: ", std::to_string(maxPlayers)},
            {"Nombre actuel de joueurs: ", std::to_string(amountOfPlayers)},
            {"\\invite\\player\\<name> ", "Inviter un joueur"},
            {"\\invite\\viewer\\<name> ", "Inviter un spectateur"},
            {"\\quit ", "Quitter la partie"}

        }},

        {jsonKeys::INPUT, "entrez le paramètre: "}
        
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getGameRequestsListMenu(const std::vector<std::vector<std::string>>& pendingRequests) const {
    json menu = {
        {"title", "Liste des demandes du rejoindre un jeu: "},
        {"options", json::array()},
        {"input", "Tapez 'accept.<Number Room>' pour accepter une demande :    "}
    };

    int index = 1;
    for (const auto& invitation : pendingRequests) {
        menu["options"].push_back(std::to_string(index) + ". " + "Vous êtes invités par '"+invitation[0]+
        "' en tant que '"+invitation[1]+"' dans la GameRoom '"+invitation[2]+"'");  
        index++;
    }

    return menu.dump() + "\n";
}

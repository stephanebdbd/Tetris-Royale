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
            {"3. ", "Gerer mes Rooms"},
            {"4. ", "chat"},
            {"5. ", "Retour"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getCreateChatRoomMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Créer une Room"},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer le nom de la Room", ":"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getJoinChatRoomMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Liste des Rooms"},
        {jsonKeys::OPTIONS, json::array()},
        {jsonKeys::INPUT, "Tapez le nom de la room à rejoindre ou './quit' pour quitter:    "}
    };

    return menu.dump() + "\n";  // Affichage formaté
}





json Menu::getListe(const std::vector<std::string>& data, std::string title, const std::string& input) const {
    json menu = {
        {jsonKeys::TITLE, title},
        {jsonKeys::OPTIONS, json::array()},
        {jsonKeys::INPUT, input}
    };

    int index = 1;
    for (const auto& item : data) {
        menu[jsonKeys::OPTIONS].push_back(std::to_string(index) + ". " + item);
        index++;
    }
    return menu.dump() + "\n";  // Affichage formaté
}

json Menu::getListeMembers(const std::vector<std::string>& data) const {
    return getListe(data, "Liste des membres", "Taper './quit' pour quitter:  ");
}

json Menu::getManageChatRoomsMenu(const std::vector<std::string>& chatRooms) const {
    return getListe(chatRooms, "Mes Rooms", "Tapez le nom d' une room pour la gérer ou './quit' pour quitter:    ");
}

json Menu::getManageRoomMenu(bool isAdmin) const {
    json menu = {
        {jsonKeys::TITLE, "Gestion de la Room"},
        {jsonKeys::OPTIONS, {
            {"1. ", "Lister les membres"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    int i;
    if(isAdmin){
        menu[jsonKeys::OPTIONS]["2. "] = "Ajouter un membre";
        menu[jsonKeys::OPTIONS]["3. "] = "Ajouter un admin";
        menu[jsonKeys::OPTIONS]["4. "] = "les demandes d'ajout";
        i = 5;
    }else{
        i = 2;
    }
    menu[jsonKeys::OPTIONS][std::to_string(i) + ". "] = "Quitter la Room";
    menu[jsonKeys::OPTIONS][std::to_string(i+1) + ". "] = "Retour";
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getAddDeleteMemberMenu(std::string title, std::string input) const {
    json menu = {
        {jsonKeys::TITLE, title},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer le pseudo du membre", ":"},
        }},
        {jsonKeys::INPUT, input}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getAddMembreMenu() const {
    return getAddDeleteMemberMenu("Ajouter un membre", "Tapez 'pseudo' pour ajouter un membre ou 'quit' pour quitter: ");
}

json Menu::getAddAdmin() const{
    return getAddDeleteMemberMenu("Ajouter un admin", "Tapez 'pseudo' pour ajouter un membre ou 'quit' pour quitter: ");
}

json Menu::getListeRequests(const std::vector<std::string>& data) const {
    return getListe(data, "Liste des demandes d'ajout", "Tapez 'accept.pseudo' ou 'reject.pseudo'  pour accepter ou refuser une demande d'amitie ou 'quit' pour quitter :    ");
}

json Menu::getQuitRoomMenu(bool isAdmin, bool lastAdmin) const {
    json menu = {
        {jsonKeys::TITLE, "Quitter la Room"},
        {jsonKeys::OPTIONS, {}},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    if (isAdmin && lastAdmin) {
        menu[jsonKeys::OPTIONS]["1. "] = "Supprimer la Room";
    }else{
        menu[jsonKeys::OPTIONS]["1. "] = "Quitter la Room";
    }
    menu[jsonKeys::OPTIONS]["2. "] = "Retour";
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getFriendMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Créer une Room"},
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


json Menu::getRequestsListMenu(const std::vector<std::string>& pendingRequests) const {
    json menu = {
        {jsonKeys::TITLE, "Liste des demandes d'amis: "},
        {jsonKeys::OPTIONS, json::array()},
        {jsonKeys::INPUT, "Tapez 'accept.pseudo' ou 'reject.pseudo' pour accepter ou refuser une demande d'amitie :    "}
    };

    int index = 1;
    for (const auto& user : pendingRequests) {
        menu[jsonKeys::OPTIONS].push_back(std::to_string(index) + ". " + user);  
        index++;
    }

    return menu.dump() + "\n";
}
json Menu::displayMessage(const std::string& message) const {
    json menu = {
        {jsonKeys::TITLE, message},
        {jsonKeys::OPTIONS, {}},
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
        {jsonKeys::TITLE, "Liste des demandes du rejoindre un jeu: "},
        {jsonKeys::OPTIONS, json::array()},
        {jsonKeys::INPUT, "Tapez 'accept.<Number Room>' pour accepter une demande :    "}
    };

    int index = 1;
    for (const auto& invitation : pendingRequests) {
        menu[jsonKeys::OPTIONS].push_back(std::to_string(index) + ". " + "Vous êtes invités par '"+invitation[0]+
        "' en tant que '"+invitation[1]+"' dans la GameRoom '"+invitation[2]+"'");  
        index++;
    }

    return menu.dump() + "\n";
}

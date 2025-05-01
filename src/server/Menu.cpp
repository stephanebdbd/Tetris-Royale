#include "Menu.hpp"
#include "../common/jsonKeys.hpp"
#include <fstream>
#include <iostream>


json Menu::getMainMenu0() const {
    json menu = {
        {jsonKeys::TITLE, "Bienvenue dans Tetris Royal !"},
        {jsonKeys::OPTIONS, {
            {"   1. ", "Se connecter"},
            {"   2. ", "Créer un compte"},
            {"   3. ", "Quitter"}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getMainMenu1() const {
    json menu = {
        {jsonKeys::TITLE, "Menu principal"},
        {jsonKeys::OPTIONS, {
            {"   1. ", "Jouer"},
            {"   2. ", "Amis"},
            {"   3. ", "Classement"},
            {"   4. ", "Teams"},
            {"   5. ", "Retour"}
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

json Menu::getTeamsMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Menu du chat"},
        {jsonKeys::OPTIONS, {
            {"1. ", "Créer une Team"},
            {"2. ", "Rejoindre une Team"},
            {"3. ", "Teams Invitation"},
            {"4. ", "Gérer mes Teams"},
            {"5. ", "Team chat"},
            {"6. ", "Retour"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getCreateChatRoomMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Créer une Team"},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer le nom de la Team", ":"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getJoinChatRoomMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Joindre une Team"},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer le nom de la Team", ":"},
        }},
        {jsonKeys::INPUT, " '/back' pour quitter:    "}
    };

    return menu.dump() + "\n";  // Affichage formaté
}

json Menu::getInvitationsRoomsMenu(const std::vector<std::string>& invitations) const {
    return getListe(invitations, "Invitations en attente:", "Tapez 'accept.roomName' ou 'reject.roomName' ou '/back' : ");
}

json Menu::getListe(const std::vector<std::string>& data, std::string title, const std::string& input) const {
    json menu = {
        {jsonKeys::TITLE, title},
        {jsonKeys::OPTIONS, json::object()},  // Utilisation d'un objet pour les options
        {jsonKeys::INPUT, input}
    };

    // Ajouter les éléments dans l'objet OPTIONS
    for (auto index = 0u; index < 10 && index < data.size(); ++index) {
        std::string item = data[index];
        std::string key = "    " + std::to_string(index + 1) + ". ";  // Créer la clé de l'option
        menu[jsonKeys::OPTIONS][key] = item;  // Ajout de la paire clé-valeur dans OPTIONS
    }

    return menu.dump() + "\n";  // Utiliser un espacement de 4 pour une belle mise en forme
}

json Menu::getListeMembers(const std::vector<std::string>& data) const {
    return getListe(data, "Liste des membres", "Taper '/back' pour quitter:  ");
}

json Menu::getManageTeamsMenu(const std::vector<std::string>& chatRooms) const {
    if (chatRooms.empty()) {
        json menu = {
            {jsonKeys::TITLE, "Mes Teams"},
            {jsonKeys::OPTIONS, {
                {"Vous n'avez aucun Team à gérer.", ""}
            }},
            {jsonKeys::INPUT, "Tapez '/back' pour quitter: "}
        };
        return menu.dump() + "\n";  // Convertir en chaîne JSON
    } else {
        return getListe(chatRooms, "Mes Teams", "Tapez le nom d'une team pour la gérer ou '/back' pour quitter: ");
    }
}
json Menu::getManageTeamMenu(bool isAdmin, bool lastAdmin) const {
    json menu = {
        {jsonKeys::TITLE, "Gestion de la Team"},
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
        if(!lastAdmin){
            menu[jsonKeys::OPTIONS]["5. "] = "Quitter la room";
        }else{
            menu[jsonKeys::OPTIONS]["5. "] = "Supprimer la room";
        }
        i = 5;
    }else{
        menu[jsonKeys::OPTIONS]["2. "] = "quitter la room";
        i = 2;
    }
    menu[jsonKeys::OPTIONS][std::to_string(i+1) + ". "] = "Retour";
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getAddMemberAdminMenu(std::string title, std::string input) const {
    json menu = {
        {jsonKeys::TITLE, title},
        {jsonKeys::OPTIONS, {}},
        {jsonKeys::INPUT, input}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getAddMembreMenu() const {
    return getAddMemberAdminMenu("Ajouter un membre", "Tapez 'pseudo' pour ajouter un membre ou '/back' pour quitter: ");
}

json Menu::getAddAdmin() const{
    return getAddMemberAdminMenu("Ajouter un admin", "Tapez 'pseudo' pour ajouter un admin ou '/back' pour quitter: ");
}

json Menu::getListeRequests(const std::vector<std::string>& data) const {
    return getListe(data, "Liste des demandes d'ajout", "Tapez 'accept'/'reject'.pseudo et '/back' pour quitter : ");
}


json Menu::getDeleteRoomConfirmationMenu(const std::string& roomName) const {
    json menu = {
        {jsonKeys::TITLE, "Confirmation de suppression"},
        {jsonKeys::OPTIONS, {
            {"Vous etes sur de vouloir supprimer la room '" + roomName + "' ? (oui/non)", ""}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getQuitRoomConfirmationMenu(const std::string& roomName) const {
    json menu = {
        {jsonKeys::TITLE, "Confirmation de quitter la room"},
        {jsonKeys::OPTIONS, {
            {"Vous etes sur de vouloir quitter la room '" + roomName + "' ? (oui/non)", ""}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}





json Menu::getFriendMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Amis"},
        {jsonKeys::OPTIONS, {
            {"   1. ", "Ajouter un ami"},
            {"   2. ", "liste des amis"},
            {"   3. ", "liste des demandes d'amis"},
            {"   4. ", "chat prive"},
            {"   5. ", "Retour"}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getAddFriendMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Ajouter un ami"},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer le pseudo de l'ami à ajouter", ":"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getRequestsListMenu(const std::vector<std::string>& pendingRequests) const {
    return getListe(pendingRequests, "Liste des demandes d'amis: ", "Tapez 'accept.pseudo' ou 'reject.pseudo' pour accepter ou refuser une demande d'amitie :    ");
}

json Menu::getFriendListMenu(const std::vector<std::string>& friends) const {
    return getListe(friends, "Liste de vos amis", "Taper '/back' pour quitter:  ");
}

json Menu::getChooseContactMenu(const std::vector<std::string>& contacts, bool prive) const {
    const std::string title = prive ? "Chatter avec un ami" : "Chatter dans une Room";
    const std::string input = prive ? "Tapez le pseudo de l'ami" : "Tapez le nom de la room";
    if (contacts.empty()) {
        json menu = {
            //{jsonKeys::TITLE, room ? "Chatter avec une Room" : "Chatter avec un ami"},
            {jsonKeys::TITLE, title},
            {jsonKeys::OPTIONS, {
                {"Vous n'avez aucune contact disponible pour chatter.", ""}
            }},
            {jsonKeys::INPUT, "Tapez '/back' pour quitter: "}
        };
        return menu.dump() + "\n";  // Convertir en chaîne JSON
    } else {
        return getListe(contacts, title, input + "pour chatter ou '/back' pour quitter: ");
    }
}

json Menu::displayMessage(const std::string& message) const {
    json menu = {
        {jsonKeys::TITLE, message},
        {jsonKeys::OPTIONS, {}},
        {jsonKeys::INPUT, ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
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

json Menu::getEndGameMenu(const std::string& message) const {
    json menu = {
        {jsonKeys::TITLE, message},
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
            {"4. ", "Royal Competition"},
            {"5. ", "Retour"}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLobbyMenu(int maxPlayers, const std::string& mode, int amountOfPlayers, int speed, int energy) const {
    std::string key;
    std::string value;
    if (mode == "Royal Competition"){
        key = "Energie: ";
        value = std::to_string(energy) + "\n";
    }
    std::string space(8, ' ');
    json menu = {
        {jsonKeys::TITLE, space + "TETRIS ROYAL LOBBY"},
        {jsonKeys::TEXT, "isText"},
        {jsonKeys::OPTIONS, {
            "\n",
            "Game Mode: " + mode,
            "Joueurs Maximum: " + std::to_string(maxPlayers),
            "Nombre actuel de joueurs: " + std::to_string(amountOfPlayers),
            "Vitesse: " + std::to_string(speed),
        }},
        {jsonKeys::INPUT, space + "Entrez: "}       
    };

    if (mode == "Royal Competition")
        menu[jsonKeys::OPTIONS].push_back(key + value);
    menu[jsonKeys::OPTIONS].push_back("\n");
    menu[jsonKeys::OPTIONS].push_back("/help Afficher le mode d'emploi");
    menu[jsonKeys::OPTIONS].push_back("/back Quitter la partie");
    menu[jsonKeys::OPTIONS].push_back("\n");

    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getGameRequestsListMenu(const std::vector<std::vector<std::string>>& pendingRequests) const {
    json menu = {
        {jsonKeys::TITLE, "Liste des demandes du rejoindre un jeu: "},
        {jsonKeys::OPTIONS, json::object()},
        {jsonKeys::INPUT, "Tapez 'accept.<state>.<Number Room>' pour accepter une demande ou '/back' pour faire un retour:    "}
    };

    int index = 1;
    for (const auto& invitation : pendingRequests) {
        std::string key = std::to_string(index) + ". ";
        std::string value = "Vous êtes invités par '" + invitation[0] +
                            "' en tant que '" + invitation[1] +
                            "' dans la GameRoom '" + invitation[2] + "'";
        menu[jsonKeys::OPTIONS][key] = value;
        index++;
    }

    return menu.dump() + "\n";
}

json Menu::getWinGameMenu() const {
    json menu = {
        {jsonKeys::TITLE, "YOU WIN!!"},
        {jsonKeys::OPTIONS, {
            {"1. ", "Rejouer"},
            {"2. ", "Retour au menu principal"}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getHelpMenu(bool isRC, bool canEditMaxPlayer) const {    
    json menu = {
        {jsonKeys::TITLE, "Mode d'emploi de l'utilisation du chat"},
        {jsonKeys::OPTIONS, json::array()},
        {jsonKeys::TEXT, "isText"},
        {jsonKeys::INPUT, "Tapez '/back' pour quitter: "}
    };
    std::ifstream file("help.txt");
    if (!file) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier help.txt." << std::endl;
        return json();
    }
    try {
        std::string line; 
        menu[jsonKeys::OPTIONS].push_back("\n");
        while (std::getline(file, line)){
            if (isRC && line.find("/energy") != std::string::npos)
                menu[jsonKeys::OPTIONS].push_back("    " + line);
            else if (canEditMaxPlayer && line.find("/max") != std::string::npos)
                menu[jsonKeys::OPTIONS].push_back("    " + line);
            else if ((line.find("/max") == std::string::npos) && (line.find("/energy") == std::string::npos))
                menu[jsonKeys::OPTIONS].push_back("    " + line);
        }
        menu[jsonKeys::OPTIONS].push_back("\n");
    } catch (const json::parse_error& e) {
        std::cerr << "Erreur de parsing JSON (MenuHelp) : " << e.what() << std::endl;
    }
    file.close();
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getQuitGameMenu() const {
    json menu = {
        {jsonKeys::TITLE, "GAME END"},
        {jsonKeys::OPTIONS, {
            {"1. ", "Rejouer"},
            {"2. ", "Retour au menu principal"}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


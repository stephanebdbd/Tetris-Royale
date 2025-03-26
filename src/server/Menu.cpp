#include "Menu.hpp"
#include "../common/jsonKeys.hpp"

const std::string messageHelp = "Appuyez sur ./ret pour revenir en arrière et ./quit pour revenir au menu principal.";

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
        {jsonKeys::DEFAULT, messageHelp},
        {jsonKeys::HELP, ""},
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
        {jsonKeys::DEFAULT, messageHelp},
        {jsonKeys::HELP, ""},
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
        {jsonKeys::DEFAULT, messageHelp},
        {jsonKeys::HELP, ""},
        {jsonKeys::OPTIONS, {
            {"1. ", "Créer une Room"},
            {"2. ", "Rejoindre une Room"},
            {"3. ", "Invitations en attente"},
            {"4. ", "Gerer mes Rooms"},
            {"5. ", "chat"},
            {"6. ", "Retour"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getCreateChatRoomMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Créer une Room"},
        {jsonKeys::DEFAULT, messageHelp},
        {jsonKeys::HELP, ""},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer le nom de la Room", ":"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getJoinChatRoomMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Joindre une Room"},
        {jsonKeys::DEFAULT, messageHelp},
        {jsonKeys::HELP, ""},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer le nom de la Room", ":"},
        }},
        {jsonKeys::INPUT, "Tapez le nom de la room à rejoindre ou './quit' pour quitter:    "}
    };

    return menu.dump() + "\n";  // Affichage formaté
}


json Menu::getInvitationsRoomsMenu(const std::vector<std::string>& invitations) const {
    return getListe(invitations, "Invitations en attente:", "Tapez 'accept.roomName' ou 'reject.roomName' ou './quit' : ");
}

json Menu::getListe(const std::vector<std::string>& data, std::string title, const std::string& input) const {
    json menu = {
        {jsonKeys::TITLE, title},
        {jsonKeys::DEFAULT, messageHelp},
        {jsonKeys::HELP, ""},
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
    if (chatRooms.empty()) {
        json menu = {
            {jsonKeys::TITLE, "Mes Rooms"},
            {jsonKeys::DEFAULT, messageHelp},
            {jsonKeys::HELP, ""},
            {jsonKeys::OPTIONS, {
                {"Vous n'avez aucun room à gérer.", ""}
            }},
            {jsonKeys::INPUT, "Tapez './quit' pour quitter: "}
        };
        return menu.dump() + "\n";  // Convertir en chaîne JSON
    } else {
        return getListe(chatRooms, "Mes Rooms", "Tapez le nom d'une room pour la gérer ou './quit' pour quitter: ");
    }
}
json Menu::getManageRoomMenu(bool isAdmin, bool lastAdmin) const {
    json menu;
    menu[jsonKeys::TITLE] = "Gestion de la Room";
    menu[jsonKeys::DEFAULT] = messageHelp;
    menu[jsonKeys::HELP] = "";
    menu[jsonKeys::OPTIONS] = json::array({"1. Lister les membres"});

    if (isAdmin) {
        menu[jsonKeys::OPTIONS].push_back("2. Ajouter un membre");
        menu[jsonKeys::OPTIONS].push_back("3. Ajouter un admin");
        menu[jsonKeys::OPTIONS].push_back("4. Voir les demandes d'ajout");
        menu[jsonKeys::OPTIONS].push_back(lastAdmin ? "5. Supprimer la room" : "5. Quitter la room");
    } else {
        menu[jsonKeys::OPTIONS].push_back("2. Quitter la room");
    }

    menu[jsonKeys::OPTIONS].push_back(std::to_string(menu[jsonKeys::OPTIONS].size() + 1) + ". Retour");
    menu[jsonKeys::INPUT] = "Votre choix: ";
    
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getAddMemberAdminMenu(std::string title, std::string input) const {
    json menu = {
        {jsonKeys::TITLE, title},
        {jsonKeys::DEFAULT, messageHelp},
        {jsonKeys::HELP, ""},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer le pseudo du membre", ":"},
        }},
        {jsonKeys::INPUT, input}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getAddMembreMenu() const {
    return getAddMemberAdminMenu("Ajouter un membre", "Tapez 'pseudo' pour ajouter un membre ou 'quit' pour quitter: ");
}

json Menu::getAddAdmin() const{
    return getAddMemberAdminMenu("Ajouter un admin", "Tapez 'pseudo' pour ajouter un membre ou 'quit' pour quitter: ");
}

json Menu::getListeRequests(const std::vector<std::string>& data) const {
    return getListe(data, "Liste des demandes d'ajout", "Tapez 'accept.pseudo' ou 'reject.pseudo'  pour accepter ou refuser une demande d'amitie ou 'quit' pour quitter :    ");
}


json Menu::getDeleteRoomConfirmationMenu(const std::string& roomName) const {
    json menu = {
        {jsonKeys::TITLE, "Confirmation de suppression"},
        {jsonKeys::DEFAULT, messageHelp},
        {jsonKeys::HELP, ""},
        {jsonKeys::OPTIONS, {
            {"Vous êtes sûr de vouloir supprimer la room '" + roomName + "' ? (oui/non)", ""}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getQuitRoomConfirmationMenu(const std::string& roomName) const {
    json menu = {
        {jsonKeys::TITLE, "Confirmation de quitter la room"},
        {jsonKeys::DEFAULT, messageHelp},
        {jsonKeys::HELP, ""},
        {jsonKeys::OPTIONS, {
            {"Vous êtes sûr de vouloir quitter la room '" + roomName + "' ? (oui/non)", ""}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}





json Menu::getFriendMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Créer une Room"},
        {jsonKeys::DEFAULT, messageHelp},
        {jsonKeys::HELP, ""},
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
        {jsonKeys::DEFAULT, "Appuyez sur ./ret pour revenir au menu principal."},
        {jsonKeys::HELP , "Pour ajouter un ami, veuillez insérer son pseudo."},
        
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer le pseudo de l'ami à ajouter", ":"},
        }},
        
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getRequestsListMenu(const std::vector<std::string>& pendingRequests) const {
    json menu = {
        {jsonKeys::TITLE, "Liste des demandes d'amis: "},
        {jsonKeys::DEFAULT, messageHelp},
        {jsonKeys::HELP, ""},
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
        {jsonKeys::INPUT, ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getFriendListMenu(const std::vector<std::string>& friends) const {
    json menu = {
        {jsonKeys::TITLE, "Liste de vos amis"},
        {jsonKeys::DEFAULT, messageHelp},
        {jsonKeys::HELP, "Tapez 'del.pseudo' pour supprimer un ami ou 'del.all' pour tout supprimer:    "},
        {jsonKeys::OPTIONS, json::array()},
        {jsonKeys::INPUT, ""}
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

json Menu::getEndGameMenu() const {
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
            {"4. ", "Royal Competition"},
            {"5. ", "Retour"}
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLobbyMenu1() const {
    json menu = {
        {jsonKeys::TITLE, "Configuration des paramètres de la partie:"},

        {jsonKeys::OPTIONS, {
            {"/max/<number> ", "Nombre maximal de joueurs"},
            {"/quit ", "Quitter la partie"}
        }},

        {jsonKeys::INPUT, "entrez le paramètre: "}
        
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLobbyMenu2(int maxPlayers, const std::string& mode, int amountOfPlayers) const {
    std::string cmd = "";
    std::string desc = "";
    if (mode == "Royal Competition"){
        cmd = "/energy/<energy> (inférieure à 15) ";
        desc = "déterminer l'énergie à atteindre";
    }

    
    json menu = {
        {jsonKeys::TITLE, "La salle d'invitation et d'attente:"},

        {jsonKeys::OPTIONS, {
            {"Game Mode: ", mode},
            {"Joueurs Maximum: ", std::to_string(maxPlayers)},
            {"Nombre actuel de joueurs: ", std::to_string(amountOfPlayers)},
            {"/invite/player/<name> ", "Inviter un joueur"},
            {"/invite/viewer/<name> ", "Inviter un spectateur"},
            {"/speed/<vitesse> (supérieure à 100) ", "déterminer la vitesse du jeu"},
            {cmd, desc},
            {"/quit ", "Quitter la partie"}

        }},

        {jsonKeys::INPUT, "entrez le paramètre: "}
        
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getGameRequestsListMenu(const std::vector<std::vector<std::string>>& pendingRequests) const {
    json menu = {
        {jsonKeys::TITLE, "Liste des demandes du rejoindre un jeu: "},
        {jsonKeys::HELP,"Tapez 'accept.<Number Room>' pour accepter une demande :    "},
        {jsonKeys::OPTIONS, json::array()},
        {jsonKeys::INPUT, "Tapez 'accept.<Number Room>' pour accepter une demande ou '/quit' pour faire un retour:    "}
    };

    int index = 1;
    for (const auto& invitation : pendingRequests) {
        menu[jsonKeys::OPTIONS].push_back(std::to_string(index) + ". " + "Vous êtes invités par '"+invitation[0]+
        "' en tant que '"+invitation[1]+"' dans la GameRoom '"+invitation[2]+"'");  
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


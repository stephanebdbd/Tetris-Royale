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

json Menu::getchatMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Menu du chat"},
        {jsonKeys::OPTIONS, {
            {"1. ", "Créer une Room"},
            {"2. ", "Rejoindre une Room"},
            {"3. ", "Lister les Rooms"},
            {"4. ", "private message"},
            {"5. ", "Retour"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getFriendMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Gestion des amis"},
        {jsonKeys::OPTIONS, {
            {"1. ", "Ajouter un ami"},
            {"2. ", "Supprimer un ami"},
            {"3. ", "Lister les amis"},
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

json Menu::getRemoveFriendMenu() const {
    json menu = {
        {jsonKeys::TITLE, "Supprimer un ami"},
        {jsonKeys::OPTIONS, {
            {"Veuillez insérer l'ID de l'ami à supprimer", ":"},
        }},
        {jsonKeys::INPUT, "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getListFriendsMenu(const std::vector<int>& friends) const {
    json menu = {
        {jsonKeys::TITLE, "Liste des amis"},
        {jsonKeys::OPTIONS, {}},
        {jsonKeys::INPUT, "Appuyez sur une touche pour revenir"}
    };

    for (const auto& friendId : friends) {
        menu[jsonKeys::OPTIONS].push_back({std::to_string(friendId)});
    }

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
            {"Joueurs Maximum: ", std::to_string(maxPlayers)},
            {"Game Mode: ", mode},
            {" "," \n"},

            {"Nombre actuel de joueurs: ", std::to_string(amountOfPlayers)},
            {" ", " \n"},

            {"\\invite\\player\\<name> ", "Inviter un joueur"},
            {"\\invite\\viewer\\<name> ", "Inviter un spectateur"},
            {"\\quit ", "Quitter la partie"}

        }},

        {jsonKeys::INPUT, "entrez le paramètre: "}
        
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}



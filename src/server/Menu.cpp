#include "Menu.hpp"
#include <iostream>


json Menu::getMainMenu0() const {
    json menu = {
        {"title", "Bienvenue dans Tetris Royal !"},
        {"options", {
            {"1. ", "Se connecter"},
            {"2. ", "Créer un compte"},
            {"3. ", "Quitter"}
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getMainMenu1() const {
    json menu = {
        {"title", "Menu principal"},
        {"options", {
            {"1. ", "Jouer"},
            {"2. ", "Amis"},
            {"3. ", "Classement"},
            {"4. ", "Chat"},
            {"5. ", "Retour"}
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getRegisterMenu1() const {
    json menu = {
        {"title", "Création de compte - Etape 1"},
        {"options", {
            {"Veuillez insérer votre pseudo", ":"},

        }},
        {"input", ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getRegisterMenuFailed() const {
    json menu = {
        {"title", "Création de compte - Etape 1"},
        {"options", {
            {"Pseudo déjà utilisé ! \n Veuillez ", ""},
            {"insérer", " votre pseudo : "},
        }},
        {"input", ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getRegisterMenu2() const {
    json menu = {
        {"title", "Création de compte - Etape 2"},
        {"options", {
            {"Veuillez insérer votre mot de passe", ":"},
        }},
        {"input", ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLoginMenu1() const {
    json menu = {
        {"title", "Connexion au compte - Etape 1"},
        {"options", {
            {"Veuillez insérer votre pseudo", ":"},

        }},
        {"input", ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLoginMenuFailed1() const {
    json menu = {
        {"title", "Connexion au compte - Etape 1"},
        {"options", {
            {"Aucun identifiant n'a été trouvé ! \n Veuillez ", ""},
            {"insérer", " votre pseudo : "},
        }},
        {"input", ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLoginMenu2() const {
    json menu = {
        {"title", "Connexion au compte  - Etape 2"},
        {"options", {
            {"Veuillez insérer votre mot de passe", ":"},

        }},
        {"input", ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getLoginMenuFailed2() const {
    json menu = {
        {"title", "Connexion au compte - Etape 2"},
        {"options", {
            {"Mot de passe incorrect ! \n Veuillez ", ""},
            {"insérer", " votre mot de passe : "},
        }},
        {"input", ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getJoinOrCreateGame() const {
    json menu = {
        {"title", "Rejoindre ou créer une partie"},
        {"options", {
            {"1. ", "créer"},
            {"2. ", "rejoindre"},
            {"3. ", "Retour"}
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getchatMenu() const {
    json menu = {
        {"title", "Menu du chat"},
        {"options", {
            {"1. ", "Créer une Room"},
            {"2. ", "Rejoindre une Room"},
            {"3. ", "Lister les Rooms"},
            {"4. ", "private message"},
            {"5. ", "Retour"},
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getFriendMenu() const {
    json menu = {
        {"title", "Gestion des amis"},
        {"options", {
            {"1. ", "Ajouter un ami"},
            {"2. ", "Supprimer un ami"},
            {"3. ", "Lister les amis"},
            {"4. ", "Retour"}
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getAddFriendMenu() const {
    json menu = {
        {"title", "Ajouter un ami"},
        {"options", {
            {"Veuillez insérer l'ID de l'ami à ajouter", ":"},
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getRemoveFriendMenu() const {
    json menu = {
        {"title", "Supprimer un ami"},
        {"options", {
            {"Veuillez insérer l'ID de l'ami à supprimer", ":"},
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getListFriendsMenu(const std::vector<int>& friends) const {
    json menu = {
        {"title", "Liste des amis"},
        {"options", {}},
        {"input", "Appuyez sur une touche pour revenir"}
    };

    for (const auto& friendId : friends) {
        menu["options"].push_back({std::to_string(friendId)});
    }

    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getRankingMenu(const std::vector<std::pair<std::string, int>>& ranking) const {
    json menu = {
        {"title", "Classement des meilleurs joueurs"},
        {"options", json::object()},  // car avec array la key commence a 0
        {"input", "Appuyez sur la touche \"1\" pour revenir au menu principal : "}
    };

    // Ajoute les 10 meilleurs joueurs
    int limit = std::min(10, static_cast<int>(ranking.size()));
    for (int i = 0; i < limit; ++i) {
        std::string key = "    " + std::to_string(i + 1) + ". ";
        std::string value = ranking[i].first + " - " + std::to_string(ranking[i].second);
        menu["options"][key] = value;
    }
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getGameOverMenu() const {
    json menu = {
        {"title", "Game Over"},
        {"options", {
            {"1. ", "Rejouer"},
            {"2. ", "Retour au menu principal"}
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


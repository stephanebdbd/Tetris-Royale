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

json Menu::getChatMenu() const {
    json menu = {
        {"title", "Menu du chat"},
        {"options", {
            {"1. ", "Créer une Room"},
            {"2. ", "Rejoindre une Room"},
            {"3. ", "Lister mes Rooms"},
            {"4. ", "chat"},
            {"5. ", "Retour"},
        }},
        {"input", "Votre choix: "}
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

json Menu::getJoinChatRoomMenu() const {
    json menu = {
        {"title", "Liste des Rooms"},
        {"options", json::array()},
        {"input", "Tapez le nom de la room à rejoindre ou './quit' pour quitter:    "}
    };

    return menu.dump() + "\n";  // Affichage formaté
}

json Menu::getListe(const std::vector<std::string>& data, std::string title, const std::string& input) const {
    json menu = {
        {"title", title},
        {"options", json::array()},
        {"input", input}
    };

    int index = 1;
    for (const auto& item : data) {
        menu["options"].push_back(std::to_string(index) + ". " + item);
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
        {"title", "Gestion de la Room"},
        {"options", {
            {"1. ", "Lister les membres"},
        }},
        {"input", "Votre choix: "}
    };
    int i;
    if(isAdmin){
        menu["options"]["2. "] = "Ajouter un admin";
        menu["options"]["3. "] = "Ajouter un membre";
        menu["options"]["4. "] = "Supprimer un membre";
        menu["options"]["5. "] = "Lister les demandes d'ajout";
        menu["options"]["6. "] = "Supprimer la Room";
        i = 7;
    }else{
        menu["options"]["2. "] = "Quitter la Room";
        i = 3;
    }
    menu["options"][std::to_string(i) + ". "] = "Retour";
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getAddDeleteMemberMenu(std::string title, std::string input) const {
    json menu = {
        {"title", title},
        {"options", {
            {"Veuillez insérer le pseudo du membre", ":"},
        }},
        {"input", input}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getAddMembreMenu() const {
    return getAddDeleteMemberMenu("Ajouter un membre", "Tapez 'add.pseudo' pour ajouter un membre ou 'quit' pour quitter:    ");
}

json Menu::getRemoveMembreMenu() const {
    return getAddDeleteMemberMenu("Supprimer un membre", "Tapez 'remove.pseudo' pour supprimer un membre ou 'quit' pour quitter:    ");
}

json Menu::getListeRequests(const std::vector<std::string>& data) const {
    return getListe(data, "Liste des demandes d'ajout", "Tapez 'accept.pseudo' ou 'reject.pseudo'  pour accepter ou refuser une demande d'amitie ou 'quit' pour quitter :    ");
}

json Menu::getQuitRoomMenu(bool isAdmin) const {
    json menu = {
        {"title", "Quitter la Room"},
        {"options", {}},
        {"input", "Votre choix: "}
    };
    if (isAdmin) {
        menu["options"]["1. "] = "Supprimer la Room";
    }else{
        menu["options"]["1. "] = "Quitter la Room";
    }
    menu["options"]["2. "] = "Retour";
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}


json Menu::getFriendMenu() const {
    json menu = {
        {"title", "Gestion des amis"},
        {"options", {
            {"1. ", "Ajouter un ami"},
            {"2. ", "liste des amis"},
            {"3. ", "liste des demandes d'amis"},
            {"4. ", "Retour"}
        }},
        {"input", "Votre choix: "}
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
        {"title", "Ajouter un ami"},
        {"options", {
            {"Veuillez insérer le pseudo de l'ami à ajouter", ":"},
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


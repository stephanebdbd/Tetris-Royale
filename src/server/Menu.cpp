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
        }},
        {"input", ""}
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


json Menu::getGameMode() const {
    json menu = {
        {"title", "Modes de jeu"},
        {"options", {
            {"1. ", "endless"},
            {"2. ", "classic"},
            {"3. ", "duel"},
            {"4. ", "royal competion"}, 
        }},
        {"input", ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}



// TODO: Ajouter les menus : creer || rejoindre && mode de jeu (endless, duel, ... )
// s'ajoute apres "jouer" de getmainmenu1
// 
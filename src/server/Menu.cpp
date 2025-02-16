#include "Menu.hpp"

json Menu::getMainMenu0() const {
    json menu = {
        {"title", "Bienvenue dans Tetris Royal !"},
        {"options", {
            {"1", "Se connecter"},
            {"2", "Créer un compte"},
            {"3", "Quitter"}
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump();  // Convertir en chaîne JSON
}

json Menu::getMainMenu1() const {
    json menu = {
        {"title", "Menu principal"},
        {"options", {
            {"1", "Jouer"},
            {"2", "Amis"},
            {"3", "Classements"},
            {"4", "Rejoindre"},
            {"5", "Retour"}
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump();  // Convertir en chaîne JSON
}
#include "Menu.hpp"
#include <iostream>

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
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getMainMenu1() const {
    json menu = {
        {"title", "Menu principal"},
        {"options", {
            {"1", "Jouer"},
            {"2", "Amis"},
            {"3", "Classement"},
            {"4", "Chat"},
            {"5", "Retour"}
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}

json Menu::getRegisterMenu() const {
    json menu = {
        {"title", "Création de compte"},
        {"options", {
            {"Veuillez insérer votre pseudo puis votre mot de passe", ":"},

        }},
        {"input", ""}
    };
    return menu.dump() + "\n";  // Convertir en chaîne JSON
}
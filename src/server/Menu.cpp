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

MenuNode::MenuNode(std::string name, MenuNode* parent) : name(name), parent(parent) {}

void MenuNode::addChild(MenuNode child) {
    children.push_back(child);
}

MenuNode MenuNode::getChild(std::string name) {
    for (MenuNode child : children) {
        if (child.getName() == name) {
            return child;
        }
    }
    return MenuNode();
}

MenuNode* MenuNode::getParent() {
    return parent;
}

std::string MenuNode::getName() {
    return name;
}
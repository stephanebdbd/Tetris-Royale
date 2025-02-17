#ifndef MENU_HPP
#define MENU_HPP

#include "../common/json.hpp"  // Inclusion de la bibliothèque JSON

using json = nlohmann::json;

class Menu {
    public:
        json getMainMenu0() const;  
        json getMainMenu1() const;  
};

class MenuNode{
    std::string name;
    std::vector<MenuNode> children;
    MenuNode* parent;
public:
    MenuNode(std::string name="", MenuNode* parent=nullptr);
    void addChild(MenuNode child);
    MenuNode getChild(std::string name);
    MenuNode* getParent();
    std::string getName();
}

#endif

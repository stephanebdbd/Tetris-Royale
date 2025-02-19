#ifndef MENU_HPP
#define MENU_HPP

#include "../common/json.hpp"  // Inclusion de la bibliothèque JSON
#include <memory>

using json = nlohmann::json;

class Menu {
    public:
        json getMainMenu0() const;  
        json getMainMenu1() const;  
};

class MenuNode : public std::enable_shared_from_this<MenuNode> {
    std::string name;
    std::vector<std::shared_ptr<MenuNode>> children;
    std::weak_ptr<MenuNode> parent;
public:
    MenuNode(std::string name, std::shared_ptr<MenuNode> parent=nullptr);
    void addChild(std::shared_ptr<MenuNode> child);
    std::shared_ptr<MenuNode> getChild(std::string name) const;
    std::shared_ptr<MenuNode> getParent() const;
    std::string getName() const;
    std::vector<std::shared_ptr<MenuNode>> getChildren() const;
    void makeNodeTree();
};

#endif
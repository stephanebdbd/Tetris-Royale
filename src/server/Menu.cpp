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
            {"3", "Classement"},
            {"4", "Chat"},
            {"5", "Retour"}
        }},
        {"input", "Votre choix: "}
    };
    return menu.dump();  // Convertir en chaîne JSON
}

MenuNode::MenuNode(std::string name, std::shared_ptr<MenuNode> parent) : name(name), parent(parent) {
    if (name == "Connexion")
        this->makeNodeTree();
}

void MenuNode::addChild(std::shared_ptr<MenuNode> child) {
    children.push_back(child);
}

std::shared_ptr<MenuNode> MenuNode::getChild(std::string name) const {
    for (auto child : children) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

std::weak_ptr<MenuNode> MenuNode::getParent() const {
    return parent;
}

std::string MenuNode::getName() const {
    return name;
}

std::vector<std::shared_ptr<MenuNode>> MenuNode::getChildren() const {
    return children;
}

void MenuNode::makeNodeTree() {
    std::shared_ptr<MenuNode> seConnecter = std::make_shared<MenuNode>("Se connecter", shared_from_this());
    std::shared_ptr<MenuNode> creerCompte = std::make_shared<MenuNode>("Créer un compte", shared_from_this());
    std::shared_ptr<MenuNode> menuPrincipal = std::make_shared<MenuNode>("Menu principal", shared_from_this());
    
    std::shared_ptr<MenuNode> amis = std::make_shared<MenuNode>("Amis");
    std::shared_ptr<MenuNode> ajouterOuSupprimer = std::make_shared<MenuNode>("Ajouter ou supprimer");
    std::shared_ptr<MenuNode> afficherAmis = std::make_shared<MenuNode>("Afficher amis");

    amis->addChild(ajouterOuSupprimer);
    amis->addChild(afficherAmis);

    std::shared_ptr<MenuNode> classement = std::make_shared<MenuNode>("Classement");
    std::shared_ptr<MenuNode> chat = std::make_shared<MenuNode>("Chat");

    std::shared_ptr<MenuNode> jouer = std::make_shared<MenuNode>("Jouer");
    std::shared_ptr<MenuNode> rejoindre = std::make_shared<MenuNode>("Rejoindre");
    std::shared_ptr<MenuNode> creer = std::make_shared<MenuNode>("Créer");
    std::shared_ptr<MenuNode> endless = std::make_shared<MenuNode>("Endless");
    std::shared_ptr<MenuNode> classic = std::make_shared<MenuNode>("Classic");
    std::shared_ptr<MenuNode> duel = std::make_shared<MenuNode>("Duel");
    std::shared_ptr<MenuNode> royalCompetition = std::make_shared<MenuNode>("Royal Competition");

    creer->addChild(endless);
    creer->addChild(classic);
    creer->addChild(duel);
    creer->addChild(royalCompetition);

    jouer->addChild(creer);
    jouer->addChild(rejoindre);

    menuPrincipal->addChild(jouer);
    menuPrincipal->addChild(amis);
    menuPrincipal->addChild(classement);
    menuPrincipal->addChild(chat);

    seConnecter->addChild(menuPrincipal);
    creerCompte->addChild(menuPrincipal);
}

void MenuNode::menutest(std::shared_ptr<MenuNode> root, int depth) {
    // Afficher le nom du nœud avec une indentation basée sur la profondeur
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
    std::cout << root->getName() << std::endl;

    // Parcourir les enfants du nœud et appeler récursivement menutest
    for (auto child : root->getChildren()) {
        menutest(child, depth + 1);
    }
}
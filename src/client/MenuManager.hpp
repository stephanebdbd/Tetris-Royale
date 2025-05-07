#ifndef MENUMANAGER_HPP
#define MENUMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>

#include "Button.hpp"           
#include "Text.hpp"
#include "Client.hpp"
#include "ClientNetwork.hpp"
#include "Textures.hpp"          
#include "SFMLGame.hpp"          
#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"
#include "../common/MenuState.hpp"
#include "../common/state.hpp"
#include "AvatarManager.hpp" 


class SFMLGame;

class MenuManager {
    sf::RenderWindow* window;
    sf::Font& font;
    Client& client;
    ClientNetwork& network;
    Textures* textures;
    SFMLGame* sfmlGame;
    AvatarManager* avatarManager;

    std::map<ButtonKey, std::unique_ptr<Button>>* buttons;
    std::map<TextFieldKey, std::unique_ptr<TextField>>* texts;
    std::map<std::string, std::unique_ptr<Button>> friendButtons;
    std::string selectedFriend;


public:
    MenuManager(sf::RenderWindow* window, sf::Font& font, Client& client, ClientNetwork& network, 
                Textures& textures, SFMLGame* sfmlGame, AvatarManager& avatarManager, std::map<ButtonKey, 
                std::unique_ptr<Button>>& buttons, std::map<TextFieldKey, std::unique_ptr<TextField>>& texts);

    void welcomeMenu();

    void connexionMenu();
    void registerMenu();
    
    void mainMenu();

    void friendsMenu();

    void rankingMenu();
    void teamsMenu();
    
    void drawButtons();

    void handleTextFieldEvents(sf::Event& event);

};

#endif 
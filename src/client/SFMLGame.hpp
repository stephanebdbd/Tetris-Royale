#ifndef SFMLGAME_HPP
#define SFMLGAME_HPP

#include "Client.hpp"
#include "ClientDisplay.hpp"  
#include "Controller.hpp"
#include "ClientNetwork.hpp"
#include "ClientChat.hpp"
#include "Button.hpp"
#include "Text.hpp"
#include "../common/MenuState.hpp"
#include <memory>
#include <vector>


struct Textures{
    sf::Texture connexion;
    sf::Texture grid;
    sf::Texture game;
    sf::Texture gameOver;
    sf::Texture gameVictory;
    sf::Texture gameLost;
    sf::Texture gamePause;
    sf::Texture settings;
    sf::Texture chat;
    sf::Texture notification;
    sf::Texture logoConnexion;
    sf::Texture logoNotification;
    sf::Texture logoSettings;
    sf::Texture logoTeams;
    sf::Texture logoRanking;
    sf::Texture logoChat;
    sf::Texture logoMain;
    sf::Texture logoExit;
};


class SFMLGame {

    private:
        std::vector<std::string> contacts = {"Alice", "Bob", "Charlie", "David", "Eve"};

        Client& client;
        std::unique_ptr<sf::RenderWindow> window;
        std::unique_ptr<ClientNetwork> network;
        std::unique_ptr<Textures> textures;
        std::vector<std::unique_ptr<Button>> buttons;
        std::vector<std::unique_ptr<TextField>> texts;
        sf::Font font; 
        MenuState currentState;

        
        //draw
        void drawButtons();
        void drawTextFields();

        //handle events
        void handleTextFieldEvents(sf::Event& event);
        void handleButtonEvents();
        void handleEvents();
        void LoadResources();
        void cleanup();

    public:
        SFMLGame(Client& client);
        ~SFMLGame();

        void run();
        void refreshMenu();
        void displayBackground(sf::Texture& texture);

        // Welcome Menu
        void welcomeMenu();

        // Main Menu
        void mainMenu();

        // Game Menu
        void gameMenu();
        void gameOverMenu();
        void gameVictoryMenu();
        void gameLostMenu();
        void gamePauseMenu();
        void settingsMenu();

        //Grid Menu
        void displayGrid();
        void displayPiece();
        void displayNextPiece();
        void displayScoreBoard();
        void displayPiecesFinalPosition();

        // Chat Menu
        void chatMenu(const std::vector<std::string>& contacts);
        void ChatRoomMenu();

        //notification
        void notificationMenu(const std::vector<std::string>& notifications);
};

#endif // SFMLGAME_HPP
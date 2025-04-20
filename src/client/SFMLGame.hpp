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
#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"
#include <memory>
#include <vector>
#include "../common/GameState.hpp"
#include "../common/state.hpp"

//resources path
const std::string FONT_PATH = "../../res/fonts/Arial.ttf";
const std::string LogoBackGround = "../../res/background/logo.png";
const std::string ConnexionBackGround = "../../res/background/connexion.png";
const std::string MainMenuBackGround = "../../res/background/main_menu.png";
const std::string GameMenuBackGround = "../../res/background/game_menu.png";
const std::string GameOverBackGround = "../../res/background/game_over.png";
const std::string GameVictoryBackGround = "../../res/background/game_victory.png";
const std::string GameLostBackGround = "../../res/background/game_lost.png";
const std::string GamePauseBackGround = "../../res/background/game_pause.png";
const std::string SettingsBackGround = GamePauseBackGround;
const std::string GridBackGround = "../../res/background/grid.png";
const std::string ChatBackGround = "../../res/background/chat.png";
const std::string NotificationBackGround = "../../res/background/notification.png";
const std::string LogoNotification = "../../res/logo/notification.png";
const std::string LogoSettings = "../../res/logo/settings.png";
const std::string LogoTeams = "../../res/logo/teams.png";
const std::string LogoRanking = "../../res/logo/ranking.png";
const std::string LogoChat = "../../res/logo/chat.png";
const std::string LogoMain = "../../res/logo/main.png";
const std::string LogoExit = "../../res/logo/exit.png";
const std::string LogoFriend = "../../res/logo/friend.png";
const std::string LogoAddFriend = "../../res/logo/addFriend.png";
const std::string LogoFrindsRequest = "../../res/logo/friendRequest.png";


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
    sf::Texture logoAddFriend;
    sf::Texture logoFrindsRequest;
};


class SFMLGame {

    private:

        Client& client;
        std::unique_ptr<sf::RenderWindow> window;
        std::unique_ptr<ClientNetwork> network;
        std::unique_ptr<Textures> textures;
        //std::vector<std::unique_ptr<Button>> buttons;
        //std::vector<std::unique_ptr<TextField>> texts;
        std::map<ButtonKey, std::unique_ptr<Button>> buttons;
        std::map<TextFieldKey, std::unique_ptr<TextField>> texts;
        
        std::vector<std::pair<std::string, std::string>> messages;
        std::vector<std::string> contacts;
        sf::Font font;
        MenuState currentState;
        std::string contact;
        float scrollSpeed = 0.5f; // Vitesse de défilement
        float friendsListOffset = 0.0f; // Décalage vertical pour la liste des amis
        float chatContactsOffset = 0.0f; // Décalage vertical pour les contacts du chat
        int MessagesY = 60; // Position Y pour afficher les messages
        int MessagesSentX = 400; // Position X pour afficher les messages
        int MessagesReceivedX = 220; // Position X pour afficher les messages
        int cellSize = 30;

        
        //draw
        void drawButtons();
        void drawTextFields();
        void drawMessages();

        //handle events
        void handleTextFieldEvents(sf::Event& event);
        void handleButtonEvents();
        void handleEvents();
        void LoadResources();
        void cleanup();

        //send json to server
        bool sendJsonToServer(json& j, const std::string& action) {
            j["action"] = action;
            return network->sendData(j.dump() + "\n", client.getClientSocket());
            cleanup();
        }

    public:
        SFMLGame(Client& client);
        ~SFMLGame() = default;

        void run();
        void refreshMenu();
        void displayBackground(sf::Texture& texture);

        // Welcome Menu
        void welcomeMenu();

        // Main Menu
        void mainMenu();
        void registerMenu();

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
        void chatMenu();
        void displayMessage(const std::string&sender, const std::string& message);
        void ChatRoomMenu();

        //friends
        void friendsMenu();
        void addFriendMenu();
        void friendRequestListMenu();

        //notification
        void notificationMenu(const std::vector<std::string>& notifications);
        void drawGrid(const json& grid);
    
        void displayGame();


        void drawTetramino(const json& tetraPiece);
        sf::Color fromShapeSymbolSFML(const std::string& symbol);
        sf::Color fromSFML(int value);
        void drawScore(const json& scoreData);
        void CreateOrJoinGame();
        void ChoiceGameMode();
        void drawEndGame(const json& endGameData);

};

#endif // SFMLGAME_HPP

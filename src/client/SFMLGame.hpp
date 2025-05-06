#ifndef SFMLGAME_HPP
#define SFMLGAME_HPP

#include "Client.hpp"
#include "ClientDisplay.hpp"  
#include "Controller.hpp"
#include "ClientNetwork.hpp"
#include "ClientChat.hpp"
#include "MenuManager.hpp"
#include "AvatarManager.hpp"
#include "Textures.hpp"
#include "Button.hpp"
#include "Text.hpp"
#include "../common/MenuState.hpp"
#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"
#include <memory>
#include <vector>
#include "../common/GameState.hpp"
#include "../common/state.hpp"


class MenuManager; // TODO: supprimer ca

class SFMLGame {
    private:

        Client& client;
        std::unique_ptr<sf::RenderWindow> window;
        std::unique_ptr<ClientNetwork> network;
        std::unique_ptr<Textures> textures;
        std::map<ButtonKey, std::unique_ptr<Button>> buttons;
        std::map<TextFieldKey, std::unique_ptr<TextField>> texts;
        std::map<std::string, std::unique_ptr<Button>> chatContacts;
        std::vector<json> messages;
        std::vector<std::string> contacts;
        std::vector<std::string> friends;
        std::map<std::string, std::vector<std::string>> ranking;
        sf::Font font;
        MenuState currentState;
        std::unique_ptr<AvatarManager> avatarManager;
        std::unique_ptr<MenuManager> menuManager;
        std::string clickedContact;
        float scrollSpeed = 0.5f; // Vitesse de défilement
        float friendsListOffset = 0.0f; // Décalage vertical pour la liste des amis
        float chatContactsOffset = 0.0f; // Décalage vertical pour les contacts du chat
        int MessagesY = 60; // Position Y pour afficher les messages
        int MessagesSentX = 400; // Position X pour afficher les messages
        int MessagesReceivedX = 220; // Position X pour afficher les messages
        int cellSize = 30;
        sf::Clock erreurClock;
        bool afficherErreurActive = false;
        std::string erreurMessageActuel;
        std::unordered_map<std::string, std::unique_ptr<Button>> friendButtons;
        std::vector<std::string> amis;
        std::string selectedFriend;
    

        //draw
        void drawFriends();
        void displayErrorMessage(const std::string& message) ;
        void friendListMenu() ;

        //handle events
        void handleButtonEvents();
        void handleEvents();

        //send json to server
        bool sendJsonToServer(json& j, const std::string& action) {
            j["action"] = action;
            return network->sendData(j.dump() + "\n", client.getClientSocket());
            cleanup();
        }
        bool duel = false;
        bool classic = false;
        bool royale = false;
        std::vector<sf::Vector2f> miniGridPositions = {
            {550.f, 0.f}, {550.f, 320.f}, {720.f, 0.f}, {720.f, 320.f},
            {890.f, 0.f}, {890.f, 320.f}, {1060.f, 0.f}, {1060.f, 320.f},

        };
        std::map<std::string, std::vector<std::unique_ptr<Button>>> inviteFriends;
        //std::map<std::string, std::vector<std::unique_ptr<Button>>> acceptInvite;
        std::map<std::string, std::unique_ptr<Button>> acceptInvite;
        std::map<std::string,  std::vector<std::string>> pseudos = {};
        std::vector<std::string> friendsLobby = {};
        std::unique_ptr<Button> quitter = std::make_unique<Button>("Quitter", font, 24, sf::Color::White, sf::Color::Red, sf::Vector2f(200, 500), sf::Vector2f(150, 50));
        //std::map<std::string, std::unique_ptr<Button>> inviteFriends;
        bool invite = false;
        bool cl = false;
        //std::vector<std::string> pseudos;
        std::vector<std::vector<bool>> isInvite;
        // Variables globales ou membres de la classe
        sf::View inviteView; // Vue pour le rectangle défilable
        float inviteScrollOffset = 0.0f; // Décalage vertical pour le défilement
        const float inviteScrollSpeed = 30.0f; // Vitesse de défilement
        float inviteMaxScroll = 500.0f; // Hauteur maximale du contenu défilable

        bool showCommand = true;
        bool showInviteCommand = true;

    public:
        SFMLGame(Client& client);
        ~SFMLGame() = default;
        void update() ;

        void run();
        void refreshMenu();
        void displayBackground(sf::Texture& texture);

        // Welcome Menu
        void registerMenu();
        void handleResize(unsigned int newWidth, unsigned int newHeight) ;

        // Main Menu
        void createRoomMenu();

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
        std::vector<std::string> getContactsContainElement(const std::string& element);
        void handleContacts();
        void getMessagesFromServer();
        void drawMessages();
        void drawContacts();
        void displayMessage(const std::string&sender, const std::string& message);
        std::string wrapText(const std::string& text, const sf::Font& font, unsigned int characterSize, float maxWidth);
        void joinTeamMenu();

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
        void drawEndGame();
        void displayWaitingRoom();
        void displayJoinGame();
        void drawMessageMalusBonus(const json& msg);
        void drawAvatar(int avatarIndex, float posX, float posY, float size);

        void drawMiniGrid(const json& miniGrid, sf::Vector2f pos);
        void drawMiniTetra(const json& miniTetra, sf::Vector2f pos);
        void displayCurrentPlayerInfo();
        





        //TODO: remettre en privé ou dans menu manager 
        void drawButtons();
        void cleanup();
        void drawErreurMessage();
        void drawTextFields();
        void afficherErreur(const std::string& message);

};

#endif // 


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
    Client& client;
    std::unique_ptr<sf::RenderWindow> window;
    std::unique_ptr<ClientNetwork> network;
    std::unique_ptr<Textures> textures;
    std::map<ButtonKey, std::unique_ptr<Button>> buttons;
    std::map<TextFieldKey, std::unique_ptr<TextField>> texts;
    std::map<std::string, std::unique_ptr<Button>> chatContacts;
    std::vector<json> messages;
    std::vector<std::string> contacts;
    std::vector<std::string> contactsToShow;
    std::vector<std::string> friends;
    std::vector<std::string> friendsToShow;
    std::map<std::string, std::vector<std::string>> ranking;
    sf::Font font;
    MenuState currentState;
    MenuState previousState;
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
    std::unordered_map<std::string, std::unique_ptr<Button>> TEAMSbuttons;
    std::string selectedTeam;
    
    bool duel = false;
    bool classic = false;
    bool royale = false;
    std::vector<sf::Vector2f> miniGridPositions = {
        {550.f, 0.f}, {550.f, 320.f}, {720.f, 0.f}, {720.f, 320.f},
        {890.f, 0.f}, {890.f, 320.f}, {1060.f, 0.f}, {1060.f, 320.f},
    };
    std::map<std::string, std::vector<std::unique_ptr<Button>>> inviteFriends;
    std::map<std::string, std::unique_ptr<Button>> acceptInvite;
    std::map<std::string,  std::vector<std::string>> pseudos = {};
    std::vector<std::string> friendsLobby = {};
    std::unique_ptr<Button> quitter = std::make_unique<Button>("Quitter", font, 24, sf::Color::White, sf::Color::Red, sf::Vector2f(50, 500), sf::Vector2f(150, 50));
    bool invite = false;
    bool cl = false;
    std::vector<std::vector<bool>> isInvite;
    
    // Variables globales ou membres de la classe
    sf::View inviteView; // Vue pour le rectangle défilable
    float inviteScrollOffset = 0.0f; // Décalage vertical pour le défilement
    const float inviteScrollSpeed = 30.0f; // Vitesse de défilement
    float inviteMaxScroll = 500.0f; // Hauteur maximale du contenu défilable
    
    bool showCommand = true;
    bool showInviteCommand = true;
    
    //handle events
    void handleButtonEvents();
    void handleEvents();
    
    public:
        SFMLGame(Client& client);
        ~SFMLGame() = default;
        void update() ;

        // Draw functions
        void drawButtons();
        void cleanup();
        void drawErreurMessage();
        void drawTextFields();
        void afficherErreur(const std::string& message);

        // Main loop
        void run();
        void refreshMenu();
        void displayBackground(sf::Texture& texture);
        void handleResize(unsigned int newWidth, unsigned int newHeight) ;

        // Teams Menu
        void manageTeamMenu();

        // Chat Menu
        std::vector<std::string> getContactsContainElement(const std::string& element);
        void handleContacts();
        void getMessagesFromServer();
        void drawMessages();
        void drawContacts();
        void addMessage(const json& msg);
        void displayMessage(const std::string&sender, const std::string& message);
        std::string wrapText(const std::string& text, const sf::Font& font, unsigned int characterSize, float maxWidth);


        //Game Menu
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
        void drawPlayerNumber(const json& playerNumber);
        void drawminiPlayerId(const json& playerId, sf::Vector2f pos);

        void resetAcceptInvite(); 
        MenuState getPreviousState() const;
        std::vector<json> getMessages() const;
        std::string getClickedContact() const;
        std::unordered_map<std::string, std::unique_ptr<Button>>& getTEAMSbuttons();


        void setSelectedTeam(const std::string& team); 
        void clearSelectedTeam();
        std::string getSelectedTeam() const;
};

#endif
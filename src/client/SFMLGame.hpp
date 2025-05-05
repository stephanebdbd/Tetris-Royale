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
const std::string RankingBackGround = "../../res/background/ranking.png";
const std::string GameMenuBackGround = "../../res/background/game_menu.png";
const std::string GridBackGround = "../../res/background/grid.png";
const std::string ChatBackGround = "../../res/background/chat.png";
const std::string NotificationBackGround = "../../res/background/notification.png";
const std::string LogoNotification = "../../res/logo/notification.png";
const std::string LogoSettings = "../../res/logo/settings.png";
const std::string LogoTeams = "../../res/logo/teams.png";
const std::string LogoRanking = "../../res/logo/ranking.png";
const std::string LogoMain = "../../res/logo/main.png";
const std::string LogoExit = "../../res/logo/exit.png";
const std::string LogoFriend = "../../res/logo/friend.png";
const std::string LogoAddFriend = "../../res/logo/addFriend.png";
const std::string backgroundMode = "../../res/background/mode.png";
const std::string LogoRemote = "../../res/logo/remote.png";
const std::string LogoViewer = "../../res/logo/viewer.png";
const std::string LogoPLus = "../../res/logo/plus.png";
const std::string LogoEsc = "../../res/logo/esc.png";
const std::string LogoP_O = "../../res/logo/inviteP_O.png";
const std::string LogoThreePoint = "../../res/logo/dots.png";
const std::string LogoAccept = "../../res/logo/accept.png";
const std::string backgroundRejoindre = "../../res/background/rejoindre.png";


struct Textures{
    sf::Texture connexion, grid, game, settings, 
                chat, ranking, logoConnexion,logoNotification,
                logoSettings, logoTeams, logoRanking,
                logoChat,logoMain,logoExit,logoAddFriend,
                logoFrindsRequest, mode, player, viewer, 
                plus, esc, P_O, playerClicked, accept, rejoindre;

    Textures() = default;
    ~Textures() = default;

    void loadTexture(sf::Texture& texture, const std::string& filePath) {
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Erreur: Impossible de charger la texture depuis " << filePath << std::endl;
        }
    }
    void loadTextures() {
        loadTexture(connexion, ConnexionBackGround);
        //loadTexture(grid, GridBackGround);
        //loadTexture(game, GameMenuBackGround);
        loadTexture(chat, ChatBackGround);
        loadTexture(ranking, RankingBackGround);
        loadTexture(logoConnexion, LogoBackGround);
        loadTexture(logoNotification, LogoNotification);
        loadTexture(logoSettings, LogoSettings);
        loadTexture(logoTeams, LogoTeams);
        loadTexture(logoRanking, LogoRanking);
        loadTexture(logoMain, LogoMain);
        loadTexture(logoExit, LogoExit);
        loadTexture(logoAddFriend, LogoAddFriend);
        loadTexture(mode, backgroundMode);
        loadTexture(player, LogoRemote);
        loadTexture(viewer, LogoViewer);
        loadTexture(plus, LogoPLus);
        loadTexture(esc, LogoEsc);
        loadTexture(P_O, LogoP_O);
        loadTexture(playerClicked, LogoThreePoint);
        loadTexture(accept, LogoAccept);
        loadTexture(rejoindre, backgroundRejoindre);
    }
};


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
        

        void afficherErreur(const std::string& message) ;
        void drawErreurMessage() ;

        //draw
        void drawButtons();
        void drawTextFields();
        void drawFriends();
        void displayErrorMessage(const std::string& message) ;
        void friendListMenu() ;

        //handle events
        void handleTextFieldEvents(sf::Event& event);
        void handleButtonEvents();
        void handleEvents();
        void cleanup();

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
        int selectedAvatar = 0; // 0 par défaut ou -1 si non sélectionné
        int avatarClickedContact; // avatar du client selection pour chatter
        std::vector<std::string> avatarPaths; // Chemins des avatars disponibles
        sf::Texture avatarduClient; // Texture pour l'avatar du client
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
        void welcomeMenu();
        void connexionMenu() ;
        void registerMenu();
        void handleResize(unsigned int newWidth, unsigned int newHeight) ;

        // Main Menu
        void mainMenu();
        void teamsMenu();

        //classement
        void rankingMenu();

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
        
        

};

#endif // 


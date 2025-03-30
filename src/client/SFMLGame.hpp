#ifndef SFMLGAME_HPP
#define SFMLGAME_HPP

#include "Client.hpp"
#include "ClientDisplay.hpp"  
#include "Controller.hpp"
#include "ClientNetwork.hpp"
#include "ClientChat.hpp"
#include "Button.hpp"
#include "Text.hpp"

#include <memory>
#include <vector>

enum class GameState {
    Welcome,
    MainMenu,
    Game,
    GameOver,
    GameVictory,
    GameLost,
    Pause,
    Settings,
    Chat
};

struct Textures {
    sf::Texture connexion;
    sf::Texture mainMenu;
    sf::Texture gameMenu;
    sf::Texture gameOver;
    sf::Texture gameVictory;
    sf::Texture gameLost;
    sf::Texture gamePause;
    sf::Texture settings;
    sf::Texture grid;
    sf::Texture chat;
};

struct Fonts {
    sf::Font font;
};

class SFMLGame {
    Client& client;
    std::shared_ptr<sf::RenderWindow> window;
    std::shared_ptr<Textures> textures;
    std::shared_ptr<Fonts> fonts;
    std::vector<std::shared_ptr<Button>> buttons;
    std::vector<std::shared_ptr<TextField>> texts;
    GameState currentState;
    

    //draw
    void drawButtons();
    void drawTextFields();

    //handle events
    void handleTextFieldEvents(sf::Event& event);
    void handleButtonEvents(sf::Event& event);
    void handleEvents();
    void LoadResources();
    void cleanup();

    public:
        SFMLGame(Client& client);
        ~SFMLGame() = default;

        void run();
        void displayBackground(const std::string& backgroundPath);

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
        void chatMenu();
        void ChatRoomMenu();
};

#endif // SFMLGAME_HPP
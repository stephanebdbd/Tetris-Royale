#include "SFMLGame.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>


//window size
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const std::string WINDOW_TITLE = "Tetris Royal - Graphique";

//resources path
const std::string FONT_PATH = "../../res/fonts/Arial.ttf";
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

SFMLGame::SFMLGame(Client& client) : 
    client(client),
    window(std::make_shared<sf::RenderWindow>()), 
    textures(std::make_shared<Textures>()), 
    fonts(std::make_shared<Fonts>()) 
    {
        LoadResources();
    }

void SFMLGame::drawButtons() {
    for (const auto& button : buttons) {
        button->draw(*window);
    }
}

void SFMLGame::drawTextFields() {
    for (const auto& text : texts) {
        text->draw(*window);
    }
}

void SFMLGame::handleTextFieldEvents(sf::Event& event) {
    for (const auto& text : texts) {
        text->handleInput(event);
    }
}
void SFMLGame::handleButtonEvents(sf::Event& event) {
    for (const auto& button : buttons) {
        if(button->isMouseOver(*window)){
            button->setBackgroundColor();
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    //button->onClick();
                }
            }
        }else{
            button->resetColor();
        }
    }
}

void SFMLGame::run() {
    if (!client.connect()) {
        std::cerr << "Erreur: Impossible de se connecter au serveur." << std::endl;
        return;
    }
    window->create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
    window->setFramerateLimit(60);

    std::thread inputThread(&Client::handleUserInput, &client);
    inputThread.detach();
    
    while (window->isOpen()) {
        handleEvents();

        window->clear(sf::Color::Black);
        switch(currentState) {
            case GameState::Welcome:
                welcomeMenu();
                break;
            case GameState::MainMenu:
                //mainMenu();
                break;
            // Other states...
            default:
                break;
        }
        window->display();
    }
    cleanup();
}

void SFMLGame::cleanup() {
    buttons.clear();
    texts.clear();
}

void SFMLGame::LoadResources() {
    // Load fonts
    if(!fonts->font.loadFromFile(FONT_PATH)) {
        std::cerr << "Erreur: Impossible de charger la police." << std::endl;
    }
    // Load textures
    if (!textures->connexion.loadFromFile(ConnexionBackGround)) {
        std::cerr << "Erreur: Impossible de charger l'image de fond de connexion." << std::endl;
    }
    if (!textures->mainMenu.loadFromFile(MainMenuBackGround)) {
        std::cerr << "Erreur: Impossible de charger l'image de fond du menu principal." << std::endl;
    }
    if (!textures->gameMenu.loadFromFile(GameMenuBackGround)) {
        std::cerr << "Erreur: Impossible de charger l'image de fond du menu de jeu." << std::endl;
    }
    if (!textures->gameOver.loadFromFile(GameOverBackGround)) {
        std::cerr << "Erreur: Impossible de charger l'image de fond du menu de fin de jeu." << std::endl;
    }
    if (!textures->gameVictory.loadFromFile(GameVictoryBackGround)) {
        std::cerr << "Erreur: Impossible de charger l'image de fond du menu de victoire." << std::endl;
    }
    if (!textures->gameLost.loadFromFile(GameLostBackGround)) {
        std::cerr << "Erreur: Impossible de charger l'image de fond du menu de défaite." << std::endl;
    }
    if (!textures->gamePause.loadFromFile(GamePauseBackGround)) {
        std::cerr << "Erreur: Impossible de charger l'image de fond du menu de pause." << std::endl;
    }
    if (!textures->settings.loadFromFile(GamePauseBackGround)) {
        std::cerr << "Erreur: Impossible de charger l'image de fond du menu des paramètres." << std::endl;
    }
    if (!textures->grid.loadFromFile(GridBackGround)) {
        std::cerr << "Erreur: Impossible de charger l'image de fond de la grille." << std::endl;
    }
    if (!textures->chat.loadFromFile(ChatBackGround)) {
        std::cerr << "Erreur: Impossible de charger l'image de fond du chat." << std::endl;
    }
}

void SFMLGame::displayBackground(const std::string& backgroundPath) {
    sf::Texture texture;
    if (!texture.loadFromFile(backgroundPath)) {
        std::cerr << "Erreur: Impossible de charger l'image de fond." << std::endl;
        return;
    }
    
    texture.setSmooth(true);
    texture.setRepeated(true);
    
    sf::Sprite sprite(texture);
    
    // Calculer le ratio de redimensionnement
    sf::Vector2u textureSize = texture.getSize();
    sf::Vector2u windowSize = window->getSize();
    
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    
    // Appliquer le redimensionnement
    sprite.setScale(scaleX, scaleY);
    
    window->draw(sprite);
}

void SFMLGame::handleEvents() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            cleanup();
            window->close();
        }

        // Gestion prioritaire des clics sur les TextField
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            bool clickedOnTextField = false;
            for (const auto& text : texts) {
                if (text->isMouseOver(*window)) {
                    text->setActive(true);
                    clickedOnTextField = true;
                } else {
                    text->setActive(false);
                }
            }
            // Ne traitez pas les boutons si un TextField est cliqué
            if (!clickedOnTextField) {
                handleButtonEvents(event);
            }
        }

        // Gestion de la saisie texte
        if (event.type == sf::Event::TextEntered) {
            handleTextFieldEvents(event);
        }
    }
}

void SFMLGame::welcomeMenu() {
    // Display the welcome menu background
    displayBackground(ConnexionBackGround);
    if(buttons.empty()){
        // Création des boutons
        Button loginButton("Connexion", fonts->font, 24, sf::Color::White, sf::Color(70, 70, 200), 
                        sf::Vector2f(190, 500), sf::Vector2f(200, 35));
        Button signupButton("Inscription", fonts->font, 24, sf::Color::White, sf::Color(70, 200, 70), 
                        sf::Vector2f(410, 500), sf::Vector2f(200, 35));
        Button quitButton("Quitter", fonts->font, 24, sf::Color::White, sf::Color(200, 70, 70), 
                        sf::Vector2f(300, 560), sf::Vector2f(200, 35));
        // Ajout des boutons au vecteur
        buttons.emplace_back(std::make_shared<Button>(loginButton));
        buttons.emplace_back(std::make_shared<Button>(signupButton));
        buttons.emplace_back(std::make_shared<Button>(quitButton));
    }
    if(texts.empty()){
        // Création des champs de texte
        TextField usernameField(fonts->font, 24, sf::Color::Black, sf::Color::White, 
            sf::Vector2f(300, 400), sf::Vector2f(200, 35), "Username");
        TextField passwordField(fonts->font, 24, sf::Color::Black, sf::Color::White, 
            sf::Vector2f(300, 440), sf::Vector2f(200, 35), "Password");
        // Ajout des champs de texte au vecteur
        texts.emplace_back(std::make_shared<TextField>(usernameField));
        texts.emplace_back(std::make_shared<TextField>(passwordField));        
    }
    
    //draw buttons
    drawButtons();
    //draw text fields
    drawTextFields();
    
}

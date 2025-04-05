#include "SFMLGame.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>


//window size
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const std::string WINDOW_TITLE = "Tetris Royal";

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


SFMLGame::SFMLGame(Client& client) : 
    client(client),
    window(std::make_unique<sf::RenderWindow>()), 
    network(std::make_unique<ClientNetwork>()),
    textures(std::make_unique<Textures>()),
    currentState(MenuState::Welcome)
    {   
        std::cout << "SFMLGame constructor called" << std::endl;
        // Load font
        if (!font.loadFromFile(FONT_PATH)) {
            std::cerr << "Erreur: Impossible de charger la police." << std::endl;
        }

        // Load textures
        LoadResources();
    }

SFMLGame::~SFMLGame() {
    cleanup();
    std::cout << "SFMLGame destructor called" << std::endl;
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
void SFMLGame::handleButtonEvents() {
    for (const auto& button : buttons) {
        button->update();
        button->setBackgroundColor(*window);
    }
}

void SFMLGame::LoadResources() {
    //map  texture->filePath
    const std::map<sf::Texture*, std::string> textureFiles = {
        {&textures->logoConnexion, LogoBackGround},
        {&textures->connexion, ConnexionBackGround},
        //{&textures->grid, GridBackGround},
        //{&textures->game, GameMenuBackGround},
        //{&textures->gameOver, GameOverBackGround},
        //{&textures->gameVictory, GameVictoryBackGround},
        //{&textures->gameLost, GameLostBackGround},
        //{&textures->gamePause, GamePauseBackGround},
        //{&textures->settings, SettingsBackGround},
        {&textures->chat, ChatBackGround},
        {&textures->notification, NotificationBackGround},
        {&textures->logoChat, LogoChat},
        {&textures->logoNotification, LogoNotification},
        {&textures->logoTeams, LogoTeams},
        {&textures->logoRanking, LogoRanking},
        {&textures->logoSettings, LogoSettings},
        {&textures->logoMain, LogoMain},
        {&textures->logoExit, LogoExit}

    };
    for (const auto& [texture, filePath] : textureFiles) {
        if (!texture->loadFromFile(filePath)) {
            std::cerr << "Erreur: Impossible de charger l'image " << filePath << "." << std::endl;
        }
    }

}

void SFMLGame::handleEvents() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            cleanup();
            window->close();
            return;
        }

        // Gestion prioritaire des clics sur les TextField
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            for (const auto& text : texts) {
                if (text->isMouseOver(*window)) {
                    text->setActive(true);
                } else {
                    text->setActive(false);
                }
            }
            
        }
        // Gestion des événements de la souris pour les boutons
        handleButtonEvents();
        // Gestion de la saisie texte
        if (event.type == sf::Event::TextEntered) {
            handleTextFieldEvents(event);
        }
    }
}

void SFMLGame::refreshMenu() {
    auto newState = client.getCurrentMenuState();
    if(newState != currentState) {
        currentState = newState;
    }
    
    switch (currentState) {
        case MenuState::Welcome:
            welcomeMenu();
            break;
        case MenuState::Main:
            mainMenu();
            break;
        case MenuState::classement:
            //rankingMenu();
            break;
        case MenuState::Settings:
            //settingsMenu();
            break;
        case MenuState::Notifications:
            //notificationsMenu();
            break;
        case MenuState::Friends:
            //friendsMenu();
            break;
        case MenuState::AddFriend:
            //addFriendMenu();
            break;
        case MenuState::FriendRequestList:
            //friendRequestListMenu();
            break;
        case MenuState::chat:
            chatMenu(contacts);
            break;
        case MenuState::ManageRoom:
            //teamsMenu();
            break;
        case MenuState::CreateRoom:
            //createRoomMenu();
            break;
        case MenuState::Game:
            //gameMenu();
            break;
        case MenuState::Pause:
            //gamePauseMenu();
            break;
        default:
            break;
    }
}


// Compléter le switch dans run()
void SFMLGame::run() {

    if (!client.connect()) {
        std::cerr << "Erreur: Impossible de se connecter au serveur." << std::endl;
        return;
    }

    // creer la fenêtre
    window->create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
    window->setFramerateLimit(60);

    // Afficher le logo pendant 3 secondes
    //displayBackground(LogoBackGround);
    //sf::sleep(sf::seconds(3));

    // lancer le thread d'entrée utilisateur
    std::thread inputThread(&Client::handleUserInput, client);
    inputThread.detach();

    //lancer le thread de reception
    std::thread receiveThread([this]() { client.receiveDisplay(); });
    receiveThread.detach();

    while (window->isOpen()) {
        
        handleEvents();

        window->clear();
        this->refreshMenu();
        window->display();
    }


    // Attendre la fin du thread d'entrée utilisateur
    if (inputThread.joinable()) {
        inputThread.join();
    }
    // Attendre la fin du thread de réception   
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
    //cleanup
    cleanup();
}


void SFMLGame::cleanup() {
    buttons.clear();
    texts.clear();
}

void SFMLGame::displayBackground(sf::Texture& texture) {

    texture.setSmooth(true); // Activer le lissage pour une meilleure qualité d'image
    // Créer un sprite pour afficher l'image de fond
    sf::Sprite sprite(texture);
    
    // Calculer le ratio de redimensionnement
    sf::Vector2u textureSize = texture.getSize();
    sf::Vector2u windowSize = window->getSize();
    
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    
    // Appliquer le redimensionnement
    sprite.setScale(scaleX, scaleY);

    // Afficher l'image de fond
    window->draw(sprite);
}


void SFMLGame::welcomeMenu() {

    // Display the welcome menu background
    displayBackground(textures->connexion);

    //ajouter les boutons et les champs de texte si ils n'existent pas
    if(buttons.empty()){
        // Création des boutons avec des couleurs harmonisées
        Button loginButton("Login", font, 24, sf::Color::White, sf::Color(100, 149, 237),
                            sf::Vector2f(190, 500), sf::Vector2f(200, 35));
        Button registreButton("Registre", font, 24, sf::Color::White, sf::Color(255, 165, 0),
                            sf::Vector2f(410, 500), sf::Vector2f(200, 35));
        Button quitButton("Exit", font, 24, sf::Color::White, sf::Color(255, 99, 71),
                            sf::Vector2f(300, 560), sf::Vector2f(200, 35));
        // Ajout des boutons au vecteur
        buttons.emplace_back(std::make_unique<Button>(loginButton));
        buttons.emplace_back(std::make_unique<Button>(registreButton));
        buttons.emplace_back(std::make_unique<Button>(quitButton));
    }
    if(texts.empty()){
        // Création des champs de texte
        TextField usernameField(font, 24, sf::Color::Black, sf::Color::White, 
            sf::Vector2f(300, 400), sf::Vector2f(200, 35), "Username");
        TextField passwordField(font, 24, sf::Color::Black, sf::Color::White,
            sf::Vector2f(300, 440), sf::Vector2f(200, 35), "Password", true);
        // Ajout des champs de texte au vecteur
        texts.emplace_back(std::make_unique<TextField>(usernameField));
        texts.emplace_back(std::make_unique<TextField>(passwordField));
    }
    
    //draw buttons
    drawButtons();
    //draw text fields
    drawTextFields();

    // Quitter le jeu
    if (buttons[2]->isClicked(*window)) {
        // Envoyer une requête de déconnexion au serveur
        cleanup();
        window->close();
        return;
    }

    // si un des champs de texte est vide, on ne fait rien
    if (texts[0]->getText().empty() || texts[1]->getText().empty()) {
        return;
    }
    
    //gerer les evenements
    json j = {
        {"username", texts[0]->getText()},
        {"password", texts[1]->getText()}
    };

    if (buttons[0]->isClicked(*window)) {
        j["action"] = "login";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
    }
    else if (buttons[1]->isClicked(*window)) {
        j["action"] = "registre";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
    }
}

void SFMLGame::mainMenu(){
    // Display the main menu background
    displayBackground(textures->logoConnexion);

    if(buttons.empty()){
        std ::cout << "Creating buttons" << std::endl;
        // Création des boutons
        Button playButton("Play", font, 24, sf::Color::White, sf::Color(70, 70, 200), 
                        sf::Vector2f(190, 560), sf::Vector2f(200, 35));
        Button chatButton("Chat", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                        sf::Vector2f(410, 560), sf::Vector2f(200, 35));
        // Création du bouton "Exit" avec une photo dans sf::Texture logoChat dans le coin supérieur gauche
        Button quitButton("", font, 24, sf::Color::Transparent, sf::Color::White,
                        sf::Vector2f(10, 20), sf::Vector2f(40, 40), sf::Color::Transparent);
        quitButton.drawPhoto(textures->logoExit);

        // Création du bouton "Settings" avec une photo dans sf::Texture logoSettings dans le coin supérieur droit
        Button settingsButton("", font, 24, sf::Color::Transparent, sf::Color::White,
            sf::Vector2f(WINDOW_WIDTH - 130, 20), sf::Vector2f(35, 35), sf::Color::Transparent);
        settingsButton.drawPhoto(textures->logoSettings);

        //Creation du button "Notification" avec une photo dans sf::Texture logoNotification dans le coin supérieur gauche
        Button notificationButton("", font, 24, sf::Color::Transparent, sf::Color::White,
            sf::Vector2f(WINDOW_WIDTH - 190, 20), sf::Vector2f(45, 45), sf::Color::Transparent);
        notificationButton.drawPhoto(textures->logoNotification);
        
        // Création du bouton "Profile" avec une photo dans sf::Texture logoTeams dans le coin supérieur droit
        Button profileButton("", font, 24, sf::Color::Transparent, sf::Color::White,
            sf::Vector2f(WINDOW_WIDTH - 70, 20), sf::Vector2f(35, 35), sf::Color::Transparent);
        //profileButton.drawPhoto(avatarduClient);
        
        // Ajout des boutons au vecteur
        buttons.emplace_back(std::make_unique<Button>(playButton));
        buttons.emplace_back(std::make_unique<Button>(chatButton));
        buttons.emplace_back(std::make_unique<Button>(quitButton));
        buttons.emplace_back(std::make_unique<Button>(settingsButton));
        buttons.emplace_back(std::make_unique<Button>(notificationButton));
        buttons.emplace_back(std::make_unique<Button>(profileButton));
    }
    
    //draw buttons
    drawButtons();

    json j;
    
    if(buttons[0]->isClicked(*window)) {
        //pass
    }
    else if(buttons[1]->isClicked(*window)) {
        // On passe à l'état du chat
        j["action"] = "chat";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
        return;
    }
    else if(buttons[2]->isClicked(*window)) {
        j["action"] = "welcome";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
        std::cout << "Exit button clicked" << j << std::endl;
        return;
    }
    else if(buttons[3]->isClicked(*window)) {
        //pass
    }
    else if(buttons[4]->isClicked(*window)) {
        // pass
    }
    else if(buttons[5]->isClicked(*window)) {
        //pass
    }
}
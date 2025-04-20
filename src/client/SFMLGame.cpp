#include "SFMLGame.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <map>
#include <algorithm>
#include <string>
#include <vector>
#include <memory>


//window size
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 650;
const std::string WINDOW_TITLE = "Tetris Royal";


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

void SFMLGame::drawButtons() {
    for (const auto& button : buttons) {
        button.second->draw(*window);
    }
}

// Affichage des champs de texte
void SFMLGame::drawTextFields() {
    for (const auto& text : texts) {
        text.second->draw(*window);
    }
}

void SFMLGame::drawMessages() {
    for (std::size_t i = 0; i < messages.size(); ++i) {
        const auto& msg = messages[i];
        MessagesY += 50 * i+1; // Adjust the Y position for each message
        displayMessage(msg.first, msg.second);
        MessagesY = 60; // Reset Y position for the next message
    }
}




void SFMLGame::handleTextFieldEvents(sf::Event& event) {
    for (const auto& text : texts) {
        text.second->handleInput(event);
    }
}
void SFMLGame::handleButtonEvents() {
    for (const auto& button : buttons) {
        button.second->update();
        button.second->setBackgroundColor(*window);
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
        //{&textures->notification, NotificationBackGround},
        //{&textures->logoChat, LogoChat},
        {&textures->logoNotification, LogoNotification},
        {&textures->logoTeams, LogoTeams},
        {&textures->logoRanking, LogoRanking},
        {&textures->logoSettings, LogoSettings},
        {&textures->logoMain, LogoMain},
        {&textures->logoExit, LogoExit},
        {&textures->logoAddFriend, LogoAddFriend},
        //{&textures->logoFrindsRequest, LogoFrindsRequest}
        
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

        // Gestion du défilement de la molette
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                if (currentState == MenuState::Friends) {
                    friendsListOffset += event.mouseWheelScroll.delta * 30; // Ajustez la vitesse de défilement
                    friendsListOffset = std::max(friendsListOffset, 0.0f); // Empêche de défiler au-dessus du début
                } else if (currentState == MenuState::chat) {
                    chatContactsOffset += event.mouseWheelScroll.delta * 30; // Ajustez la vitesse de défilement
                    chatContactsOffset = std::max(chatContactsOffset, 0.0f); // Empêche de défiler au-dessus du début
                }
            }
        }

        // Gestion des clics sur les TextField
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            for (const auto& text : texts) {
                if (text.second->isMouseOver(*window)) {
                    text.second->setActive(true);
                } else {
                    text.second->setActive(false);
                }
            }
        }

        // Gestion des événements de la souris pour les boutons
        if (!buttons.empty()) {
            handleButtonEvents();
        }

        // Gestion des événements de texte
        if (event.type == sf::Event::TextEntered) {
            handleTextFieldEvents(event);
        }
        if (event.type == sf::Event::KeyPressed) {
            std::string input;
        
            switch (event.key.code) {
                case sf::Keyboard::Left:
                    input = std::string(1, static_cast<char>(KEY_LEFT)); 
                    break;
                case sf::Keyboard::Right:
                    input = std::string(1, static_cast<char>(KEY_RIGHT));
                    break;
                case sf::Keyboard::Down:
                    input = std::string(1, static_cast<char>(KEY_DOWN)); 
                    break;
                case sf::Keyboard::Up:
                    input = std::string(1, static_cast<char>(KEY_UP)); 
                    break;
                case sf::Keyboard::Space:
                    input = " ";
                    break;
            
                default:
                    break;
            }
        
            if (!input.empty()) {
                client.sendInputFromSFML(input);
            }
        }
    }
}

void SFMLGame::refreshMenu() {
    auto newState = client.getCurrentMenuState();
    if(newState != currentState) {
        currentState = newState;
        std::cout << "State changed to: " << static_cast<int>(currentState) << std::endl;
        this->cleanup();
    }

    window->clear();
    
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
            //to do  
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
            chatMenu();
            break;
        case MenuState::ManageRoom:
            //teamsMenu();
            break;
        case MenuState::CreateRoom:
            //createRoomMenu();
            break;
        case MenuState::JoinOrCreateGame:
            CreateOrJoinGame();
            break;
        case MenuState::Pause:
            //gamePauseMenu();
            
            break;
        case MenuState::CreateGame:
            ChoiceGameMode();
            break;
        case MenuState::Play:
            displayGame();
            break;
        default:
            //std::cerr << "Unhandled MenuState: " << static_cast<int>(currentState) << std::endl;
            break;
    }
    window->display();
}


// Compléter le switch dans run()
void SFMLGame::run() {
    // Start client threads (they're now managed by the Client class)
    std::thread clientThread([this]() { client.run(); });
    clientThread.detach();

    window->create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
    window->setFramerateLimit(60);

    //window->setVerticalSyncEnabled(true);
    //std::cout << "Vertical sync enabled (if supported by the system)." << std::endl;

    while (window->isOpen()) {
        handleEvents();
        refreshMenu();
    }
}

void SFMLGame::cleanup() {
    buttons.clear();
    if(buttons.empty()){
        std::cout << "Buttons cleared" << std::endl;
    }
    texts.clear();
    if(texts.empty()){
        std::cout << "Texts cleared" << std::endl;
    }
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
        buttons["login"] = std::make_unique<Button>(loginButton);
        buttons["registre"] = std::make_unique<Button>(registreButton);
        buttons["quit"] = std::make_unique<Button>(quitButton);
    }
    if(texts.empty()){
        // Création des champs de texte
        TextField usernameField(font, 24, sf::Color::Black, sf::Color::White, 
            sf::Vector2f(300, 400), sf::Vector2f(200, 35), "Username");
        TextField passwordField(font, 24, sf::Color::Black, sf::Color::White,
            sf::Vector2f(300, 440), sf::Vector2f(200, 35), "Password", true);
        // Ajout des champs de texte au vecteur
        texts["username"] = std::make_unique<TextField>(usernameField);
        texts["password"] = std::make_unique<TextField>(passwordField);
    }
    
    //draw buttons
    drawButtons();
    //draw text fields
    drawTextFields();

    // Quitter le jeu
    if (buttons["quit"]->isClicked(*window)) {
        // Envoyer une requête de déconnexion au serveur
        cleanup();
        window->close();
        return;
    }

    if (texts["username"]->getText().empty() || texts["password"]->getText().empty()) {
        return;
    }
    
    //gerer les evenements
    json j = {
        {"username", texts["username"]->getText()},
        {"password", texts["password"]->getText()}
    };

    if (buttons["login"]->isClicked(*window)) {
        j["action"] = "login";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
    else if (buttons["registre"]->isClicked(*window)) {
        j["action"] = "registre";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
}

void SFMLGame::mainMenu(){
    // Display the main menu background
    displayBackground(textures->logoConnexion);

    if(buttons.empty()){
        std ::cout << "Creating buttons" << std::endl;
        // Création des boutons
        Button teamsButton("Teams", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                        sf::Vector2f(80, 560), sf::Vector2f(150, 35));
        Button playButton("Play", font, 24, sf::Color::White, sf::Color(70, 70, 200), 
                        sf::Vector2f(230, 560), sf::Vector2f(150, 35));
        Button chatButton("Chat", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                        sf::Vector2f(410, 560), sf::Vector2f(150, 35));
        Button friendsButton("Friends", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                        sf::Vector2f(630, 560), sf::Vector2f(150, 35));
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
        buttons["teams"] = std::make_unique<Button>(teamsButton);
        buttons["play"] = std::make_unique<Button>(playButton);
        buttons["chat"] = std::make_unique<Button>(chatButton);
        buttons["friends"] = std::make_unique<Button>(friendsButton);
        buttons["quit"] = std::make_unique<Button>(quitButton);
        buttons["settings"] = std::make_unique<Button>(settingsButton);
        buttons["notification"] = std::make_unique<Button>(notificationButton);
        buttons["profile"] = std::make_unique<Button>(profileButton);
    }
    
    //draw buttons
    drawButtons();

    json j;
    
    if(buttons["teams"]->isClicked(*window)) {
        //pass
    }
    else if(buttons["play"]->isClicked(*window)) {
        j["action"] = "createjoin";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        //cleanup();
        return;
    }
    else if(buttons["chat"]->isClicked(*window)) {
        // On passe à l'état du chat
        j["action"] = "chat";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        std::cout << "Chat button clicked" << std::endl;
        return;
    }
    else if(buttons["friends"]->isClicked(*window)) {
        j["action"] = "friends";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        std::cout << "Friends button clicked" << j << std::endl;
        return;
    }
    else if(buttons["quit"]->isClicked(*window)) {
        j["action"] = "welcome";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        std::cout << "Exit button clicked" << j << std::endl;
        return;
    }
    else if(buttons["settings"]->isClicked(*window)) {
        //pass
    }else if(buttons["notification"]->isClicked(*window)) {
        //pass
    }
}


void SFMLGame::CreateOrJoinGame(){
    displayBackground(textures->logoConnexion);
    if(buttons.empty()){
        std ::cout << "Creating buttons" << std::endl;
        // Création des boutons
        Button CreateButton("Créer", font, 24, sf::Color::White, sf::Color(65, 105, 225), 
                        sf::Vector2f(310, 300), sf::Vector2f(180, 45));
        Button JoinButton("Rejoindre", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                        sf::Vector2f(310, 360), sf::Vector2f(180, 45));
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
        buttons["create"] = std::make_unique<Button>(CreateButton);
        buttons["join"] = std::make_unique<Button>(JoinButton);
        buttons["quit"] = std::make_unique<Button>(quitButton);
        buttons["settings"] = std::make_unique<Button>(settingsButton);
        buttons["notification"] = std::make_unique<Button>(notificationButton);
        buttons["profile"] = std::make_unique<Button>(profileButton);
    }
    
    //draw buttons
    drawButtons();
    json j;
    
    if(buttons["create"]->isClicked(*window)) {
        j["action"] = "choiceMode";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        //cleanup();
        return;
    }
    else if(buttons["quit"]->isClicked(*window)){
        j["action"] = "main";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        std::cout << "Exit button clicked" << j << std::endl;
        return;
    }
}


void SFMLGame::ChoiceGameMode(){
    displayBackground(textures->logoConnexion);
    if(buttons.empty()){
        std ::cout << "Creating buttons" << std::endl;
        // Création des boutons
        Button EndlessButton("Mode Endless", font, 24, sf::Color::White, sf::Color(65, 105, 225), 
                        sf::Vector2f(310, 250), sf::Vector2f(180, 45));
        Button DuelButton("Mode Duel", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                        sf::Vector2f(310, 310), sf::Vector2f(180, 45));

        Button ClassicButton("Mode Classic", font, 24, sf::Color::White, sf::Color(65, 105, 225), 
                        sf::Vector2f(310, 370), sf::Vector2f(180, 45));
        Button RoyaleButton("Mode Royale", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                        sf::Vector2f(310, 430), sf::Vector2f(180, 45));
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
        buttons["endless"] = std::make_unique<Button>(EndlessButton);
        buttons["duel"] = std::make_unique<Button>(DuelButton);
        buttons["classic"] = std::make_unique<Button>(ClassicButton);
        buttons["royale"] = std::make_unique<Button>(RoyaleButton);
        buttons["quit"] = std::make_unique<Button>(quitButton);
        buttons["settings"] = std::make_unique<Button>(settingsButton);
        buttons["notification"] = std::make_unique<Button>(notificationButton);
        buttons["profile"] = std::make_unique<Button>(profileButton);

    }
    
    //draw buttons
    drawButtons();
    json j;
    
    if(buttons["endless"]->isClicked(*window)) {
        j["action"] = "EndlessMode";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        //cleanup();
        return;
    }else if(buttons["duel"]->isClicked(*window)){
        j["action"] = "DuelMode";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        //cleanup();
        return;
    }

}
void SFMLGame::displayGame(){
    
    if (client.isGameStateUpdated()) {
        
        GameState gameData = client.getGameState();
        if(gameData.isGame){
            drawGrid(gameData.gridData);
            drawTetramino(gameData.currentPieceData);
            drawTetramino(gameData.nextPieceData);
            drawScore(gameData.scoreData);
        }
        
        else if (gameData.isEnd){
            drawEndGame(gameData.menu);
        }
    }

  

    
}

void SFMLGame::drawGrid(const json& grid) {
    int width = grid[jsonKeys::WIDTH]; 
    int height = grid[jsonKeys::HEIGHT]; 
    const json& cells = grid[jsonKeys::CELLS];
    sf::Color color ;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            sf::RectangleShape cell(sf::Vector2f(cellSize - 1, cellSize - 1)); 
            cell.setPosition(x * cellSize + cellSize, y * cellSize);  
            
            bool occupied = cells[y][x][jsonKeys::OCCUPIED]; 
            if (occupied) {
                int colorValue = cells[y][x][jsonKeys::COLOR];
                sf::Color color = fromSFML(colorValue);
                cell.setFillColor(color);
            }else{
                cell.setFillColor(sf::Color(50, 50, 50));
            }

            window->draw(cell);  
        }
    }


    for (int y = 0; y < 2*height; ++y) {
        sf::RectangleShape leftWall(sf::Vector2f(cellSize/2, cellSize/2));
        leftWall.setPosition(cellSize/2, y * (cellSize/2));
        leftWall.setFillColor(sf::Color(139, 69, 19));
        leftWall.setOutlineColor(sf::Color::Black); 
        leftWall.setOutlineThickness(1.0f); 
        window->draw(leftWall); // Mur gauche
    }
    for (int x = 1; x <= 2*width + 2; ++x) {
        sf::RectangleShape bottomWall(sf::Vector2f(cellSize/2, cellSize/2));
        bottomWall.setPosition(x * (cellSize/2), cellSize * height);
        bottomWall.setFillColor(sf::Color(139, 69, 19));
        bottomWall.setOutlineColor(sf::Color::Black); 
        bottomWall.setOutlineThickness(1.0f); 
        window->draw(bottomWall); // Mur bas
    }
    for (int y = 0; y < 2*height; ++y) {
        sf::RectangleShape rightWall(sf::Vector2f(cellSize/2, cellSize/2));
        rightWall.setPosition(cellSize * width + cellSize, y * (cellSize/2));
        rightWall.setFillColor(sf::Color(139, 69, 19));
        rightWall.setOutlineColor(sf::Color::Black); 
        rightWall.setOutlineThickness(1.0f); 
        window->draw(rightWall); // Mur droit
    }
}


void SFMLGame::drawTetramino(const json& tetraPiece) {
    // Récupération des informations
    int x = tetraPiece[jsonKeys::X];
    int y = tetraPiece[jsonKeys::Y];
    std::vector<std::vector<std::string>> shape = tetraPiece[jsonKeys::SHAPE];
    int shapeSymbol = tetraPiece[jsonKeys::SHAPE_SYMBOL];

    // Taille d’un bloc de tétrimino en pixels

    sf::Color color = SFMLGame::fromShapeSymbolSFML(std::string(1, shapeSymbol));

    for (size_t row = 0; row < shape.size(); ++row) {
        for (size_t col = 0; col < shape[row].size(); ++col) {
            if (shape[row][col][0] != ' ') {
                sf::RectangleShape block(sf::Vector2f(cellSize-1, cellSize-1));
                block.setFillColor(color);

                block.setPosition(((x + col) * cellSize) , (y + row) * cellSize);

                window->draw(block);
            }
        }
    }

}

void SFMLGame::drawEndGame(const json& endGameData) {
    std::string message = endGameData[jsonKeys::TITLE];
    sf::Color color;
    if (message == "GAME OVER"){
        color = sf::Color::Red;
    }
    else{
        color = sf::Color::Green;
    }

    sf::Text endGameText(message, font, 50);
    endGameText.setFillColor(color);
    endGameText.setPosition(WINDOW_WIDTH / 2 - endGameText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 4 - endGameText.getGlobalBounds().height / 2);
    window->draw(endGameText);

    if(buttons.empty()){
        
        Button Rejouer("rejouer", font, 24, sf::Color::White, sf::Color(65, 105, 225), 
                        sf::Vector2f(200, 310), sf::Vector2f(180, 45));
        Button Retour("retour au menu", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                        sf::Vector2f(450, 310), sf::Vector2f(180, 45));

        
        buttons["rejouer"] = std::make_unique<Button>(Rejouer);
        buttons["retour au menu"] = std::make_unique<Button>(Retour);
        
    }
    
    drawButtons();

    json j;
    
    if(buttons["rejouer"]->isClicked(*window)) {
        j["action"] = "createjoin";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        client.setGameStateUpdated(false);
        client.setGameStateIsEnd(false);
        //cleanup();
        return;
    }else if(buttons["retour au menu"]->isClicked(*window)){
        j["action"] = "main";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        client.setGameStateUpdated(false);
        client.setGameStateIsEnd(false);
        //cleanup();
        return;
    }

}

sf::Color SFMLGame::fromShapeSymbolSFML(const std::string& symbol) {
    if (symbol == "I") return sf::Color::Cyan;
    if (symbol == "O") return sf::Color::Yellow;
    if (symbol == "T") return sf::Color(128, 0, 128); // Violet
    if (symbol == "S") return sf::Color::Green;
    if (symbol == "Z") return sf::Color::Red;
    if (symbol == "J") return sf::Color::Blue;
    if (symbol == "L") return sf::Color(255, 165, 0); // Orange
    return sf::Color::White; // Default
}

sf::Color SFMLGame::fromSFML(int value) {
    if (value == 4) return sf::Color::Cyan;
    if (value == 2) return sf::Color::Yellow;
    if (value == 6) return sf::Color(128, 0, 128); // Violet
    if (value == 3) return sf::Color::Green;
    if (value == 1) return sf::Color::Red;
    if (value == 5) return sf::Color::Blue;
    if (value == 7) return sf::Color(255, 165, 0); // Orange
    return sf::Color::White; // Default
}


void SFMLGame::chatMenu() {
    // Display the chat background
    displayBackground(textures->chat);

    // Sidebar for contacts list
    Rectangle sidebar(sf::Vector2f(0, 0), sf::Vector2f(200, WINDOW_HEIGHT), sf::Color(50, 50, 70), sf::Color(100, 100, 120));
    sidebar.draw(*window);

    // Header for contacts list
    sf::Text contactsHeader("Contacts", font, 28);
    contactsHeader.setFillColor(sf::Color::White);
    contactsHeader.setStyle(sf::Text::Bold | sf::Text::Underlined | sf::Text::Italic);
    contactsHeader.setPosition(10, 10);
    window->draw(contactsHeader);

    if (texts.empty()) {
        // Création des champs de texte
        TextField messageField(font, 20, sf::Color::Black, sf::Color(250, 250, 250),
            sf::Vector2f(205, WINDOW_HEIGHT - 40), sf::Vector2f(WINDOW_WIDTH - 250, 35), "Enter un message");

        TextField searchField(font, 20, sf::Color::Black, sf::Color(250, 250, 250),
            sf::Vector2f(40, 50), sf::Vector2f(155, 35), "Search");

        // Ajout des champs de texte au vecteur
        texts["message"] = std::make_unique<TextField>(messageField);
        texts["search"] = std::make_unique<TextField>(searchField);
    }

    if (buttons.empty()) {
        // Bouton pour revenir au menu principal
        Button backButton("", font, 20, sf::Color::Transparent, sf::Color::White,
                          sf::Vector2f(7, 50), sf::Vector2f(25, 35));
        backButton.drawPhoto(textures->logoMain);
        
        // Bouton pour envoyer le message
        Button sendButton(">", font, 20, sf::Color::White, sf::Color(70, 200, 70),
                          sf::Vector2f(WINDOW_WIDTH - 40, WINDOW_HEIGHT - 40), sf::Vector2f(35, 35));
        

        // Ajout des boutons au vecteur
        buttons["quit"] = std::make_unique<Button>(backButton);
        buttons["send"] = std::make_unique<Button>(sendButton);
        
    }
    
    drawTextFields();
    drawButtons();
    drawMessages();
    
    if (!client.getServerData().empty() && client.getServerData().contains("data")) {
        contacts = client.getServerData()["data"];
        client.clearServerData();
    }

    // Drapeau pour indiquer si un clic a été traité
    bool clickHandled = false;

    // Vérifier d'abord le backButton
    if (buttons["quit"]->isClicked(*window)) {
        json j;
        j["action"] = "main";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        clickHandled = true; // Marquer le clic comme traité
        return; // Sortir immédiatement après avoir traité le clic
    }

    // Ensuite vérifier le sendButton
    if (!clickHandled && buttons["send"]->isClicked(*window)) {
        json j = {
            {"message", texts["message"]->getText()},
        };
        network->sendData(j.dump() + "\n", client.getClientSocket());
        messages.push_back({"You", texts["message"]->getText()});
        //messages.push_back({contact, "hellllllo"});
        texts["message"]->clear(); // Effacer le champ de texte après l'envoi
        clickHandled = true; // Marquer le clic comme traité
        return;
    }

    // Gérer les boutons de contact uniquement si aucun autre clic n'a été traité
    if (!clickHandled) {
        static std::vector<sf::Texture> avatarTextures(20); // Persistent storage for textures
        const float avatarRadius = 15.0f;
        const float contactHeight = 50.0f;

        for (size_t i = 0; i < contacts.size() && i < 20; ++i) {
            bool exists = std::any_of(buttons.begin(), buttons.end(), [&](const auto& button) {
                return button.first.compare(contacts[i]) == 0; ;
            });
            if (!exists) {
                // Create a new button for the contact
                Button contactButton(contacts[i], font, 20, sf::Color::White, sf::Color::Transparent,
                                     sf::Vector2f(0, 100 + i * 50), sf::Vector2f(200, 50), sf::Color::Transparent);
                buttons[contacts[i]] = std::make_unique<Button>(contactButton);
            }
            float contactY = 100.0f + i * contactHeight;

            // Check if avatar is already drawn for this contact
            static std::map<std::string, sf::Sprite> drawnAvatars;
            if (drawnAvatars.find(contacts[i]) == drawnAvatars.end()) {
                // Load avatar texture
                const std::string avatarPath = "../../res/avatar/avatar" + std::to_string(i + 1) + ".png";

                // Check if the texture is already loaded
                if (avatarTextures[i].loadFromFile(avatarPath)) {
                    // Create sprite with circular clipping effect
                    sf::Sprite avatarSprite(avatarTextures[i]);

                    // Get size and compute scale to fit in the circle
                    sf::Vector2u textureSize = avatarTextures[i].getSize();
                    float scale = (avatarRadius * 2) / std::max(textureSize.x, textureSize.y);
                    avatarSprite.setScale(scale, scale);

                    // Re-center the image inside the circle
                    float newWidth = textureSize.x * scale;
                    float newHeight = textureSize.y * scale;
                    float offsetX = avatarRadius - newWidth / 2;
                    float offsetY = avatarRadius - newHeight / 2;

                    avatarSprite.setPosition(20 + offsetX, contactY + offsetY);

                    // Store the sprite in the map
                    drawnAvatars[contacts[i]] = avatarSprite;
                } else {
                    // Fallback: grey circle + initial
                    sf::CircleShape avatarCircle(avatarRadius);
                    avatarCircle.setPosition(20, contactY);
                    avatarCircle.setFillColor(sf::Color::White);

                    std::string contactStr = contacts[i];
                    char initial = contactStr.empty() ? '?' : static_cast<char>(std::toupper(contactStr[0]));
                    sf::Text initialText(std::string(1, initial), font, 24);
                    initialText.setFillColor(sf::Color::White);
                    initialText.setStyle(sf::Text::Bold);
                    initialText.setPosition(20 + avatarRadius - 8, contactY + avatarRadius - 14);

                    // Draw fallback avatar
                    window->draw(avatarCircle);
                    window->draw(initialText);
                    continue;
                }
            }

            // Draw the avatar sprite
            window->draw(drawnAvatars[contacts[i]]);
        }
        
        /*
        // Vérifier les boutons de contact
        for (size_t i = 0; i < buttons.size() - 2; ++i) {
            if (buttons[contact[i]]->isClicked(*window)) {
                std::cout << "Contact " << contacts[i] << " clicked!" << std::endl;
                json j = {
                    {"action", "openChat"},
                    {"contact", contacts[i - 2]}
                };
                contact = contacts[i - 2];
                network->sendData(j.dump() + "\n", client.getClientSocket());
                break; // Sortir de la boucle après avoir trouvé le contact cliqué
            }
        }*/
    }
    // Afficher les anciens messages entre l'utilisateur et le contact après avoir cliqué sur le contact
    if (!contact.empty()) {
        Rectangle sidebar(sf::Vector2f(202, 0), sf::Vector2f(WINDOW_WIDTH - 200, 50), sf::Color(50, 50, 70), sf::Color(100, 100, 120));
        sidebar.draw(*window);
        sf::Text contactName(contact, font, 20);
        contactName.setFillColor(sf::Color::White);
        contactName.setStyle(sf::Text::Bold);
        contactName.setPosition(260, 10);
        window->draw(contactName);

        // Afficher les message de l'utilisateur
        auto message = client.getServerData();
        if (message.contains("sender") && message["sender"] == contact) {
            messages.push_back({contact, message["message"]});
        }
        client.clearServerData();
    }
    clickHandled = false; // Réinitialiser le drapeau après le traitement des clics
}

void SFMLGame::displayMessage(const std::string& sender, const std::string& message) {
    const bool isYou = sender == "You";
    const float bubbleHeight = 30.f;
    const float bubbleWidth = std::min(300.f, message.length() * 13.f);
    const float cornerRadius = 10.f;
    const float bubbleX = isYou ? WINDOW_WIDTH - 5 - bubbleWidth : 210;
    const sf::Color bubbleColor = isYou ? sf::Color(70, 130, 180) : sf::Color(90, 90, 110);

    // Create and draw the main bubble
    sf::RectangleShape mainRect(sf::Vector2f(bubbleWidth - 2 * cornerRadius, bubbleHeight));
    mainRect.setPosition(bubbleX + cornerRadius, MessagesY);
    mainRect.setFillColor(bubbleColor);
    mainRect.setOutlineColor(sf::Color(120, 120, 140));
    mainRect.setOutlineThickness(1);
    window->draw(mainRect);


    // Draw message text
    sf::Text messageText(message, font, 16);
    messageText.setFillColor(sf::Color::White);
    
    const sf::FloatRect textBounds = messageText.getLocalBounds();
    messageText.setPosition(
        bubbleX + cornerRadius + 5.f, 
        MessagesY + (bubbleHeight - textBounds.height) / 2 - textBounds.top
    );
    window->draw(messageText);
}



void SFMLGame::drawScore(const json& scoreData) {
    try {
        // Version sécurisée avec vérifications
        int score = scoreData.value("score", 0); // Valeur par défaut 0 si "current" n'existe pas
        int combo = scoreData.value("combo", 0);
        
        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(cellSize*12 + 10, cellSize); // Positionnement recommandé

        sf::Text comboText;
        comboText.setFont(font);
        comboText.setString("Combo: " + std::to_string(combo));
        comboText.setCharacterSize(24);
        comboText.setFillColor(sf::Color::White);
        comboText.setPosition(cellSize*12 + 10, 2*cellSize + 10);
        
        window->draw(scoreText);
        window->draw(comboText);
    } catch (const json::exception& e) {
        std::cerr << "Erreur d'affichage du score: " << e.what() << std::endl;
        // Version de secours
        sf::Text errorText("Score: N/A", font, 24);
        errorText.setPosition(20, 20);
        window->draw(errorText);
    }
}

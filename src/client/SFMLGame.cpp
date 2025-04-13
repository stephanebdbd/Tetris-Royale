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
const unsigned int WINDOW_HEIGHT = 600;
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
        button->draw(*window);
    }
}

void SFMLGame::drawTextFields() {
    for (const auto& text : texts) {
        text->draw(*window);
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
                if (text->isMouseOver(*window)) {
                    text->setActive(true);
                } else {
                    text->setActive(false);
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
            chatMenu();
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
    window->display();
}


// Compléter le switch dans run()
void SFMLGame::run() {
    // Start client threads (they're now managed by the Client class)
    std::thread clientThread([this]() { client.run(); });
    clientThread.detach();

    window->create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
    window->setFramerateLimit(60);

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
        return;
    }
    else if (buttons[1]->isClicked(*window)) {
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
        buttons.emplace_back(std::make_unique<Button>(teamsButton));
        buttons.emplace_back(std::make_unique<Button>(playButton));
        buttons.emplace_back(std::make_unique<Button>(chatButton));
        buttons.emplace_back(std::make_unique<Button>(friendsButton));
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
        //pass
    }
    else if(buttons[2]->isClicked(*window)) {
        // On passe à l'état du chat
        j["action"] = "chat";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        std::cout << "Chat button clicked" << std::endl;
        return;
    }
    else if(buttons[3]->isClicked(*window)) {
        j["action"] = "friends";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        std::cout << "Friends button clicked" << j << std::endl;
        return;
    }
    else if(buttons[4]->isClicked(*window)) {
        j["action"] = "welcome";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        std::cout << "Exit button clicked" << j << std::endl;
        return;
    }
    else if(buttons[5]->isClicked(*window)) {
        //pass
    }else if(buttons[6]->isClicked(*window)) {
        //pass
    }
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
    contactsHeader.setStyle(sf::Text::Bold);
    contactsHeader.setPosition(10, 10);
    window->draw(contactsHeader);

    if (texts.empty()) {
        // Création des champs de texte
        TextField messageField(font, 20, sf::Color::Black, sf::Color(250, 250, 250),
            sf::Vector2f(205, WINDOW_HEIGHT - 40), sf::Vector2f(WINDOW_WIDTH - 250, 35), "Enter un message");

        TextField searchField(font, 20, sf::Color::Black, sf::Color(250, 250, 250),
            sf::Vector2f(40, 50), sf::Vector2f(155, 35), "Search");

        // Ajout des champs de texte au vecteur
        texts.emplace_back(std::make_unique<TextField>(messageField));
        texts.emplace_back(std::make_unique<TextField>(searchField));
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
        buttons.emplace_back(std::make_unique<Button>(backButton));
        buttons.emplace_back(std::make_unique<Button>(sendButton));
        
    }
    
    drawTextFields();
    drawButtons();
    drawMessages();
    
    if (!client.getServerData().empty() && client.getServerData().contains("data")) {
        contacts = client.getServerData()["data"];
        client.clearServerData();
    }
    //float contactY = 100 - chatContactsOffset; // Appliquer le décalage vertical

    /** 
    for (size_t i = 0; i < contacts.size(); ++i) {
        if (contactY + i * 50 >= 100 && contactY + i * 50 <= WINDOW_HEIGHT - 50) { // Afficher uniquement les contacts visibles
            sf::Text contactName(std::string(contacts[i]), font, 20);
            contactName.setFillColor(sf::Color::White);
            contactName.setPosition(20, contactY + i * 50);
            window->draw(contactName);
        }
    }*/

    // Drapeau pour indiquer si un clic a été traité
    bool clickHandled = false;

    // Vérifier d'abord le backButton
    if (buttons[0]->isClicked(*window)) {
        json j;
        j["action"] = "main";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        clickHandled = true; // Marquer le clic comme traité
        return; // Sortir immédiatement après avoir traité le clic
    }

    // Ensuite vérifier le sendButton
    if (!clickHandled && buttons[1]->isClicked(*window)) {
        json j = {
            {"message", texts[0]->getText()},
        };
        network->sendData(j.dump() + "\n", client.getClientSocket());
        messages.push_back({"You", texts[0]->getText()});
        //messages.push_back({contact, "hellllllo"});
        texts[0]->clear(); // Effacer le champ de texte après l'envoi
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
                return button->getText().compare(contacts[i]) == 0; ;
            });
            if (!exists) {
                // Create a new button for the contact
                Button contactButton(contacts[i], font, 20, sf::Color::White, sf::Color::Transparent,
                                     sf::Vector2f(0, 100 + i * 50), sf::Vector2f(200, 50), sf::Color::Transparent);
                buttons.emplace_back(std::make_unique<Button>(contactButton));
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
        

        // Vérifier les boutons de contact
        for (size_t i = 0; i < buttons.size() - 2; ++i) {
            if (buttons[i + 2]->isClicked(*window)) {
                std::cout << "Contact " << contacts[i] << " clicked!" << std::endl;
                json j = {
                    {"action", "openChat"},
                    {"contact", contacts[i]}
                };
                contact = contacts[i];
                network->sendData(j.dump() + "\n", client.getClientSocket());
                break; // Sortir de la boucle après avoir trouvé le contact cliqué
            }
        }
    }
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
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


const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 650;
const std::string WINDOW_TITLE = "Tetris Royal";

float buttonWidth = 175;
float buttonHeight = 50;
float spacing = 20;
float startX = (WINDOW_WIDTH - (4 * buttonWidth + 3 * spacing)) / 2;
float y = 560;

// Couleurs harmonisées
sf::Color background = sf::Color(30, 30, 30, 180); // noir semi-transparent
sf::Color outline = sf::Color(0, 255, 100);        // vert fluo
sf::Color text = sf::Color::White;

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
    for (const auto& [_, button] : buttons) {
        button->draw(*window);
    }
}

// Affichage des champs de texte
void SFMLGame::drawTextFields() {
    for (const auto& [_, field] : texts) {
        field->draw(*window);
    }
}

// Gestion des champs de texte
void SFMLGame::handleTextFieldEvents(sf::Event& event) {
    for (const auto& [_, field] : texts) {
        field->handleInput(event);
    }
}

// Gestion des boutons
void SFMLGame::handleButtonEvents() {
    for (const auto& [_, button] : buttons) {
        button->update();
        button->setBackgroundColor(*window);
    }
    for (const auto& [_, contact] : chatContacts) {
        contact->update();
        contact->setBackgroundColor(*window);
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
            for (const auto& [_, text] : texts) {
                if (text->isMouseOver(*window)) {
                    text->setActive(true);
                } else {
                    text->setActive(false);
                }
            }
        }
        if (event.type == sf::Event::Resized) {
            handleResize(event.size.width, event.size.height);
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
        case MenuState::Register:
            registerMenu();
            break;
        case MenuState::Login:
            connexionMenu();
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
            friendsMenu();
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
    cleanup();
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

/*
Login/Register Menu
*/


void SFMLGame::welcomeMenu() {
    // Afficher l'arrière-plan du menu de bienvenue
    displayBackground(textures->connexion);

    // Ajouter les boutons s'ils n'existent pas
    if (buttons.empty()) {
        buttons[ButtonKey::Login] = std::make_unique<Button>("Login", font, 24, sf::Color::White, sf::Color(100, 149, 237),
                                                             sf::Vector2f(190, 500), sf::Vector2f(200, 35));
        buttons[ButtonKey::Registre] = std::make_unique<Button>("Registre", font, 24, sf::Color::White, sf::Color(255, 165, 0),
                                                                sf::Vector2f(410, 500), sf::Vector2f(200, 35));
        buttons[ButtonKey::Quit] = std::make_unique<Button>("Exit", font, 24, sf::Color::White, sf::Color(255, 99, 71),
                                                            sf::Vector2f(300, 560), sf::Vector2f(200, 35));
    }

    // Dessiner les boutons
    drawButtons();
    json j;
    // Gérer les clics sur les boutons
    if (buttons.count(ButtonKey::Quit) && buttons[ButtonKey::Quit]->isClicked(*window)) {
        window->close();
        return;
    }

    if (buttons.count(ButtonKey::Registre) && buttons[ButtonKey::Registre]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::REGISTER_MENU;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    if (buttons.count(ButtonKey::Login) && buttons[ButtonKey::Login]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::LOGIN_MENU;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
}
void SFMLGame::registerMenu() {
    // Afficher l'arrière-plan du formulaire d'inscription
    displayBackground(textures->connexion);

    // Ajouter les champs de texte et les boutons s'ils n'existent pas
    if (texts.empty()) {
        texts[TextFieldKey::Username] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                    sf::Vector2f(300, 350), sf::Vector2f(200, 35), "Username.");
        texts[TextFieldKey::Password] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                    sf::Vector2f(300, 400), sf::Vector2f(200, 35), "Password", true);
        texts[TextFieldKey::ConfirmPassword] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                           sf::Vector2f(300, 450), sf::Vector2f(200, 35), "Confirm Password", true);
    }

    if (buttons.empty()) {
        buttons[ButtonKey::Registre] = std::make_unique<Button>("S'inscrire", font, 24, sf::Color::White, sf::Color(100, 149, 237),
                                                                sf::Vector2f(300, 500), sf::Vector2f(200, 35));
        buttons[ButtonKey::Retour] = std::make_unique<Button>("Retour", font, 24, sf::Color::White, sf::Color(255, 99, 71),
                                                              sf::Vector2f(300, 560), sf::Vector2f(200, 35));
    }

    // Dessiner les champs de texte et les boutons
    drawTextFields();
    drawButtons();

    // Gérer les clics sur les boutons
    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        //currentState = MenuState::Welcome; // Retourner au menu de bienvenue
        json j;
        j[jsonKeys::ACTION] = jsonKeys::WELCOME;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    if (buttons[ButtonKey::Registre]->isClicked(*window)) {
        // Récupérer les données des champs de texte
        std::string username = texts[TextFieldKey::Username]->getText();
        std::string password = texts[TextFieldKey::Password]->getText();
        std::string confirmPassword = texts[TextFieldKey::ConfirmPassword]->getText();

        if (username.empty() || password.empty() || confirmPassword.empty()) {
            std::cerr << "Tous les champs doivent être remplis !" << std::endl;
            return;
        }

        if (password != confirmPassword) {
            std::cerr << "Les mots de passe ne correspondent pas !" << std::endl;
            return;
        }

        json j = {
            {jsonKeys::ACTION, "register"},
            {jsonKeys::USERNAME, username},
            {jsonKeys::PASSWORD, password}
        };
        network->sendData(j.dump() + "\n", client.getClientSocket());
    }
}
void SFMLGame::connexionMenu() {
    // Afficher l'arrière-plan du menu de connexion
    displayBackground(textures->connexion);

    // Ajouter les champs de texte et les boutons s'ils n'existent pas
    if (texts.empty()) {
        texts[TextFieldKey::Username] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                    sf::Vector2f(300, 400), sf::Vector2f(200, 35), "Username.");
        texts[TextFieldKey::Password] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                    sf::Vector2f(300, 450), sf::Vector2f(200, 35), "Password", true);
    }

    if (buttons.empty()) {
        buttons[ButtonKey::Login] = std::make_unique<Button>("Se connecter", font, 24, sf::Color::White, sf::Color(100, 149, 237),
                                                             sf::Vector2f(300, 500), sf::Vector2f(200, 35));
        buttons[ButtonKey::Retour] = std::make_unique<Button>("Retour", font, 24, sf::Color::White, sf::Color(255, 99, 71),
                                                              sf::Vector2f(300, 560), sf::Vector2f(200, 35));
    }

    // Dessiner les champs de texte et les boutons
    drawTextFields();
    drawButtons();

    // Gérer les clics sur les boutons
    if (buttons.count(ButtonKey::Retour) && buttons[ButtonKey::Retour]->isClicked(*window)) {
        //currentState = MenuState::Welcome; // Retourner au menu de bienvenue
        cleanup();
        json j;
        j[jsonKeys::ACTION] = jsonKeys::WELCOME;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    if (buttons.count(ButtonKey::Login) && buttons[ButtonKey::Login]->isClicked(*window)) {
        // Récupérer les données des champs de texte
        std::string username = texts[TextFieldKey::Username]->getText();
        std::string password = texts[TextFieldKey::Password]->getText();

        if (username.empty() || password.empty()) {
            std::cerr << "Tous les champs doivent être remplis !" << std::endl;
            return;
        }

        json j = {
            {jsonKeys::ACTION, jsonKeys::LOGIN},
            {jsonKeys::USERNAME, username},
            {jsonKeys::PASSWORD, password}
        };
        network->sendData(j.dump() + "\n", client.getClientSocket());
        //currentState = MenuState::Main; // Passer au menu principal après la connexion
        cleanup();
    }
}

/*
Main Menu
*/


void SFMLGame::mainMenu() {
    // Afficher l'arrière-plan du menu principal
    displayBackground(textures->logoConnexion);

    // Ajouter les boutons s'ils n'existent pas
    if (buttons.empty()) {
// Boutons principaux
        buttons[ButtonKey::Teams] = std::make_unique<Button>("Teams", font, 26, text, background, sf::Vector2f(startX, y), sf::Vector2f(buttonWidth, buttonHeight), outline);
        buttons[ButtonKey::Play] = std::make_unique<Button>("Play", font, 26, text, background, sf::Vector2f(startX + (buttonWidth + spacing), y), sf::Vector2f(buttonWidth, buttonHeight), outline);
        buttons[ButtonKey::Chat] = std::make_unique<Button>("Chat", font, 26, text, background, sf::Vector2f(startX + 2 * (buttonWidth + spacing), y), sf::Vector2f(buttonWidth, buttonHeight), outline);
        buttons[ButtonKey::Friends] = std::make_unique<Button>("Friends", font, 26, text, background, sf::Vector2f(startX + 3 * (buttonWidth + spacing), y), sf::Vector2f(buttonWidth, buttonHeight), outline);

        buttons[ButtonKey::Quit]= std::make_unique<Button>(textures->logoExit, sf::Vector2f(10, 20), sf::Vector2f(40, 40));

        buttons[ButtonKey::Settings] = std::make_unique<Button>(textures->logoSettings,sf::Vector2f(WINDOW_WIDTH - 130, 20), sf::Vector2f(35, 35));
     

        buttons[ButtonKey::Notification] = std::make_unique<Button>(textures->logoNotification, sf::Vector2f(WINDOW_WIDTH - 190, 20), sf::Vector2f(45, 45));

        buttons[ButtonKey::Profile] = std::make_unique<Button>("", font, 24, sf::Color::Transparent, sf::Color::White,
                                                               sf::Vector2f(WINDOW_WIDTH - 70, 20), sf::Vector2f(35, 35), sf::Color::Transparent);
        // buttons[ButtonKey::Profile]->drawPhoto(avatarduClient);
    }

    // Dessiner les boutons
    drawButtons();

    // Gérer les clics sur les boutons
    json j;
    if (buttons.count(ButtonKey::Teams) && buttons[ButtonKey::Teams]->isClicked(*window)) {
        // Action pour le bouton "Teams"
    } else if (buttons.count(ButtonKey::Play) && buttons[ButtonKey::Play]->isClicked(*window)) {
        j[jsonKeys::ACTION] = "createjoin";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    } else if (buttons.count(ButtonKey::Chat) && buttons[ButtonKey::Chat]->isClicked(*window)) {
        j[jsonKeys::ACTION] = "chat";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    } else if (buttons.count(ButtonKey::Friends) && buttons[ButtonKey::Friends]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::FRIENDS;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    } else if (buttons.count(ButtonKey::Quit) && buttons[ButtonKey::Quit]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::WELCOME;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
}

/*
Friends Menu
*/

void SFMLGame::friendsMenu() {
    displayBackground(textures->connexion); // fond dédié, genre un mur bleu clair

    // Bouton "Ajouter un ami" en haut à droite
    buttons[ButtonKey::AddFriend] = std::make_unique<Button>(textures->logoAddFriend,
        sf::Vector2f(window->getSize().x - 150, 20), sf::Vector2f(120, 35));
    // Barre de recherche d'amis
    sf::RectangleShape searchBox(sf::Vector2f(300, 35));
    searchBox.setPosition(50, 20);
    searchBox.setFillColor(sf::Color(230, 230, 250));
    window->draw(searchBox);

    sf::Text searchText("Rechercher...", font, 18);
    searchText.setFillColor(sf::Color::Black);
    searchText.setPosition(60, 25);
    window->draw(searchText);

    // Affichage de la liste d'amis (exemple visuel)
    float startY = 80;
    float spacing = 70;
    friends = client.getServerData()["data"]; // Récupérer la liste d'amis
    //affichage de la liste d'amis sur le terminal
    std::cout << "Liste d'amis : " << std::endl;
    for (const auto& friendName : friends) {
        std::cout << friendName << std::endl;
    }
    
    for (size_t i = 0; i < friends.size(); ++i) {
        sf::RectangleShape friendCard(sf::Vector2f(600, 60));
        friendCard.setPosition(50, startY + i * spacing);
        friendCard.setFillColor(sf::Color(245, 245, 245));
        friendCard.setOutlineThickness(1);
        friendCard.setOutlineColor(sf::Color(200, 200, 200));
        window->draw(friendCard);

        // Avatar fictif
        sf::CircleShape avatar(20);
        avatar.setFillColor(sf::Color(100, 149, 237));
        avatar.setPosition(60, startY + i * spacing + 10);
        window->draw(avatar);

        // Nom de l’ami
        sf::Text name(friends[i], font, 20);
        name.setFillColor(sf::Color::Black);
        name.setPosition(100, startY + i * spacing + 15);
        window->draw(name);
    }
    buttons[ButtonKey::Retour] = std::make_unique<Button>(
        "Retour", font, 22, sf::Color::White, sf::Color(220, 20, 60),
        sf::Vector2f(50, window->getSize().y - 70), sf::Vector2f(150, 40));
    // Bouton retour en bas

    drawButtons();

    // Gestion des clics
    if (buttons[ButtonKey::AddFriend]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = jsonKeys::ADD_FRIEND;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = jsonKeys::MAIN;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
}

/*
Chat Menus
*/

void SFMLGame::chatMenu() {
    // Display the chat background
    displayBackground(textures->chat);

    // Sidebar for contacts list
    Rectangle sidebar(sf::Vector2f(0, 0), sf::Vector2f(200, WINDOW_HEIGHT), sf::Color(50, 50, 70), sf::Color(100, 100, 120));
    sidebar.draw(*window);

    // Header for contacts list
    Text header("Contacts", font, 24, sf::Color::White, sf::Vector2f(20, 10));
    header.draw(*window);

    if (texts.empty()) {
        // Création des champs de texte
        TextField searchField(font, 20, sf::Color::Black, sf::Color(250, 250, 250),
            sf::Vector2f(40, 50), sf::Vector2f(155, 35), "Search");
        // Champ de texte pour envoyer un message
        TextField messageField(font, 20, sf::Color::Black, sf::Color(250, 250, 250),
            sf::Vector2f(205, WINDOW_HEIGHT - 40), sf::Vector2f(WINDOW_WIDTH - 250, 35), "Enter un message");

        // Ajout des champs de texte au vecteur
        texts[TextFieldKey::SearchField] = std::make_unique<TextField>(searchField);
        texts[TextFieldKey::MessageField] = std::make_unique<TextField>(messageField);
    }

    if (buttons.empty()) {
        // Bouton pour revenir au menu principal
        Button backButton(textures->logoMain,sf::Vector2f(7, 50), sf::Vector2f(25, 35));        // Bouton pour envoyer le message
        Button sendButton(">", font, 20, sf::Color::White, sf::Color(70, 200, 70),
                          sf::Vector2f(WINDOW_WIDTH - 40, WINDOW_HEIGHT - 40), sf::Vector2f(35, 35));
        // Ajout des boutons au vecteur
        buttons[ButtonKey::Retour] = std::make_unique<Button>(backButton);
        buttons[ButtonKey::Send] = std::make_unique<Button>(sendButton);
    }

    drawTextFields();
    drawButtons();
    drawContacts();
    
    if (!client.getServerData().empty() && client.getServerData().contains("data")) {
        contacts = client.getServerData()["data"];
        client.clearServerData();
    }

    // Vérifier d'abord le backButton
    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = "main";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return; // Sortir immédiatement après avoir traité le clic
    }

    // Ensuite vérifier le sendButton
    if (buttons[ButtonKey::Send]->isClicked(*window) && !texts[TextFieldKey::MessageField]->getText().empty()) {
        json j = {
            {"message", texts[TextFieldKey::MessageField]->getText()},
        };
        network->sendData(j.dump() + "\n", client.getClientSocket());
        messages.push_back({"You", texts[TextFieldKey::MessageField]->getText()});
        texts[TextFieldKey::MessageField]->clear(); // Effacer le champ de texte après l'envoi
        return;
    }

    static std::vector<sf::Texture> avatarTextures(20); // Persistent storage for textures
    const float avatarRadius = 30.0f;
    const float contactHeight = 50.0f;
    float contactY = 100.0f;

    for (size_t i = 0; i < contacts.size() && i < 20; ++i) {
        contactY = 100 + i * contactHeight;
        bool exists = chatContacts.find(contacts[i]) != chatContacts.end();
        if (!exists) {
            // Create a new button for the contact
            Button contactButton(contacts[i], font, 20, sf::Color::White, sf::Color::Transparent,
                                 sf::Vector2f(0, contactY), sf::Vector2f(200, contactHeight), sf::Color::Transparent);
            chatContacts[contacts[i]] = std::make_unique<Button>(contactButton);
        }
        // Draw the contact avatar
        if(avatarTextures[i].loadFromFile("../../res/avatar/avatar" + std::to_string(i+1) + ".png")){
            Circle avatar(sf::Vector2f(7, contactY + 8), avatarRadius, sf::Color::White, sf::Color::Transparent);
            avatar.drawPhoto(avatarTextures[i], *window);
        }else{
            //dessiner le cercle avec la lettre initiale du contact
            //to do
        }
    }

    // Vérifier les boutons de contact
    for (const auto& [contact, button] : chatContacts) {
        if(button->isClicked(*window)) {
            std::cout << "Contact " << contact << " is clicked" << std::endl;
            json j = {
                {"action", "openChat"},
                {"contact", contact}
            };
            clickedContact = contact;
            network->sendData(j.dump() + "\n", client.getClientSocket());
            break; // Sortir de la boucle après avoir trouvé le contact cliqué
        }
    }

    // Afficher les anciens messages entre l'utilisateur et le contact après avoir cliqué sur le contact
    if (!clickedContact.empty()) {
        Rectangle sidebar(sf::Vector2f(202, 0), sf::Vector2f(WINDOW_WIDTH - 200, 50), sf::Color(50, 50, 70), sf::Color(100, 100, 120));
        sidebar.draw(*window);
        Text header(clickedContact, font, 24, sf::Color::White, sf::Vector2f(250, 10));
        header.draw(*window);
        // Draw the contact avatar
        Circle avatar(sf::Vector2f(215, 10), avatarRadius, sf::Color::White, sf::Color::Transparent);
        avatar.drawPhoto(avatarTextures[0], *window);

        // Afficher les message de l'utilisateur
        auto message = client.getServerData();
        if (message.contains("sender") && message["sender"] == clickedContact) {
            messages.push_back({message["sender"], message["message"]});
        }
        client.clearServerData();
    }
    drawMessages();
}

void SFMLGame::drawContacts() {
    // Afficher la liste des contacts
    for(auto& [contact, button] : chatContacts) {
        button->draw(*window);
    }
}


void SFMLGame::drawMessages() {
    for (auto& [sender, message] : messages) {
        displayMessage(sender, message);
        MessagesY += 40; // Adjust the Y position for each message
    }
    MessagesY = 60; // Reset Y position after all messages are drawn
}


void SFMLGame::displayMessage(const std::string& sender, const std::string& message) {
    const bool isYou = sender == "You";
    const float bubbleHeight = 30.f;
    const float bubbleWidth = std::min(300.f, message.length() * 13.f);
    const float cornerRadius = 10.f;
    const float bubbleX = isYou ? WINDOW_WIDTH - 5 - bubbleWidth : 210;
    const sf::Color bubbleColor = isYou ? sf::Color(70, 130, 180) : sf::Color(90, 90, 110);

    Rectangle bubble(sf::Vector2f(bubbleX, MessagesY), sf::Vector2f(bubbleWidth, bubbleHeight), bubbleColor, sf::Color(100, 100, 120));
    bubble.draw(*window);

    // Draw message text
    Text messageText(message, font, 16, sf::Color::White, sf::Vector2f(bubbleX + cornerRadius + 5.f, MessagesY));
    messageText.draw(*window);
}
/*
Game Menus
*/

void SFMLGame::CreateOrJoinGame() {
    displayBackground(textures->logoConnexion);

    if (buttons.empty()) {
        std::cout << "Creating buttons" << std::endl;

        // Création des boutons
        buttons[ButtonKey::Create] = std::make_unique<Button>("Créer", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                                                              sf::Vector2f(310, 300), sf::Vector2f(180, 45));
        buttons[ButtonKey::Join] = std::make_unique<Button>("Rejoindre", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                                                            sf::Vector2f(310, 360), sf::Vector2f(180, 45));

        buttons[ButtonKey::Quit] = std::make_unique<Button>(textures->logoExit, sf::Vector2f(10, 20), sf::Vector2f(40, 40));

        buttons[ButtonKey::Settings] = std::make_unique<Button>(textures->logoSettings, sf::Vector2f(WINDOW_WIDTH - 130, 20), sf::Vector2f(35, 35));

        buttons[ButtonKey::Notification] = std::make_unique<Button>(textures->logoNotification, sf::Vector2f(WINDOW_WIDTH - 190, 20), sf::Vector2f(45, 45));

        buttons[ButtonKey::Profile] = std::make_unique<Button>("", font, 24, sf::Color::Transparent, sf::Color::White,
                                                               sf::Vector2f(WINDOW_WIDTH - 70, 20), sf::Vector2f(35, 35), sf::Color::Transparent);
        // buttons[ButtonKey::Profile]->drawPhoto(avatarduClient);
    }

    // Dessiner les boutons
    drawButtons();

    // Gérer les clics sur les boutons
    json j;
    if (buttons.count(ButtonKey::Create) && buttons[ButtonKey::Create]->isClicked(*window)) {
        j[jsonKeys::ACTION] = "choiceMode";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    } 
    // TO DO FOR 
}

void SFMLGame::ChoiceGameMode(){
    displayBackground(textures->logoConnexion);
    if (buttons.empty()) {
        std::cout << "Creating buttons" << std::endl;

        // Création des boutons
        buttons[ButtonKey::Endless] = std::make_unique<Button>("Mode Endless", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                                                               sf::Vector2f(310, 250), sf::Vector2f(180, 45));
        buttons[ButtonKey::Duel] = std::make_unique<Button>("Mode Duel", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                                                            sf::Vector2f(310, 310), sf::Vector2f(180, 45));
        buttons[ButtonKey::Classic] = std::make_unique<Button>("Mode Classic", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                                                               sf::Vector2f(310, 370), sf::Vector2f(180, 45));
        buttons[ButtonKey::Royale] = std::make_unique<Button>("Mode Royale", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                                                              sf::Vector2f(310, 430), sf::Vector2f(180, 45));
        buttons[ButtonKey::Quit] = std::make_unique<Button>(textures->logoExit, sf::Vector2f(10, 20), sf::Vector2f(40, 40));
    }
    
    //draw buttons
    drawButtons();
    json j;
    
    if (buttons.count(ButtonKey::Endless) && buttons[ButtonKey::Endless]->isClicked(*window)) {
        j[jsonKeys::ACTION] = "EndlessMode";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    } else if (buttons.count(ButtonKey::Duel) && buttons[ButtonKey::Duel]->isClicked(*window)) {
        //j[jsonKeys::ACTION] = "DuelMode";
        //network->sendData(j.dump() + "\n", client.getClientSocket());
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

    if (buttons.empty()) {
        buttons[ButtonKey::Rejouer] = std::make_unique<Button>("Rejouer", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                                                               sf::Vector2f(200, 310), sf::Vector2f(180, 45));
        buttons[ButtonKey::Retour] = std::make_unique<Button>("Retour au menu", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                                                              sf::Vector2f(450, 310), sf::Vector2f(180, 45));
    }
    
    drawButtons();

    json j;
    
    if (buttons.count(ButtonKey::Rejouer) && buttons[ButtonKey::Rejouer]->isClicked(*window)) {
        j[jsonKeys::ACTION] = "createjoin";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        client.setGameStateUpdated(false);
        client.setGameStateIsEnd(false);
        return;
    } else if (buttons.count(ButtonKey::Retour) && buttons[ButtonKey::Retour]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::MAIN;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        client.setGameStateUpdated(false);
        client.setGameStateIsEnd(false);
        return;
    }

}
void SFMLGame::handleResize(unsigned int newWidth, unsigned int newHeight) {
    // Ajuster la vue pour correspondre à la nouvelle taille de la fenêtre
    sf::View view = window->getView();
    view.setSize(newWidth, newHeight);
    view.setCenter(newWidth / 2.0f, newHeight / 2.0f);
    window->setView(view);

    // Recalculer les positions et tailles des éléments
    float scaleX = static_cast<float>(newWidth) / WINDOW_WIDTH;
    float scaleY = static_cast<float>(newHeight) / WINDOW_HEIGHT;

    // Redimensionner les boutons
    for (auto& [_, button] : buttons) {
        button->resize(scaleX, scaleY);
    }

    // Redimensionner les champs de texte
    for (auto& [_, text] : texts) {
        text->resize(scaleX, scaleY);
    }

    // Redimensionner les autres éléments si nécessaire
    // Exemple : ajuster la taille des messages ou des grilles
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
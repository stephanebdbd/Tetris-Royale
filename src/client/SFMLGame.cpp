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
#include <set>


const unsigned int WINDOW_WIDTH = 1500;
const unsigned int WINDOW_HEIGHT = 850;
const std::string WINDOW_TITLE = "Tetris Royal";

SFMLGame::SFMLGame(Client& client) : 
    client(client),
    window(std::make_unique<sf::RenderWindow>()), 
    network(std::make_unique<ClientNetwork>()),
    textures(std::make_unique<Textures>()),
    currentState(MenuState::Welcome),
    avatarManager(std::make_unique<AvatarManager>(window.get())),
    menuManager(std::make_unique<MenuManager>(window.get(), font, client, *network, *textures, 
    this, *avatarManager, this->buttons, this->texts))
{
    std::cout << "SFMLGame constructor called" << std::endl;

    if (!font.loadFromFile(FONT_PATH)) {
        std::cerr << "Erreur: Impossible de charger la police." << std::endl;
    }
    std::vector<std::string> paths;
    for (int i = 1; i <= 10; ++i) {
        paths.push_back("../../res/avatar/avatar" + std::to_string(i) + ".png");
    }
    avatarManager->loadAvatarPaths(paths);

    textures->loadTextures();
}

void SFMLGame::update() {
    // Autres mises à jour de logique du jeu...
    std::string tempMessage = client.getTemporaryMessage();
    if (!tempMessage.empty()) {
        afficherErreur(tempMessage);  // Appelle ta méthode d'affichage
        client.setTemporaryMessage("");  // Efface le message temporaire après l'avoir affiché
    }
}

void SFMLGame::afficherErreur(const std::string& message) {
    // Affiche le panneau pendant 3 secondes
    erreurMessageActuel = message;
    erreurClock.restart();
    afficherErreurActive = true;
}

void SFMLGame::drawErreurMessage() {
    if (!afficherErreurActive) return;

    if (erreurClock.getElapsedTime().asSeconds() >= 3.0f) {
        afficherErreurActive = false;
        return;
    }

    // Créer un panneau semi-transparent
    const auto& panneuxSize = sf::Vector2f(500, 100);
    Rectangle panneau(sf::Vector2f(WINDOW_WIDTH/2 - 500/2, 250), panneuxSize, sf::Color(200, 0, 0, 200), sf::Color::White);
    panneau.draw(*window);

    // Créer un texte d'erreur
    Text texteErreur(erreurMessageActuel, font, 22, sf::Color::White, sf::Vector2f(0, 0));
    sf::FloatRect bounds = texteErreur.getLocalBounds();
    float posX = panneau.getPosition().x + (panneuxSize.x - bounds.width) / 2 - bounds.left;
    float posY = panneau.getPosition().y + (panneuxSize.y - bounds.height) / 2 - bounds.top;
    texteErreur.setPosition(posX, posY);
    texteErreur.draw(*window);

}


void SFMLGame::drawButtons() {
    drawErreurMessage();

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


// Gestion des boutons
void SFMLGame::handleButtonEvents() {
    for (const auto& [_, button] : buttons) {
        button->update();
        button->setBackgroundColor(*window);
    }
    for (const auto& [_, contact] :  chatContacts){
        contact->update();
        contact->setBackgroundColor(*window);
    }

    for (const auto& [_, inviteButtons] :  inviteFriends){
        inviteButtons[0]->update();
        inviteButtons[0]->setBackgroundColor(*window);

        inviteButtons[1]->update();
        inviteButtons[1]->setBackgroundColor(*window);
    }

    for (const auto& [_, buttonsA] : acceptInvite) {
        buttonsA->update();
        buttonsA->setBackgroundColor(*window);  
    }

    quitter->update();
    quitter->setBackgroundColor(*window);
}

void SFMLGame::cleanup() {
    buttons.clear();
    texts.clear();
    chatContacts.clear();
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

                else if (currentState == MenuState::Settings) {
                    if(invite){
                        inviteScrollOffset -= event.mouseWheelScroll.delta * inviteScrollSpeed;
                        inviteScrollOffset = std::clamp(inviteScrollOffset, 0.0f, inviteMaxScroll); // Limiter le défilement
                    }
                    
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
            menuManager->handleTextFieldEvents(event);
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
        previousState = currentState;
        currentState = newState;
        std::cout << "State changed to: " << static_cast<int>(currentState) << std::endl;
        this->cleanup();
    }

    window->clear();
    
    switch (currentState) {
        case MenuState::Welcome:
            menuManager->welcomeMenu();
            break;
        case MenuState::Register:
            menuManager->registerMenu();
            break;
        case MenuState::Login:
            menuManager->connexionMenu();
            break;
        case MenuState::Main:
            menuManager->mainMenu();
            break;
        case MenuState::classement:
            menuManager->rankingMenu();
            break;
        case MenuState::Settings:
            displayWaitingRoom();  
            break;
        case MenuState::Friends:
            menuManager->friendsMenu();
            break;
        case MenuState::FriendList:
            menuManager->friendListMenu();
            break;
        case MenuState::AddFriend:
            menuManager->addFriendMenu();
            break;
        case MenuState::FriendRequestList:
            menuManager->friendRequestListMenu();
            break;
        case MenuState::chat:
            menuManager->chatMenu();
            break;
        case MenuState::Team:
            menuManager->teamsMenu();
            break;
        case MenuState::CreatTeamMenu:
            menuManager->createRoomMenu();
            break;
        case MenuState::JoinTeam:
            menuManager->joinTeamMenu();
            break;
        case MenuState::ManageTeams:
            menuManager->displayRoomsMenu();
            break;
        case MenuState::ManageTeam:
            menuManager->manageTeamMenu();
            break;
        case MenuState::JoinOrCreateGame:
            CreateOrJoinGame();
            break;
        case MenuState::JoinGame:
            displayJoinGame();
            break;
        case MenuState::CreateGame:
            ChoiceGameMode();
            break;
        case MenuState::Play:
            displayGame();
            break;
        case MenuState::Observer:
            displayGame();
            break;
        case MenuState::GameOver:
            drawEndGame();
            break;

        default:
            std::cerr << "Unhandled MenuState: " << static_cast<int>(currentState) << std::endl;
            break;
    }
    client.clearServerData();
    window->display();
    sleep(0.1);
}

// Compléter le switch dans run()
void SFMLGame::run() {
    // Début de thread du client
    std::thread clientThread([this]() { client.run("gui"); });
    clientThread.detach();

    window->create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
    window->setFramerateLimit(60);

    while (window->isOpen()) {
        update();
        handleEvents();
        refreshMenu();
    }
    messages.clear();
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


void SFMLGame::displayCurrentPlayerInfo() {
    // Récupérer les informations du joueur actuel
    auto username = client.getPlayerInfo()[0];
    auto score = client.getPlayerInfo()[1];
    auto level = stoi(score) % 1000;

    float infoBoxWidth = 300;
    float infoBoxHeight = 100;
    float infoBoxX = WINDOW_WIDTH - infoBoxWidth - 20; // Position à droite avec une marge de 20px
    float infoBoxY = 80; 

    // Fond du rectangle
    sf::RectangleShape infoBox(sf::Vector2f(infoBoxWidth, infoBoxHeight));
    infoBox.setFillColor(sf::Color(50, 50, 70, 200));   
    infoBox.setOutlineColor(sf::Color::White);          // Bordure blanche
    infoBox.setOutlineThickness(2);
    infoBox.setPosition(infoBoxX, infoBoxY);

    // Texte pour le nom du joueur
    Text playerName("Nom: " + username, font, 20, sf::Color::White, sf::Vector2f(infoBoxX + 10, infoBoxY + 10));
    playerName.draw(*window);

    // Texte pour le score du joueur
    Text playerScore("Score: " + score, font, 20, sf::Color::White, sf::Vector2f(infoBoxX + 10, infoBoxY + 40));
    playerScore.draw(*window);
    // Texte pour le niveau du joueur
    Text playerLevel("Niveau: " + std::to_string(level), font, 20, sf::Color::White, sf::Vector2f(infoBoxX + 10, infoBoxY + 70));
    playerLevel.draw(*window);

    //  pour fermer la fenêtre
    if (!buttons.count(ButtonKey::Close)) {
        buttons[ButtonKey::Close] = std::make_unique<Button>("X", font, 20, sf::Color::White, sf::Color::Red,
                                                             sf::Vector2f(infoBoxX + infoBoxWidth - 20, infoBoxY - 3),
                                                             sf::Vector2f(20, 20)); // Position en haut à droite de la fenêtre
    }

    if (buttons[ButtonKey::Close]->isClicked(*window)) {    // pour fermer la boîte d'informations
        client.setShow(false);
        buttons.erase(ButtonKey::Close);
        return;
    }

    
    window->draw(infoBox);
    playerName.draw(*window);
    playerScore.draw(*window);
    playerLevel.draw(*window);
    buttons[ButtonKey::Close]->draw(*window);
}


void SFMLGame::handleContacts() {
    const auto& contacts = client.getContacts(); 
    if (contacts.empty()) return;
    static std::vector<sf::Texture> avatarTextures(20);
    const float contactHeight = 50.0f;
    
    // Gestion des contacts
    for (std::size_t i = 0; i < std::min(contacts.size(), avatarTextures.size()); ++i) {
        const auto& [contactName, avatarIndex] = contacts[i];
        float contactY = 100 + i * contactHeight;

        const auto& circleSize = sf::Vector2f(7, contactY + 5);
        const auto& circleRadius = 20.0f;

        if (!chatContacts.count(contactName)) {
            chatContacts[contactName] = std::make_unique<Button>(
                contactName, font, 20, sf::Color::White, sf::Color::Transparent,
                sf::Vector2f(0, contactY), sf::Vector2f(200, contactHeight), sf::Color::Transparent);
        }

        if (avatarIndex >= 0 && avatarIndex < static_cast<int>(avatarManager->getAvatarPath().size())) {
            if (avatarTextures[avatarIndex].loadFromFile(avatarManager->getAvatarPath()[avatarIndex])) {
                Circle circle(circleSize, circleRadius, sf::Color::White, sf::Color::Transparent);
                circle.setTexture(avatarTextures[avatarIndex]);
                circle.draw(*window);
            }
        }

        else{
            Circle circle(circleSize, circleRadius, sf::Color(100, 100, 200), sf::Color::Transparent);
            circle.draw(*window);

            // Dessiner la première lettre du nom dans le cercle
            Text initial(contactName.substr(0, 1), font, 20, sf::Color::White, circleSize);

            // Centrer la lettre dans le cercle
            sf::FloatRect textBounds = initial.getLocalBounds();
            initial.setPosition(
                circleSize.x + circleRadius - textBounds.width / 2 - textBounds.left,
                circleSize.y + circleRadius - textBounds.height / 2 - textBounds.top
            );
            initial.draw(*window);
        }
        //gestion des clicks
        const auto& Button = chatContacts[contactName];
        if(Button->isClicked(*window)){
            clickedContact = contactName;
            avatarManager->setAvatarClickedContact(avatarIndex);
            network->sendData(json{{"action", "openChat"}, {"contact", contactName}}.dump() + "\n", 
                        client.getClientSocket());
        }
        
    }

    // Affichage du chat sélectionné
    if (!clickedContact.empty()) {
        Rectangle(sf::Vector2f(202, 0), sf::Vector2f(WINDOW_WIDTH - 200, 50),
        sf::Color(50, 50, 70), sf::Color(100, 100, 120)).draw(*window);

        Text(clickedContact, font, 24, sf::Color::White, sf::Vector2f(250, 10)).draw(*window);

        Circle circle(sf::Vector2f(210, 8), 20.0f, sf::Color::White, sf::Color::Transparent);
        circle.setTexture(avatarTextures[avatarManager->getAvatarClickedContact()]);
        circle.draw(*window);
    }

}


void SFMLGame::getMessagesFromServer() {
    try {
        auto message = client.getServerData();
        if (message.contains("sender")) {
            messages.emplace_back(std::move(message));
            if (messages.size() > 100) {
                messages.erase(messages.begin());
            }
        }
        if(!message.empty())
            std::cout << "Messages : " << message << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error processing message: " << e.what() << std::endl;

    }
    
}

void SFMLGame::drawContacts() {
    // Afficher la liste des contacts
    for(auto& [contact, button] : chatContacts) {
        button->draw(*window);
    }
}


void SFMLGame::drawMessages() {
    int startIdx = std::max(0, static_cast<int>(messages.size()) - 18);
    for (int i = startIdx; i < static_cast<int>(messages.size()); ++i) {
        auto& msg = messages[i];
        auto sender = msg["sender"];
        auto receiver = msg["receiver"];
        if ((sender == clickedContact) || (sender == "You" && receiver == clickedContact)) {
            displayMessage(sender, msg["message"]);
        }
    }
    MessagesY = 60; // Reset Y position after all messages are drawn
}


void SFMLGame::displayMessage(const std::string& sender, const std::string& message) {
    const bool isYou = sender == "You";

    // Définir les paramètres de style
    const unsigned int fontSize = 16;
    const float padding = 10.f;
    const float maxWidth = 500.f;
    const float margin = 5.f;
    const float leftStartX = 210.f;

    // Créer l'objet texte temporaire pour mesurer
    sf::Text tempText(message, font, fontSize);
    tempText.setFillColor(sf::Color::White);
    tempText.setPosition(0, 0); // temporaire

    // Ajuster le texte à une largeur maximale
    std::string wrappedText = wrapText(message, font, fontSize, maxWidth - 2 * padding);
    sf::Text messageText(wrappedText, font, fontSize);
    sf::FloatRect bounds = messageText.getLocalBounds();

    // Taille finale de la bulle
    float bubbleWidth = std::min(maxWidth, bounds.width + 2 * padding);
    float bubbleHeight = bounds.height + 2 * padding;

    // Position de la bulle
    float bubbleX = isYou ? WINDOW_WIDTH - margin - bubbleWidth : leftStartX;
    sf::Color bubbleColor = isYou ? sf::Color(70, 130, 180) : sf::Color(90, 90, 110);

    // Créer et dessiner la bulle
    Rectangle bubble(sf::Vector2f(bubbleX, MessagesY), sf::Vector2f(bubbleWidth, bubbleHeight), bubbleColor, sf::Color(100, 100, 120));
    bubble.draw(*window);

    // Positionner et dessiner le texte
    messageText.setPosition(bubbleX + padding, MessagesY + padding / 2);
    window->draw(messageText);

    // Avancer la position Y pour le prochain message
    MessagesY += bubbleHeight + 10.f;
}

std::string SFMLGame::wrapText(const std::string& text, const sf::Font& font, unsigned int characterSize, float maxWidth) {
    std::string result;
    std::string word;
    sf::Text temp("", font, characterSize);

    for (char c : text) {
        if (c == ' ' || c == '\n') {
            temp.setString(result + word);
            if (temp.getLocalBounds().width > maxWidth) {
                result += '\n' + word;
            } else {
                result += word;
            }
            result += c;
            word.clear();
        } else {
            word += c;
        }
    }

    // Dernier mot
    if (!word.empty()) {
        temp.setString(result + word);
        if (temp.getLocalBounds().width > maxWidth) {
            result += '\n' + word;
        } else {
            result += word;
        }
    }

    return result;
}


/*
Game Menus
*/

void SFMLGame::CreateOrJoinGame() {
    displayBackground(textures->rejoindre);

    if (buttons.empty()) {
        std::cout << "Creating buttons" << std::endl;

        // Création des boutons
        buttons[ButtonKey::Create] = std::make_unique<Button>("Creer", font, 50, sf::Color::White, sf::Color(34, 139, 34),
                                                              sf::Vector2f(400, 500), sf::Vector2f(250, 60));
        buttons[ButtonKey::Join] = std::make_unique<Button>("Rejoindre", font, 50, sf::Color::White, sf::Color(184, 134, 11),
                                                            sf::Vector2f(850, 500), sf::Vector2f(250, 60));

        buttons[ButtonKey::Quit] = std::make_unique<Button>(textures->logoExit, sf::Vector2f(10, 20), sf::Vector2f(40, 40));

        buttons[ButtonKey::Settings] = std::make_unique<Button>(textures->logoSettings, sf::Vector2f(WINDOW_WIDTH - 130, 20), sf::Vector2f(35, 35));

        buttons[ButtonKey::Notification] = std::make_unique<Button>(textures->logoNotification, sf::Vector2f(WINDOW_WIDTH - 190, 20), sf::Vector2f(45, 45));

        buttons[ButtonKey::Profile] = std::make_unique<Button>("", font, 24, sf::Color::Transparent, sf::Color::White,
                                                               sf::Vector2f(WINDOW_WIDTH - 70, 20), sf::Vector2f(35, 35), sf::Color::Transparent);
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
    else if (buttons.count(ButtonKey::Join) && buttons[ButtonKey::Join]->isClicked(*window)){
        j[jsonKeys::ACTION] = "Rejoindre";
        client.sendInputFromSFML("2");
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
    else if(buttons.count(ButtonKey::Quit) && buttons[ButtonKey::Quit]->isClicked(*window)){
        j[jsonKeys::ACTION] = jsonKeys::MAIN;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
    else if (buttons.count(ButtonKey::Settings) && buttons[ButtonKey::Settings]->isClicked(*window)) {
        // Action pour le bouton "Settings"
    } else if (buttons.count(ButtonKey::Notification) && buttons[ButtonKey::Notification]->isClicked(*window)) {
        // Action pour le bouton "Notification"
    } else if (buttons.count(ButtonKey::Profile) && buttons[ButtonKey::Profile]->isClicked(*window)) {
        // Action pour le bouton "Profile"
    }
    int avatarIndex = client.getAvatarIndex();
    avatarManager->drawAvatar(avatarIndex, WINDOW_WIDTH - 70, 20, 45.0f);
}

void SFMLGame::ChoiceGameMode(){
    displayBackground(textures->mode);
    if (buttons.empty()) {
        std::cout << "Creating buttons" << std::endl;

        // Création des boutons
        buttons[ButtonKey::Endless] = std::make_unique<Button>("ENDLESS", font, 30, sf::Color::White, sf::Color(255, 255, 255, 0),
                                                               sf::Vector2f(340, 490), sf::Vector2f(160, 45), sf::Color(255, 255, 255, 0));

        buttons[ButtonKey::Duel] = std::make_unique<Button>("DUEL", font, 30, sf::Color::White, sf::Color(255, 255, 255, 0),
                                                            sf::Vector2f(375, 640), sf::Vector2f(100, 45), sf::Color(255, 255, 255, 0));

        buttons[ButtonKey::Classic] = std::make_unique<Button>("CLASSIC", font, 30, sf::Color::White, sf::Color(255, 255, 255, 0),
                                                               sf::Vector2f(980, 490), sf::Vector2f(150, 45), sf::Color(255, 255, 255, 0));

        buttons[ButtonKey::Royale] = std::make_unique<Button>("ROYALE", font, 30, sf::Color::White, sf::Color(255, 255, 255, 0),
                                                              sf::Vector2f(980, 640), sf::Vector2f(150, 45), sf::Color(255, 255, 255, 0));

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
        duel = true;
        j[jsonKeys::ACTION] = "DuelMode";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    else if(buttons.count(ButtonKey::Classic) && buttons[ButtonKey::Classic]->isClicked(*window)){
        classic = true;
        j[jsonKeys::ACTION] = "ClassicMode";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }else if(buttons.count(ButtonKey::Royale) && buttons[ButtonKey::Royale]->isClicked(*window)){
        royale = true;
        j[jsonKeys::ACTION] = "RoyaleMode";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }else if(buttons.count(ButtonKey::Quit) && buttons[ButtonKey::Quit]->isClicked(*window)){
        j[jsonKeys::ACTION] = "createjoin";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
    int avatarIndex = client.getAvatarIndex();
    avatarManager->drawAvatar(avatarIndex, WINDOW_WIDTH - 70, 20, 45.0f);

}
void SFMLGame::displayGame(){
    
    GameState gameData = client.getGameState();

    if (client.isGameStateUpdated()) {
        
        if(gameData.isGame){
            drawGrid(gameData.gridData);
            drawTetramino(gameData.currentPieceData);
            drawTetramino(gameData.nextPieceData);
            drawScore(gameData.scoreData);
            drawPlayerNumber(gameData.playerNumberData);
            drawMessageMalusBonus(gameData.message);
            if(gameData.miniUpdate){
                int i = 0;
                for(const auto& miniGrid : gameData.miniGrid) {
                    drawMiniGrid(miniGrid["grid"], miniGridPositions[i]);
                    drawMiniTetra(miniGrid["tetra"], miniGridPositions[i]);
                    drawminiPlayerId(miniGrid["playerId"], miniGridPositions[i]);
                    i++;
                }
            }
        } 
    }
}

void SFMLGame::drawminiPlayerId(const json& playerId, sf::Vector2f pos) {
    int playerNumber = playerId;
    sf::Text playerText;
    playerText.setFont(font);
    playerText.setString(std::to_string(playerNumber));
    playerText.setCharacterSize(15);
    playerText.setFillColor(sf::Color::White);
    playerText.setPosition(pos.x - 10, pos.y); 

    window->draw(playerText);
}

void SFMLGame::drawMiniGrid(const json& miniGrid, sf::Vector2f pos) {
    // Récupération des informations


    if(miniGrid[jsonKeys::LIGHT_GRID]) return;
    int width = miniGrid[jsonKeys::WIDTH]; 
    int height = miniGrid[jsonKeys::HEIGHT]; 
    const json& cells = miniGrid[jsonKeys::CELLS];
    sf::Color color ;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            sf::RectangleShape cell(sf::Vector2f(15 - 1, 15 - 1)); 
            cell.setPosition(x * 15 + pos.x, y * 15 + pos.y);  
            
            bool occupied = cells[y][x][jsonKeys::OCCUPIED]; 
            if (occupied) {
                int colorValue = cells[y][x][jsonKeys::COLOR];
                sf::Color color = fromSFML(colorValue);
                cell.setFillColor(color);
            }else{
                cell.setFillColor(sf::Color(200, 200, 200));
            }

            window->draw(cell);  
        }
    }
    
}

void SFMLGame::drawMiniTetra(const json& miniTetra, sf::Vector2f pos) {
    // Récupération des informations
    if(miniTetra[jsonKeys::LIGHT_TETRA]) return;
    int x = miniTetra[jsonKeys::X];
    int y = miniTetra[jsonKeys::Y];
    std::vector<std::vector<std::string>> shape = miniTetra[jsonKeys::SHAPE];
    int shapeSymbol = miniTetra[jsonKeys::SHAPE_SYMBOL];

    // Taille d’un bloc de tétrimino en pixels

    sf::Color color = SFMLGame::fromShapeSymbolSFML(std::string(1, shapeSymbol));

    for (std::size_t row = 0; row < shape.size(); ++row) {
        for (std::size_t col = 0; col < shape[row].size(); ++col) {
            if (shape[row][col][0] != ' ') {
                sf::RectangleShape block(sf::Vector2f(15-1, 15-1));
                block.setFillColor(color);

                block.setPosition(((x + col) * 15 + pos.x - 15) , (y + row) * 15 + pos.y);

                window->draw(block);
            }
        }
    }

}

void SFMLGame::drawGrid(const json& grid) {
    if(grid[jsonKeys::LIGHT_GRID]) return;
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
                cell.setFillColor(sf::Color(200, 200, 200));
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
    if(tetraPiece[jsonKeys::LIGHT_TETRA]) return;
    int x = tetraPiece[jsonKeys::X];
    int y = tetraPiece[jsonKeys::Y];
    std::vector<std::vector<std::string>> shape = tetraPiece[jsonKeys::SHAPE];
    int shapeSymbol = tetraPiece[jsonKeys::SHAPE_SYMBOL];

    // Taille d’un bloc de tétrimino en pixels

    sf::Color color = SFMLGame::fromShapeSymbolSFML(std::string(1, shapeSymbol));

    for (std::size_t row = 0; row < shape.size(); ++row) {
        for (std::size_t col = 0; col < shape[row].size(); ++col) {
            if (shape[row][col][0] != ' ') {
                sf::RectangleShape block(sf::Vector2f(cellSize-1, cellSize-1));
                block.setFillColor(color);

                block.setPosition(((x + col) * cellSize) , (y + row) * cellSize);

                window->draw(block);
            }
        }
    }
}

void SFMLGame::drawEndGame() {
    GameState gameData = client.getGameState();
    client.reintiliseData();
    json endGameData = gameData.menu;
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
                                                               sf::Vector2f(400, 310), sf::Vector2f(180, 45));
        buttons[ButtonKey::Retour] = std::make_unique<Button>("Retour au menu", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                                                              sf::Vector2f(650, 310), sf::Vector2f(180, 45));
    }
    
    drawButtons();

    json j;
    
    if (buttons.count(ButtonKey::Rejouer) && buttons[ButtonKey::Rejouer]->isClicked(*window)) {
        j[jsonKeys::ACTION] = "createjoin";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    } else if (buttons.count(ButtonKey::Retour) && buttons[ButtonKey::Retour]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::MAIN;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        client.setGameStateUpdated(false);
        return;
    }

}
void SFMLGame::handleResize(unsigned int newWidth, unsigned int newHeight) {
    // Calculer le ratio d’aspect original et celui de la nouvelle taille
    float windowRatio = static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT;
    float newRatio = static_cast<float>(newWidth) / newHeight;

    // Créer une nouvelle vue basée sur la taille logique du jeu
    sf::View view(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));

    if (newRatio > windowRatio) {
        // La fenêtre est trop large → bandes sur les côtés (letterbox horizontal)
        float width = WINDOW_HEIGHT * newRatio;
        float offsetX = (width - WINDOW_WIDTH) / 2.f;
        view.setViewport(sf::FloatRect(-offsetX / width, 0, WINDOW_WIDTH / width, 1));
    } else {
        // La fenêtre est trop haute → bandes en haut/bas (letterbox vertical)
        float height = WINDOW_WIDTH / newRatio;
        float offsetY = (height - WINDOW_HEIGHT) / 2.f;
        view.setViewport(sf::FloatRect(0, -offsetY / height, 1, WINDOW_HEIGHT / height));
    }

    window->setView(view);

    // Mettre à jour l’échelle pour redimensionner les éléments
    float scaleX = static_cast<float>(newWidth) / WINDOW_WIDTH;
    float scaleY = static_cast<float>(newHeight) / WINDOW_HEIGHT;

    for (auto& [_, button] : buttons) {
        button->resize(scaleX, scaleY);
    }

    for (auto& [_, text] : texts) {
        text->resize(scaleX, scaleY);
    }

    // Redimensionner d'autres éléments si besoin
}

sf::Color SFMLGame::fromShapeSymbolSFML(const std::string& symbol) {
    if (symbol == "I") return sf::Color::Cyan;
    if (symbol == "O") return sf::Color::Yellow;
    if (symbol == "T") return sf::Color(128, 0, 128); // Violet
    if (symbol == "S") return sf::Color::Green;
    if (symbol == "Z") return sf::Color::Red;
    if (symbol == "J") return sf::Color::Blue;
    if (symbol == "L") return sf::Color(255, 165, 0); // Orange
    return sf::Color(100,100,100); // Default
}

sf::Color SFMLGame::fromSFML(int value) {
    if (value == 4) return sf::Color::Cyan;
    if (value == 2) return sf::Color::Yellow;
    if (value == 6) return sf::Color(128, 0, 128); // Violet
    if (value == 3) return sf::Color::Green;
    if (value == 1) return sf::Color::Red;
    if (value == 5) return sf::Color::Blue;
    if (value == 7) return sf::Color(255, 165, 0); // Orange
    return sf::Color(100,100,100); // Default
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
void SFMLGame::drawPlayerNumber(const json& playerData) {
    int playerNumber = playerData;
    sf::Text playerText;
    playerText.setFont(font);
    playerText.setString("Joueur: " + std::to_string(playerNumber));
    playerText.setCharacterSize(24);
    playerText.setFillColor(sf::Color::White);
    playerText.setPosition(cellSize * 12 + 10, 3 * cellSize +15); 

    window->draw(playerText);
}



void SFMLGame::displayWaitingRoom() {
    window->clear(sf::Color(30, 30, 60));
    

    Text title("ATTENTE DANS LE LOBBY", font, 50, sf::Color::White, sf::Vector2f(WINDOW_WIDTH/2 - 250, 30));
    
    title.draw(*window);
    Rectangle frameCommand(sf::Vector2f(20, 120), sf::Vector2f(500, 450), sf::Color::Transparent, sf::Color(135, 206, 250));
    frameCommand.draw(*window);
    

    if (client.isGameStateUpdated()) {
        GameState gameData = client.getGameState();
        json lines = gameData.menu[jsonKeys::OPTIONS];

        Rectangle framestate(sf::Vector2f(550, 120), sf::Vector2f(470, 450), sf::Color::Transparent, sf::Color(135, 206, 250));
        framestate.draw(*window);

        Rectangle frameActivePlayer(sf::Vector2f(1060, 120), sf::Vector2f(215, 450), sf::Color::Transparent, sf::Color(135, 206, 250));
        frameActivePlayer.draw(*window);
        Rectangle frameActiveObserver(sf::Vector2f(1275, 120), sf::Vector2f(215, 450), sf::Color::Transparent, sf::Color(135, 206, 250));
        frameActiveObserver.draw(*window);

        float startY = 130; // Position de départ pour les amis
        float spacing = 30; // Espacement entre les amis

        if(!gameData.pseudos.empty()){
            pseudos = gameData.pseudos;
        }
    
        if(!gameData.friendsLobby.empty()){
            friendsLobby = gameData.friendsLobby;
        }
        if(!gameData.showCommand.empty()){
            if(gameData.showCommand == "player")
                showCommand = false;
            else if(gameData.showCommand == "observer"){
                showCommand = false;
                showInviteCommand = false;
            }
                
            
        }

        Text player("JOUEURS: " , font, 20, sf::Color::White, sf::Vector2f(1065, startY));
        player.draw(*window);
        Text observer("OBSERVATEUR: " , font, 20, sf::Color::White, sf::Vector2f(1280, startY));
        observer.draw(*window);
        if(!pseudos.empty()){
            for(std::size_t i = 0; i < pseudos["player"].size(); ++i){
            
                Text pseudoText(pseudos["player"][i], font, 20, sf::Color::White, sf::Vector2f(1070, spacing + startY + i * spacing));
                pseudoText.draw(*window);
                
            }
            for(std::size_t i = 0; i < pseudos["observer"].size(); ++i){
                
                Text pseudoText(pseudos["observer"][i], font, 20, sf::Color::White, sf::Vector2f(1290, spacing + startY + i * spacing));
                pseudoText.draw(*window);
                
            }
        }
        
        std::string line;
        int i = 0;
        for (auto& [key, value] : lines.items()) {
            line =  value.get<std::string>();
            if (line.rfind("/help", 0) == 0 || line.rfind("/back", 0) == 0) {
                continue; // Ignore cet élément
            }

            std::size_t colonPos = line.find(":");
            std::string beforeColon = (colonPos != std::string::npos) ? line.substr(0, colonPos + 1) : line; // Inclut ":"
            std::string afterColon = (colonPos != std::string::npos) ? line.substr(colonPos + 1) : "";
        
            // Dessiner la partie avant ":" en gris
            Text beforeText(beforeColon, font, 20, sf::Color(150, 150, 150), sf::Vector2f(570, 130 + i));
            beforeText.draw(*window);
        
            // Dessiner la partie après ":" en blanc
            if (!afterColon.empty()) {
                Text afterText(afterColon, font, 20, sf::Color::White, sf::Vector2f(570 + beforeText.getLocalBounds().width, 130 + i));
                afterText.draw(*window);
            }
            i+=50;
        }
    }

    if(invite){
        if(cl){
            cleanup();
            cl = false;
        }
    
        Rectangle frameEsc(sf::Vector2f(WINDOW_WIDTH/2 - 275, 90), sf::Vector2f(550, 30), sf::Color(0, 0, 139), sf::Color(0, 0, 255));
        frameEsc.draw(*window);


        Rectangle invitePage(sf::Vector2f(WINDOW_WIDTH/2 - 275, 120), sf::Vector2f(550, 450), sf::Color(30, 30, 60), sf::Color(0, 0, 255));
        invitePage.draw(*window);

        // Définir la vue pour le rectangle défilable
        inviteView.setViewport(sf::FloatRect(0.25f, 0.16f, 0.5f, 0.6f)); // Position et taille relatives à la fenêtre
        inviteView.setCenter(WINDOW_WIDTH / 2, 120 + 450 / 2 + inviteScrollOffset); // Ajuster la position de la vue
        inviteView.setSize(550, 450); // Taille de la vue
        window->setView(inviteView);

        
        // Dessiner la liste des amis
        float startY = 130; // Position de départ pour les amis
        float spacing = 50; // Espacement entre les amis
        float totalHeight = friendsLobby.size() * spacing; // Hauteur totale du contenu
        inviteMaxScroll = std::max(0.0f, totalHeight - 450); // Ajuster la hauteur maximale de défilement

        for (std::size_t i = 0; i < friendsLobby.size(); ++i) {
            Text friendName(friendsLobby[i], font, 20, sf::Color::White, sf::Vector2f(WINDOW_WIDTH / 2 - 200, startY + i * spacing));

            if (!inviteFriends.count(friendsLobby[i])){
                inviteFriends[friendsLobby[i]].push_back(std::make_unique<Button>(textures->player, sf::Vector2f(800, startY + i * spacing - 5), sf::Vector2f(50, 50)));
                inviteFriends[friendsLobby[i]].push_back(std::make_unique<Button>(textures->viewer, sf::Vector2f(870, startY + i * spacing - 5), sf::Vector2f(50, 50)));
                isInvite.push_back({false, false});
            }
            friendName.draw(*window);
            inviteFriends[friendsLobby[i]][0]->draw(*window);
            inviteFriends[friendsLobby[i]][1]->draw(*window);
        }

        int i = 0;
        for (const auto& [friendName, buttonsI] : inviteFriends) {
            if (!isInvite[i][0] && buttonsI[0]->isClicked(*window)) {
                std::cout<<"/invite/player/" << friendName << std::endl;
                client.sendInputFromSFML("/invite/player/" + friendName);
                buttonsI[0]->setTexture(textures->playerClicked);
                isInvite[i][0] = true;
                break;
            }
            if (!isInvite[i][1] && buttonsI[1]->isClicked(*window)) {
                std::cout<<"/invite/observer/" << friendName << std::endl;
                client.sendInputFromSFML("/invite/observer/" + friendName);
                buttonsI[1]->setTexture(textures->playerClicked);
                isInvite[i][1] = true;
                break;
            }
            i++;
        }


        // Réinitialiser la vue à la vue par défaut
        window->setView(window->getDefaultView());

        // Dessiner la barre de défilement
        float scrollbarHeight = 450 * (450 / totalHeight); // Hauteur proportionnelle
        float scrollbarY = 120 + (inviteScrollOffset / inviteMaxScroll) * (450 - scrollbarHeight);
        sf::RectangleShape scrollbar(sf::Vector2f(10, scrollbarHeight));
        scrollbar.setFillColor(sf::Color::White);
        scrollbar.setPosition(WINDOW_WIDTH / 2 + 275 - 10, scrollbarY);
        window->draw(scrollbar);

        
    }
    

    if (buttons.empty() && texts.empty()) {

        if(invite && showInviteCommand){
            buttons[ButtonKey::esc] = std::make_unique<Button>(textures->esc, sf::Vector2f(995, 90), sf::Vector2f(30, 30));
        }
        if(showCommand){
            texts[TextFieldKey::Speed] = std::make_unique<TextField>(font, 30, sf::Color::Black, sf::Color::White,
                sf::Vector2f(50, 150), sf::Vector2f(200, 50), "vitesse");
            buttons[ButtonKey::Valider] = std::make_unique<Button>("Valider", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                sf::Vector2f(270, 150), sf::Vector2f(100, 50));
        }
        
        if (classic || royale) {
            texts[TextFieldKey::NbreJoueurs] = std::make_unique<TextField>(font, 30, sf::Color::Black, sf::Color::White,
                sf::Vector2f(50, 220), sf::Vector2f(200, 50), "Nbre joueurs");
            buttons[ButtonKey::ValiderNb] = std::make_unique<Button>("Valider Nb", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                sf::Vector2f(270, 220), sf::Vector2f(100, 50));
        }

        if (royale) {
            texts[TextFieldKey::Energie] = std::make_unique<TextField>(font, 30, sf::Color::Black, sf::Color::White,
                sf::Vector2f(50, 290), sf::Vector2f(200, 50), "Energie");
            buttons[ButtonKey::ValiderEnergie] = std::make_unique<Button>("Valider Energie", font, 24, sf::Color::White, sf::Color(255, 165, 0),
                sf::Vector2f(270, 290), sf::Vector2f(150, 50));
        }

        buttons[ButtonKey::Chat] = std::make_unique<Button>("Chat", font, 24, sf::Color::White, sf::Color::Blue,
                                                                   sf::Vector2f(220, 500), sf::Vector2f(150, 50));

        float yPos = 250;
        if (royale) yPos = 360;
        else if (classic) yPos = 290;

        if(showInviteCommand)
            buttons[ButtonKey::InviteP_O] = std::make_unique<Button>(textures->P_O, sf::Vector2f(200, yPos), sf::Vector2f(120, 120));

        yPos += 120;
    }

    drawTextFields();
    drawButtons();
    quitter->draw(*window);


    if (buttons.count(ButtonKey::esc) && invite) {
        if (buttons[ButtonKey::esc]->isClicked(*window)) {
            std::cout << "Esc button clicked" << std::endl;
            invite = false;
            inviteScrollOffset = 0;
            inviteMaxScroll = 0;
            cleanup();
            inviteFriends.clear();
            isInvite.clear();
            return;
        }
    }


    if (!invite && buttons.count(ButtonKey::Valider) && buttons[ButtonKey::Valider]->isClicked(*window)) {
            std::string speed = texts[TextFieldKey::Speed]->getText();
            if(speed.empty()) {
                std::cerr << "Remplissez le champs de vitesse " << std::endl;
                return;
            }
            client.sendInputFromSFML("/speed/" + speed);
            texts[TextFieldKey::Speed]->setText("");
            return;
    }
   
    if (!invite && quitter->isClicked(*window)) {
        client.sendInputFromSFML("/back");
        return;
    }

    if(!invite && buttons.count(ButtonKey::Chat) && buttons[ButtonKey::Chat]->isClicked(*window)) {
        client.sendInputFromSFML(jsonKeys::CHAT_LOBBY);
        return;
    }

    if (!invite && (classic || royale)  && buttons.count(ButtonKey::ValiderNb) && buttons[ButtonKey::ValiderNb]->isClicked(*window)) {
            std::string nbPlayers = texts[TextFieldKey::NbreJoueurs]->getText();
            if(nbPlayers.empty()) {
                std::cerr << "Remplissez le champs du nombre de joueurs " << std::endl;
                return;
            }
            if (std::stoi(nbPlayers) > 0) {
                std::cout << "Nombre de joueurs: " << "/max/" + nbPlayers << std::endl;
                client.sendInputFromSFML("/max/" + nbPlayers);
                
            }
            texts[TextFieldKey::NbreJoueurs]->setText("");

            return;
    }

    if (!invite && royale  && buttons.count(ButtonKey::ValiderEnergie) && buttons[ButtonKey::ValiderEnergie]->isClicked(*window)) {
            std::string energy = texts[TextFieldKey::Energie]->getText();
            if(energy.empty()) {
                std::cerr << "Remplissez le champs d'énergie " << std::endl;
                return;
            }
            if (std::stoi(energy) > 0) {
                std::cout << "Énergie: " << "/energy/" + energy << std::endl;
                client.sendInputFromSFML("/energy/" + energy);
                
            }
            texts[TextFieldKey::Energie]->setText("");
            return;  
    }

    if (!invite && buttons.count(ButtonKey::InviteP_O) && buttons[ButtonKey::InviteP_O]->isClicked(*window)) {
        invite = true;
        cl = true;
        
        return;
        
    }
    int avatarIndex = client.getAvatarIndex();
    avatarManager->drawAvatar(avatarIndex, WINDOW_WIDTH - 70, 20, 45.0f);
    
}

void SFMLGame::displayJoinGame() {
    window->clear(sf::Color(30, 30, 60)); // Fond bleu nuit
    int avatarIndex = client.getAvatarIndex();
    avatarManager->drawAvatar(avatarIndex, WINDOW_WIDTH - 70, 20, 45.0f);

    if (buttons.empty()) {
        buttons[ButtonKey::Quit] = std::make_unique<Button>(textures->logoExit, sf::Vector2f(10, 20), sf::Vector2f(40, 40));
    }
    drawButtons();

    json j;

    if (buttons.count(ButtonKey::Quit) && buttons[ButtonKey::Quit]->isClicked(*window)) {
        j[jsonKeys::ACTION] = "createjoin";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    // Créer et configurer le texte du titre
    Text title("DEMANDES DE JEU", font, 40, sf::Color::White, sf::Vector2f(WINDOW_WIDTH / 2 - 250, 30));
    title.draw(*window);

    if (client.isGameStateUpdated()) {
        GameState gameData = client.getGameState();
        json requests = gameData.menu[jsonKeys::OPTIONS];
        std::string titre = gameData.menu[jsonKeys::TITLE];
        Text t(titre, font, 40, sf::Color::White, sf::Vector2f(20, 100));
        t.draw(*window);

        std::string line;
        if (requests.empty()) {
            line = "Aucune demande de jeu";
            Text requ(line, font, 40, sf::Color::Red, sf::Vector2f(20, 170));
            requ.draw(*window);
        } 
        else{
            // Synchroniser acceptInvite avec requests
            std::set<std::string> currentInvitations;
            int i = 0;

            for (auto& [key, value] : requests.items()) {
                line = key + value.get<std::string>();
                std::string message = line;

                // Extraire les informations de l'invitation
                std::size_t startPos = message.find("GameRoom '");
                std::string gameRoomNumber, inviter, status;
                if (startPos != std::string::npos) {
                    startPos += std::string("GameRoom '").length();
                    std::size_t endPos = message.find("'", startPos);
                    if (endPos != std::string::npos) {
                        gameRoomNumber = message.substr(startPos, endPos - startPos);
                    }

                    // Extraire le nom de l'invitant
                    std::size_t inviterStart = message.find("par '");
                    if (inviterStart != std::string::npos) {
                        inviterStart += std::string("par '").length();
                        std::size_t inviterEnd = message.find("'", inviterStart);
                        if (inviterEnd != std::string::npos) {
                            inviter = message.substr(inviterStart, inviterEnd - inviterStart);
                        }
                    }

                    // Extraire le statut (joueur/observateur)
                    std::size_t statusStart = message.find("en tant que '");
                    if (statusStart != std::string::npos) {
                        statusStart += std::string("en tant que '").length();
                        std::size_t statusEnd = message.find("'", statusStart);
                        if (statusEnd != std::string::npos) {
                            status = message.substr(statusStart, statusEnd - statusStart);
                        }
                    }
                }

                // Générer une clé unique pour cette invitation
                std::string invitationKey = gameRoomNumber + "|" + inviter + "|" + status;
                currentInvitations.insert(invitationKey);

                // Afficher le message
                float messageX = 20; // Position horizontale du message
                float messageY = 150 + i * 50;

                Text requ(message, font, 20, sf::Color::White, sf::Vector2f(messageX, messageY));
                requ.draw(*window);

                float buttonX = messageX + 800; // Décalage horizontal pour placer le bouton à droite du message
                float buttonY = messageY;

                // Ajouter un bouton pour cette invitation si elle n'existe pas déjà
                if (!acceptInvite.count(invitationKey)) {
                    acceptInvite[invitationKey] = std::make_unique<Button>(textures->accept, sf::Vector2f(buttonX, buttonY), sf::Vector2f(25, 25));
                }

                i++;
            }

            // Supprimer les invitations obsolètes
            for (auto it = acceptInvite.begin(); it != acceptInvite.end();) {
                if (currentInvitations.find(it->first) == currentInvitations.end()) {
                    it = acceptInvite.erase(it);
                } else {
                    ++it;
                }
            }

            // Dessiner les boutons
            for (const auto& [invitationKey, buttonsA] : acceptInvite) {
                buttonsA->draw(*window);
            }

            // Gérer les clics sur les boutons
            for (const auto& [invitationKey, buttonsA] : acceptInvite) {
                if (buttonsA->isClicked(*window)) {
                        

                    // Extraire les informations de la clé
                    std::size_t firstDelim = invitationKey.find("|");
                    std::size_t secondDelim = invitationKey.find("|", firstDelim + 1);
                    std::string gameRoomNumber = invitationKey.substr(0, firstDelim);
                    std::string inviter = invitationKey.substr(firstDelim + 1, secondDelim - firstDelim - 1);
                    std::string status = invitationKey.substr(secondDelim + 1);

                    // Envoyer la réponse au serveur
                    client.sendInputFromSFML("accept."+status+"." + gameRoomNumber);
                    acceptInvite.clear();
                    break;
                }
            }
        }
    }
}


void SFMLGame::drawMessageMalusBonus(const json& msg){

    if (msg[jsonKeys::CLEAR]){
        std::cout << "clear" << std::endl;
        cleanup();
        return;
    }

    if(msg[jsonKeys::CHOICE_CIBLE]) {
        std::string message = "Joueur Cible";
        if (buttons.empty() && texts.empty()) {
        
            texts[TextFieldKey::Victime] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                sf::Vector2f(190, 670), sf::Vector2f(90, 30), "Victime");
            buttons[ButtonKey::Valider] = std::make_unique<Button>("Valider", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                sf::Vector2f(300, 670), sf::Vector2f(90, 30));
        }

        drawTextFields();
        drawButtons();

        if (buttons.count(ButtonKey::Valider) && buttons[ButtonKey::Valider]->isClicked(*window)) {
            std::string id = texts[TextFieldKey::Victime]->getText();
            if(id.empty()) {
                std::cerr << "Remplissez le champs de victime " << std::endl;
                return;
            }
            client.sendInputFromSFML(id);
            texts[TextFieldKey::Victime]->setText("");
            return;
        }
        
        sf::Text messageText(message, font, 24);
        messageText.setFillColor(sf::Color::White);
        messageText.setPosition(1,650);
        window->draw(messageText);

    }
    
    else if(msg[jsonKeys::PROPOSITION_CIBLE]){
        int id = msg[jsonKeys::CIBLE_ID];
        std::string message = std::string("Joueur Cible") + " "+  std::to_string(id);

        if (buttons.empty()) {
            buttons[ButtonKey::Yes] = std::make_unique<Button>("Oui", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                sf::Vector2f(290, 670), sf::Vector2f(90, 30));

            buttons[ButtonKey::No] = std::make_unique<Button>("Non", font, 24, sf::Color::White, sf::Color::Red,
                sf::Vector2f(410, 670), sf::Vector2f(90, 30));
            
            
        }

        drawButtons();

        if (buttons.count(ButtonKey::Yes) && buttons[ButtonKey::Yes]->isClicked(*window)) {
            client.sendInputFromSFML("y");
            return;
        }

        if (buttons.count(ButtonKey::No) && buttons[ButtonKey::No]->isClicked(*window)) {
            client.sendInputFromSFML("n");
            return;
        }

        sf::Text messageText(message, font, 30);
        messageText.setFillColor(sf::Color::White);
        messageText.setPosition(1,670);
        window->draw(messageText);
    }

    /*--------------------------------------------------------------------------------------*/

    else if(msg[jsonKeys::CHOICE_MALUS_BONUS]){
        if (buttons.empty()) {
            buttons[ButtonKey::Malus] = std::make_unique<Button>("Malus", font, 24, sf::Color::White, sf::Color::Red,
                sf::Vector2f(50, 670), sf::Vector2f(100, 30));

            buttons[ButtonKey::Bonus] = std::make_unique<Button>("Bonus", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                sf::Vector2f(200, 670), sf::Vector2f(100, 30));
        }

        drawButtons();

        if (buttons.count(ButtonKey::Malus) && buttons[ButtonKey::Malus]->isClicked(*window)) {
            client.sendInputFromSFML("1");
            return;
        }

        if (buttons.count(ButtonKey::Bonus) && buttons[ButtonKey::Bonus]->isClicked(*window)) {
            client.sendInputFromSFML("2");
            return;
        }

    }

    else if(msg[jsonKeys::CHOICE_MALUS]){
        if (buttons.empty()) {
            buttons[ButtonKey::Inversion] = std::make_unique<Button>("Inversion", font, 24, sf::Color::White, sf::Color::Red,
                sf::Vector2f(50, 670), sf::Vector2f(100, 30));

            buttons[ButtonKey::Blocage] = std::make_unique<Button>("Blocage", font, 24, sf::Color::White, sf::Color::Red,
                sf::Vector2f(200, 670), sf::Vector2f(100, 30));

            buttons[ButtonKey::ChuteRapide] = std::make_unique<Button>("Chute rapide", font, 24, sf::Color::White, sf::Color::Red,
                sf::Vector2f(350, 670), sf::Vector2f(100, 30));

            buttons[ButtonKey::Suppression] = std::make_unique<Button>("Suppression", font, 24, sf::Color::White, sf::Color::Red,
                sf::Vector2f(500, 670), sf::Vector2f(100, 30));

            buttons[ButtonKey::EcranNoir] = std::make_unique<Button>("Ecran noir", font, 24, sf::Color::White, sf::Color::Red,
                sf::Vector2f(650, 670), sf::Vector2f(100, 30));

        }

        drawButtons();

        if (buttons.count(ButtonKey::Inversion) && buttons[ButtonKey::Inversion]->isClicked(*window)) {
            client.sendInputFromSFML("1");
            return;
        }

        if (buttons.count(ButtonKey::Blocage) && buttons[ButtonKey::Blocage]->isClicked(*window)) {
            client.sendInputFromSFML("2");
            return;
        }

        if (buttons.count(ButtonKey::ChuteRapide) && buttons[ButtonKey::ChuteRapide]->isClicked(*window)) {
            client.sendInputFromSFML("3");
            return;
        }

        if (buttons.count(ButtonKey::Suppression) && buttons[ButtonKey::Suppression]->isClicked(*window)) {
            client.sendInputFromSFML("4");
            return;
        }

        if (buttons.count(ButtonKey::EcranNoir) && buttons[ButtonKey::EcranNoir]->isClicked(*window)) {
            client.sendInputFromSFML("5");
            return;
        }
    }

    else if(msg[jsonKeys::CHOICE_BONUS]){
        if (buttons.empty()) {

            buttons[ButtonKey::Ralentir] = std::make_unique<Button>("Ralentir", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                sf::Vector2f(50, 670), sf::Vector2f(100, 30));

            buttons[ButtonKey::MiniBlocs] = std::make_unique<Button>("Mini-blocs", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                sf::Vector2f(200, 670), sf::Vector2f(100, 30));  
        }

        drawButtons();

        if (buttons.count(ButtonKey::Ralentir) && buttons[ButtonKey::Ralentir]->isClicked(*window)) {
            client.sendInputFromSFML("1");
            return;
        }

        if (buttons.count(ButtonKey::MiniBlocs) && buttons[ButtonKey::MiniBlocs]->isClicked(*window)) {
            client.sendInputFromSFML("2");
            return;
        }
    }
}

void SFMLGame::addMessage(const json& message) {
    messages.push_back(message);
}

void SFMLGame::resetAcceptInvite() {
    acceptInvite.clear();
}

MenuState SFMLGame::getPreviousState() const {
    return previousState;
}

std::vector<json> SFMLGame::getMessages() const {
    return messages;
}

std::string SFMLGame::getClickedContact() const {
    return clickedContact;
}
void SFMLGame::clearClickedContact() {
    clickedContact.clear();
}

std::unordered_map<std::string, std::unique_ptr<Button>>& SFMLGame::getTEAMSbuttons() {
    return TEAMSbuttons;
}

void SFMLGame::setSelectedTeam(const std::string& team) {
    selectedTeam = team;
}

std::string SFMLGame::getSelectedTeam() const {
    return selectedTeam;
}

void SFMLGame::clearSelectedTeam() {
    selectedTeam.clear();
}
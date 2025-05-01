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
const unsigned int WINDOW_HEIGHT = 750;
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
    for (int i = 1; i <= 10; ++i) { // Supposons 10 avatars disponibles
        avatarPaths.push_back("../../res/avatar/avatar" + std::to_string(i) + ".png");
    }

    // Load textures
    textures->loadTextures();
}
void SFMLGame::update() {
    // Autres mises à jour de logique du jeu...
    
    std::string tempMessage = client.getTemporaryMessage();
    if (!tempMessage.empty()) {
        afficherErreur(tempMessage);  // Appelle ta méthode d'affichage
        client.setTemporaryMessage("");  // Efface le message temporaire après l'avoir affiché
    }

    // Autres affichages ou mises à jour de l'interface
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

    sf::RectangleShape panneau;
    panneau.setSize(sf::Vector2f(500, 100));
    panneau.setFillColor(sf::Color(200, 0, 0, 200));
    panneau.setOutlineColor(sf::Color::White);
    panneau.setOutlineThickness(3);
    panneau.setPosition(WINDOW_WIDTH/2 - 500/2, 250);

    sf::Text texteErreur;
    texteErreur.setFont(font);
    texteErreur.setString(erreurMessageActuel);
    texteErreur.setCharacterSize(22);
    texteErreur.setFillColor(sf::Color::White);

    sf::FloatRect bounds = texteErreur.getLocalBounds();
    float posX = panneau.getPosition().x + (panneau.getSize().x - bounds.width) / 2 - bounds.left;
    float posY = panneau.getPosition().y + (panneau.getSize().y - bounds.height) / 2 - bounds.top;
    texteErreur.setPosition(posX, posY);

    window->draw(panneau);
    window->draw(texteErreur);
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
        for(const auto& buttonA : buttonsA) {
            buttonA->update();
            buttonA->setBackgroundColor(*window);
        }
        
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
void SFMLGame::addFriendMenu() {
    // Affichage de l'arrière-plan
    displayBackground(textures->chat);

    // Titre principal
    Text title("Ajouter un ami", font, 30, sf::Color::White, sf::Vector2f(250, 30));
    title.draw(*window);

    // Slogan ou aide en dessous du titre
    Text subtitle("Entre le pseudo de ton futur ami 🎉", font, 18, sf::Color(200, 200, 220), sf::Vector2f(250, 80));
    subtitle.draw(*window);

    // Création du champ de texte si pas encore fait
    if (texts.empty()) {
        TextField pseudoField(font, 20, sf::Color::Black, sf::Color::White,
                              sf::Vector2f(250, 130), sf::Vector2f(300, 40), "Pseudo");
        texts[TextFieldKey::AddFriendField] = std::make_unique<TextField>(pseudoField);
    }

    // Création des boutons si pas encore fait
    if (buttons.empty()) {
        Button sendButton("Envoyer", font, 20, sf::Color::White, sf::Color(70, 170, 250),
                          sf::Vector2f(250, 190), sf::Vector2f(140, 45));

        Button backButton("Retour", font, 20, sf::Color::White, sf::Color(180, 70, 70),
                          sf::Vector2f(410, 190), sf::Vector2f(140, 45));

        buttons[ButtonKey::Send] = std::make_unique<Button>(sendButton);
        buttons[ButtonKey::Retour] = std::make_unique<Button>(backButton);
    }

    // Dessin des éléments
    drawTextFields();
    drawButtons();

    // Traitement du bouton envoyer
    if (buttons[ButtonKey::Send]->isClicked(*window) && !texts[TextFieldKey::AddFriendField]->getText().empty()) {
        std::string friendName = texts[TextFieldKey::AddFriendField]->getText();
        json j;
        j[jsonKeys::ACTION] = jsonKeys::ADD_FRIEND;
        j["friend"] = friendName;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        //afficherErreur("Demande d'ami envoyée à " + friendName + ". .");
        texts[TextFieldKey::AddFriendField]->clear();
        return;
    }

    // Traitement du bouton retour  
    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = jsonKeys::FRIENDS;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
}
void SFMLGame::friendRequestListMenu() {
    // Fond
    displayBackground(textures->chat);

    // Barre latérale "Liste d'amis"
    Rectangle sidebar(sf::Vector2f(0, 0), sf::Vector2f(200, WINDOW_HEIGHT), sf::Color(30, 30, 50), sf::Color(80, 80, 100));
    sidebar.draw(*window);

    // Titre
    Text header("Liste d'amis", font, 24, sf::Color::White, sf::Vector2f(20, 10));
    header.draw(*window);

    auto serverData = client.getServerData();
    // Requête au serveur pour les amis si data reçu
    if (!serverData.empty() && serverData.contains("data") && serverData["message"] == jsonKeys::FRIEND_REQUEST_LIST) {
        amis = client.getServerData()["data"];
    }
    // Affichage des amis
    static std::vector<sf::Texture> avatarTextures(20);
    const float contactHeight = 50.0f;

    for (size_t i = 0; i < std::min(amis.size(), avatarTextures.size()); ++i) {
        float y = 100 + i * contactHeight;

        if (!friendButtons.count(amis[i])) {
            friendButtons[amis[i]] = std::make_unique<Button>(
                amis[i], font, 20, sf::Color::White, sf::Color::Transparent,
                sf::Vector2f(0, y), sf::Vector2f(200, contactHeight), sf::Color::Transparent);
        }

        if (avatarTextures[i].loadFromFile("../../res/avatar/avatar" + std::to_string(i + 1) + ".png")) {
            Circle(sf::Vector2f(7, y + 8), 30.0f, sf::Color::White, sf::Color::Transparent)
                .drawPhoto(avatarTextures[i], *window);
        }

        friendButtons[amis[i]]->draw(*window);
    }

    // Clic sur un ami
    for (const auto& [nom, bouton] : friendButtons) {
        if (bouton->isClicked(*window)) {
            selectedFriend = nom;
            break;
        }
    }

    // Si un ami est sélectionné
    if (!selectedFriend.empty()) {
        Rectangle(sf::Vector2f(220, 200), sf::Vector2f(300, 150), sf::Color(60, 60, 90), sf::Color::White);
        Text("Voulez-vous accepter l’ami :", font, 20, sf::Color::White, sf::Vector2f(230, 210)).draw(*window);
        Text(selectedFriend, font, 24, sf::Color::Cyan, sf::Vector2f(230, 240)).draw(*window);

        // Boutons accepter / refuser
        if (!buttons.count(ButtonKey::AcceptFriendRequest)) {
            buttons[ButtonKey::AcceptFriendRequest] = std::make_unique<Button>(
                "Accepter", font, 18, sf::Color::White, sf::Color(70, 200, 70),
                sf::Vector2f(230, 280), sf::Vector2f(100, 40));
        }
        if (!buttons.count(ButtonKey::RefuseFriendRequest)) {
            buttons[ButtonKey::RefuseFriendRequest] = std::make_unique<Button>(
                "Refuser", font, 18, sf::Color::White, sf::Color(200, 70, 70),
                sf::Vector2f(350, 280), sf::Vector2f(100, 40));
        }

        buttons[ButtonKey::AcceptFriendRequest]->draw(*window);
        buttons[ButtonKey::RefuseFriendRequest]->draw(*window);

        // Action sur clic
        if (buttons[ButtonKey::AcceptFriendRequest]->isClicked(*window)) {
            json j = {{jsonKeys::ACTION, jsonKeys::ACCEPT_FRIEND_REQUEST}, {"friend", selectedFriend}};
            network->sendData(j.dump() + "\n", client.getClientSocket());
            selectedFriend.clear();
        }
        if (buttons[ButtonKey::RefuseFriendRequest]->isClicked(*window)) {
            json j = {{jsonKeys::ACTION, jsonKeys::REJECT_FRIEND_REQUEST}, {"friend", selectedFriend}};
            network->sendData(j.dump() + "\n", client.getClientSocket());
            selectedFriend.clear();
        }
    }

    // Bouton retour
    if (!buttons.count(ButtonKey::Retour)) {
        buttons[ButtonKey::Retour] = std::make_unique<Button>(
            textures->logoMain, sf::Vector2f(7, 50), sf::Vector2f(25, 35));
    }
    buttons[ButtonKey::Retour]->draw(*window);

    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j["action"] = "main";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        selectedFriend.clear();
        return;
    }
}



void SFMLGame::friendsMenu() {
    // Affichage de l'arrière-plan
    //std::cout << "Affichage du menu des amis" << std::endl;
    displayBackground(textures->chat);
    

    // Titre du menu
    Text header("Gestion des amis", font, 30, sf::Color::White, sf::Vector2f(250, 20));
    header.draw(*window);
        // Requête au serveur pour les amis si data reçu

    if (buttons.empty()) {
        // Bouton "Ajouter un ami"
        Button addFriendButton("Ajouter un ami", font, 20, sf::Color::White, sf::Color(100, 200, 250),
                               sf::Vector2f(250, 100), sf::Vector2f(200, 50));
        
        // Bouton "Liste des amis"
        Button listFriendsButton("Liste des amis", font, 20, sf::Color::White, sf::Color(70, 180, 100),
                                 sf::Vector2f(250, 170), sf::Vector2f(200, 50));
        
        // Bouton "Demandes d'amis"
        Button requestsButton("Demandes d'amis", font, 20, sf::Color::White, sf::Color(200, 180, 70),
                              sf::Vector2f(250, 240), sf::Vector2f(200, 50));

        // Bouton retour
        Button backButton("Retour", font, 20, sf::Color::White, sf::Color(180, 70, 70),
                          sf::Vector2f(250, 310), sf::Vector2f(200, 50));

        buttons[ButtonKey::AddFriend] = std::make_unique<Button>(addFriendButton);
        buttons[ButtonKey::FriendList] = std::make_unique<Button>(listFriendsButton);
        buttons[ButtonKey::FriendRequestList] = std::make_unique<Button>(requestsButton);
        buttons[ButtonKey::Retour] = std::make_unique<Button>(backButton);
    }

    drawButtons();

    // Actions associées aux boutons
    if (buttons[ButtonKey::AddFriend]->isClicked(*window)) {

        std::cout << "Bouton Ajouter un ami cliqué !" << std::endl;
        json j;
        j[jsonKeys::ACTION] = jsonKeys::ADD_FRIEND_MENU;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
        return;
    }

    if (buttons[ButtonKey::FriendList]->isClicked(*window)) {
        // Afficher la liste des amis
        std::cout << "Affichage de la liste des amis..." << std::endl;
        json j;
        j[jsonKeys::ACTION] = jsonKeys::FRIEND_LIST;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
        return;
    }

    if (buttons[ButtonKey::FriendRequestList]->isClicked(*window)) {
        std::cout << "Affichage des demandes d’amis..." << std::endl;
        json j;
        j[jsonKeys::ACTION] = jsonKeys::FRIEND_REQUEST_LIST;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
        return;
    }

    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = jsonKeys::MAIN;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
        return;
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
            rankingMenu();
            break;
        case MenuState::Settings:
            displayWaitingRoom();
            //to do  
            break;
        case MenuState::Notifications:
            //notificationsMenu();
            break;
        case MenuState::Friends:
            friendsMenu();
            break;
        case MenuState::FriendList:
            friendListMenu();
            break;
        case MenuState::AddFriend:
            addFriendMenu();
            break;
        case MenuState::FriendRequestList:
            friendRequestListMenu();
            break;
        case MenuState::chat:
            chatMenu();
            break;
        case MenuState::ManageTeam:
            //teamsMenu();
            break;
        case MenuState::CreateTeam:
            //createRoomMenu();
            break;
        case MenuState::JoinOrCreateGame:
            CreateOrJoinGame();
            break;
        case MenuState::JoinGame:
            displayJoinGame();
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
        case MenuState::Observer:
            displayGame();
            break;
        case MenuState::GameOver:
            drawEndGame();
            break;
        default:
            //std::cerr << "Unhandled MenuState: " << static_cast<int>(currentState) << std::endl;
            break;
    }
    client.clearServerData();
    window->display();
    sleep(0.1);
}
void SFMLGame::friendListMenu() {
    // Fond
    //std::cout << "Affichage de la liste d'amis" << std::endl;
    displayBackground(textures->chat);

    // Barre latérale "Amis"
    Rectangle sidebar(sf::Vector2f(0, 0), sf::Vector2f(200, WINDOW_HEIGHT), sf::Color(30, 30, 50), sf::Color(80, 80, 100));
    sidebar.draw(*window);

    // Titre
    Text header("Mes amis", font, 24, sf::Color::White, sf::Vector2f(20, 10));
    header.draw(*window);
    auto serverData = client.getServerData();
    // Requête au serveur pour les amis si data reçu
    if (!serverData.empty() && serverData.contains("data") && serverData["message"] == jsonKeys::FRIEND_LIST) {
        amis = serverData["data"];
        if (amis.empty()) {
            std::cout << "Aucun ami trouvé." << std::endl;
        } else {
            std::cout << "Liste d'amis reçue avec succès." << std::endl;
        }
        for (const auto& friendName : amis) {
            std::cout << "------->   Ami: " << friendName << std::endl;
        }
    }
    // Affichage des amis
    static std::vector<sf::Texture> avatarTextures(20);
    const float contactHeight = 50.0f;

    for (size_t i = 0; i < std::min(amis.size(), avatarTextures.size()); ++i) {
        float y = 100 + i * contactHeight;

        if (!friendButtons.count(amis[i])) {
            friendButtons[amis[i]] = std::make_unique<Button>(
                amis[i], font, 20, sf::Color::White, sf::Color::Transparent,
                sf::Vector2f(0, y), sf::Vector2f(200, contactHeight), sf::Color::Transparent);
        }

        if (avatarTextures[i].loadFromFile("../../res/avatar/avatar" + std::to_string(i + 1) + ".png")) {
            Circle(sf::Vector2f(7, y + 8), 30.0f, sf::Color::White, sf::Color::Transparent)
                .drawPhoto(avatarTextures[i], *window);
        }

        friendButtons[amis[i]]->draw(*window);
    }

    // Clic sur un ami
    for (const auto& [nom, bouton] : friendButtons) {
        if (bouton->isClicked(*window)) {
            selectedFriend = nom;
            break;
        }
    }

    // Si un ami est sélectionné
    if (!selectedFriend.empty()) {
        Rectangle(sf::Vector2f(220, 200), sf::Vector2f(300, 150), sf::Color(60, 60, 90), sf::Color::White);
        Text("Voulez-vous supprimer :", font, 20, sf::Color::White, sf::Vector2f(230, 210)).draw(*window);
        Text(selectedFriend, font, 24, sf::Color::Cyan, sf::Vector2f(230, 240)).draw(*window);

        // Boutons supprimer / annuler
        if (!buttons.count(ButtonKey::RemoveFriend)) {
            buttons[ButtonKey::RemoveFriend] = std::make_unique<Button>(
                "Supprimer", font, 18, sf::Color::White, sf::Color(200, 70, 70),
                sf::Vector2f(230, 280), sf::Vector2f(100, 40));
        }
        if (!buttons.count(ButtonKey::Cancel)) {
            buttons[ButtonKey::Cancel] = std::make_unique<Button>(
                "Annuler", font, 18, sf::Color::White, sf::Color(100, 100, 100),
                sf::Vector2f(350, 280), sf::Vector2f(100, 40));
        }

        buttons[ButtonKey::RemoveFriend]->draw(*window);
        buttons[ButtonKey::Cancel]->draw(*window);

        // Action sur clic
        if (buttons[ButtonKey::RemoveFriend]->isClicked(*window)) {
            json j = {
                {jsonKeys::ACTION, jsonKeys::REMOVE_FRIEND},
                {"friend", selectedFriend}
            };
            network->sendData(j.dump() + "\n", client.getClientSocket());
            selectedFriend.clear();
        }

        if (buttons[ButtonKey::Cancel]->isClicked(*window)) {
            selectedFriend.clear();
        }
    }

    // Bouton retour
    if (!buttons.count(ButtonKey::Retour)) {
        buttons[ButtonKey::Retour] = std::make_unique<Button>(
            textures->logoMain, sf::Vector2f(7, 50), sf::Vector2f(25, 35));
    }
    buttons[ButtonKey::Retour]->draw(*window);

    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j["action"] = "main";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        selectedFriend.clear();
        return;
    }
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


void SFMLGame::welcomeMenu() {
    // Afficher l'arrière-plan du menu de bienvenue
    displayBackground(textures->connexion);

    // Ajouter les boutons s'ils n'existent pas
    if (buttons.empty()) {
        buttons[ButtonKey::Login] = std::make_unique<Button>("Login", font, 24, sf::Color::White, sf::Color(100, 149, 237),
                                                             sf::Vector2f(WINDOW_WIDTH/2 - 220, 500), sf::Vector2f(200, 35));
        buttons[ButtonKey::Registre] = std::make_unique<Button>("Registre", font, 24, sf::Color::White, sf::Color(255, 165, 0),
                                                                sf::Vector2f(WINDOW_WIDTH/2 + 20, 500), sf::Vector2f(200, 35));
        buttons[ButtonKey::Quit] = std::make_unique<Button>("Exit", font, 24, sf::Color::White, sf::Color(255, 99, 71),
                                                            sf::Vector2f(WINDOW_WIDTH/2 - 200/2, 560), sf::Vector2f(200, 35));
    }

    // Dessiner les boutons
    drawButtons();
    json j;
    // Gérer les clics sur les boutons
    if (buttons.count(ButtonKey::Quit) && buttons[ButtonKey::Quit]->isClicked(*window)) {
        cleanup();
        window->close();
        return;
    }

    if (buttons.count(ButtonKey::Registre) && buttons[ButtonKey::Registre]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::REGISTER_MENU;
        //currentState = MenuState::Register; // Passer à l'état d'inscription
        //std::cout << "Register button clicked" << std::endl;
        //std::cout << "currentState: " << static_cast<int>(currentState) << std::endl;
        network->sendData(j.dump() + "\n", client.getClientSocket());

        cleanup();
        return;
    }

    if (buttons.count(ButtonKey::Login) && buttons[ButtonKey::Login]->isClicked(*window)) {
        //currentState = MenuState::Login; // Passer à l'état de connexion
        j[jsonKeys::ACTION] = jsonKeys::LOGIN_MENU;
        network->sendData(j.dump() + "\n", client.getClientSocket());

        cleanup();
        return;
    }
}


void SFMLGame::registerMenu() {
    // Afficher l'arrière-plan du formulaire d'inscription
    displayBackground(textures->connexion);

    // Titre principal
    Text title("Creation de compte", font, 30, sf::Color::White, sf::Vector2f(WINDOW_WIDTH / 2 - 150, 30));
    title.draw(*window);

    // Slogan ou aide
    Text subtitle("Rejoignez la communaute Tetris Royal", font, 18, sf::Color(200, 200, 220), sf::Vector2f(WINDOW_WIDTH / 2 - 180, 80));
    subtitle.draw(*window);

    // Centrage des champs
    float fieldWidth = 250;
    float fieldHeight = 35;
    float centerX = WINDOW_WIDTH / 2 - fieldWidth / 2;
    float startY = 180;
    float fieldSpacing = 50;

    // Ajouter les champs de texte s'ils n'existent pas
    if (texts.empty()) {
        texts[TextFieldKey::Username] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                     sf::Vector2f(centerX, startY), sf::Vector2f(fieldWidth, fieldHeight), "Username");
        texts[TextFieldKey::Password] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                     sf::Vector2f(centerX, startY + fieldSpacing), sf::Vector2f(fieldWidth, fieldHeight), "Password", true);
        texts[TextFieldKey::ConfirmPassword] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                            sf::Vector2f(centerX, startY + 2 * fieldSpacing), sf::Vector2f(fieldWidth, fieldHeight), "Confirm Password", true);
    }

    // Section Sélection d'avatar
    float avatarTitleY = startY + 3 * fieldSpacing + 20;
    Text avatarTitle("Choisissez votre avatar:", font, 20, sf::Color::White, sf::Vector2f(WINDOW_WIDTH / 2 - 130, avatarTitleY));
    avatarTitle.draw(*window);

    // afficher la galerie d'avatar
    float avatarSize = 50;
    float avatarSpacing = 15;
    int avatarsPerRow = 5;
    float avatarRowWidth = avatarsPerRow * avatarSize + (avatarsPerRow - 1) * avatarSpacing;
    float avatarStartX = WINDOW_WIDTH / 2 - avatarRowWidth / 2;
    float avatarY = avatarTitleY + 40;

    for (size_t i = 0; i < avatarPaths.size(); ++i) {
        float avatarX = avatarStartX + (i % avatarsPerRow) * (avatarSize + avatarSpacing);
        float currentY = avatarY + (i / avatarsPerRow) * (avatarSize + avatarSpacing);

        sf::Texture avatarTex;
        if (avatarTex.loadFromFile(avatarPaths[i])) {
            sf::CircleShape avatarCircle(avatarSize / 2);
            avatarCircle.setPosition(avatarX, currentY);
            avatarCircle.setOutlineThickness(2);
            avatarCircle.setOutlineColor((selectedAvatar == static_cast<int>(i)) ? sf::Color::Yellow : sf::Color::Transparent);
            avatarCircle.setFillColor(sf::Color(100, 100, 100));
            avatarCircle.setTexture(&avatarTex);

            window->draw(avatarCircle);

            sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
                std::sqrt(std::pow(mousePos.x - (avatarX + avatarSize / 2), 2) +
                          std::pow(mousePos.y - (currentY + avatarSize / 2), 2)) <= avatarSize / 2) {
                selectedAvatar = static_cast<int>(i);
                std::cout << "Avatar sélectionné : " << selectedAvatar << std::endl;
            }
        }
    }

    // Ajouter les boutons s'ils n'existent pas
    float buttonWidth = 200;
    float buttonHeight = 35;
    float buttonX = WINDOW_WIDTH / 2 - buttonWidth / 2;
    float avatarRows = (avatarPaths.size() + avatarsPerRow - 1) / avatarsPerRow;
    float buttonY = avatarY + avatarRows * (avatarSize + avatarSpacing) + 30;

    if (buttons.empty()) {
        buttons[ButtonKey::Registre] = std::make_unique<Button>("S'inscrire", font, 24, sf::Color::White, sf::Color(100, 149, 237),
                                                                 sf::Vector2f(buttonX, buttonY), sf::Vector2f(buttonWidth, buttonHeight));
        buttons[ButtonKey::Retour] = std::make_unique<Button>("Retour", font, 24, sf::Color::White, sf::Color(255, 99, 71),
                                                               sf::Vector2f(buttonX, buttonY + 60), sf::Vector2f(buttonWidth, buttonHeight));
    }

    // Dessiner les champs de texte et les boutons
    drawTextFields();
    drawButtons();

    // Affichage du message d'erreur s'il est actif
    drawErreurMessage();

    // Gérer les clics sur les boutons
    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        selectedAvatar = -1; // Réinitialiser la sélection
        json j;
        j[jsonKeys::ACTION] = jsonKeys::WELCOME;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
        return;
    }

    if (buttons[ButtonKey::Registre]->isClicked(*window)) {
        std::string username = texts[TextFieldKey::Username]->getText();
        std::string password = texts[TextFieldKey::Password]->getText();
        std::string confirmPassword = texts[TextFieldKey::ConfirmPassword]->getText();

        // Validation des champs
        if (username.empty() || password.empty() || confirmPassword.empty()) {
            afficherErreur("Tous les champs doivent être remplis.");
            return;
        }

        if (password != confirmPassword) {
            afficherErreur("Les mots de passe ne correspondent pas.");
            return;
        }

        if (selectedAvatar < 0 || selectedAvatar >= static_cast<int>(avatarPaths.size())) {
            afficherErreur("Veuillez sélectionner un avatar.");
            return;
        }
        client.setAvatarIndex(selectedAvatar);

        // Envoi des données au serveur
        json j = {
            {jsonKeys::ACTION, "register"},
            {jsonKeys::USERNAME, username},
            {jsonKeys::PASSWORD, password},
            {"avatar", selectedAvatar}
        };

        network->sendData(j.dump() + "\n", client.getClientSocket());

        // Réinitialisation des champs
        texts[TextFieldKey::Username]->clear();
        texts[TextFieldKey::Password]->clear();
        texts[TextFieldKey::ConfirmPassword]->clear();
        selectedAvatar = -1;
    }
}


void SFMLGame::connexionMenu() {
    // Afficher l'arrière-plan du menu de connexion
    displayBackground(textures->connexion);

    // Ajouter les champs de texte et les boutons s'ils n'existent pas
    if (texts.empty()) {
        texts[TextFieldKey::Username] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                    sf::Vector2f(WINDOW_WIDTH/2 - 200/2, 400), sf::Vector2f(200, 35), "Username.");
        texts[TextFieldKey::Password] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                    sf::Vector2f(WINDOW_WIDTH/2 - 200/2, 450), sf::Vector2f(200, 35), "Password", true);
    }

    if (buttons.empty()) {
        buttons[ButtonKey::Login] = std::make_unique<Button>("Se connecter", font, 24, sf::Color::White, sf::Color(100, 149, 237),
                                                             sf::Vector2f(WINDOW_WIDTH/2 - 200/2, 500), sf::Vector2f(200, 35));
        buttons[ButtonKey::Retour] = std::make_unique<Button>("Retour", font, 24, sf::Color::White, sf::Color(255, 99, 71),
                                                              sf::Vector2f(WINDOW_WIDTH/2 - 200/2, 560), sf::Vector2f(200, 35));
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
        cleanup();
        
        
    }
}

void SFMLGame::drawAvatar(int avatarIndex, float posX, float posY, float size) {
    if (avatarIndex >= 0 && avatarIndex < static_cast<int>(avatarPaths.size())) {
        sf::Texture avatarTexture;
        if (avatarTexture.loadFromFile(avatarPaths[avatarIndex])) {
            sf::Sprite avatarSprite;
            avatarSprite.setTexture(avatarTexture);

            // Redimensionner l'avatar pour qu'il rentre bien dans le bouton
            avatarSprite.setScale(
                size / avatarTexture.getSize().x,
                size / avatarTexture.getSize().y
            );

            // Positionner l'avatar
            avatarSprite.setPosition(posX, posY);

            window->draw(avatarSprite);
        } else {
            std::cout << "Erreur de chargement de l'avatar !" << std::endl;
        }
    }
}

void SFMLGame::mainMenu() {
    // Afficher l'arrière-plan du menu principal
    displayBackground(textures->logoConnexion);

    float buttonWidth = 120;
    float buttonHeight = 50;
    float spacing = 40;
    float startX = 450;
    float y = 660;

    // Couleurs harmonisées
    sf::Color background = sf::Color(30, 30, 30, 180); // noir semi-transparent
    sf::Color outline = sf::Color(135, 206, 250);      // bleu ciel
    sf::Color text = sf::Color::White;

    // Ajouter les boutons s'ils n'existent pas
    if (buttons.empty()) {
        // Boutons principaux
        buttons[ButtonKey::Ranking] = std::make_unique<Button>("Ranking", font, 26, text, background,
                                                                sf::Vector2f(startX, y),
                                                                sf::Vector2f(buttonWidth, buttonHeight), outline);

        buttons[ButtonKey::Play] = std::make_unique<Button>("Play", font, 26, text, background,
                                                            sf::Vector2f(startX + (buttonWidth + spacing), y + 30),
                                                            sf::Vector2f(buttonWidth, buttonHeight), outline);

        buttons[ButtonKey::Chat] = std::make_unique<Button>("Chat", font, 26, text, background,
                                                            sf::Vector2f(startX + 2 * (buttonWidth + spacing), y + 30),
                                                            sf::Vector2f(buttonWidth, buttonHeight), outline);

        buttons[ButtonKey::Friends] = std::make_unique<Button>("Friends", font, 26, text, background,
                                                               sf::Vector2f(startX + 3 * (buttonWidth + spacing), y),
                                                               sf::Vector2f(buttonWidth, buttonHeight), outline);

        buttons[ButtonKey::Quit] = std::make_unique<Button>(textures->logoExit,
                                                            sf::Vector2f(10, 20),
                                                            sf::Vector2f(40, 40));

        buttons[ButtonKey::Settings] = std::make_unique<Button>(textures->logoSettings,
                                                                sf::Vector2f(WINDOW_WIDTH - 130, 20),
                                                                sf::Vector2f(35, 35));

        buttons[ButtonKey::Notification] = std::make_unique<Button>(textures->logoNotification,
                                                                    sf::Vector2f(WINDOW_WIDTH - 190, 20),
                                                                    sf::Vector2f(45, 45));

        buttons[ButtonKey::Profile] = std::make_unique<Button>("", font, 24, sf::Color::Transparent, sf::Color::White,
                                                                sf::Vector2f(WINDOW_WIDTH - 70, 20),
                                                                sf::Vector2f(45, 45), sf::Color::Transparent);
    }

    // Dessiner les boutons
    drawButtons();
    int avatarIndex = client.getAvatarIndex();
    drawAvatar(avatarIndex, WINDOW_WIDTH - 70, 20, 45.0f);
    

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

    } else if (buttons.count(ButtonKey::Ranking) && buttons[ButtonKey::Ranking]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::RANKING;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;

    } else if (buttons.count(ButtonKey::Settings) && buttons[ButtonKey::Settings]->isClicked(*window)) {
        // Action pour le bouton "Settings"

    } else if (buttons.count(ButtonKey::Notification) && buttons[ButtonKey::Notification]->isClicked(*window)) {
        // Action pour le bouton "Notification"

    } else if (buttons.count(ButtonKey::Profile) && buttons[ButtonKey::Profile]->isClicked(*window)) {
        // Action pour le bouton "Profile"

    } else if (buttons.count(ButtonKey::Quit) && buttons[ButtonKey::Quit]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::WELCOME;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
}



void SFMLGame::rankingMenu(){
    //afficher la background
    displayBackground(textures->ranking);
    // Header for ranking list
    Text header("LeaderBoard", font, 24, sf::Color::White, sf::Vector2f(60, 20));
    header.draw(*window);
    if(buttons.empty()){
        buttons[ButtonKey::Retour]= std::make_unique<Button>(textures->logoExit, sf::Vector2f(10, 20), sf::Vector2f(40, 40));
        buttons[ButtonKey::Settings] = std::make_unique<Button>(textures->logoSettings,sf::Vector2f(WINDOW_WIDTH - 130, 20), sf::Vector2f(35, 35));
        buttons[ButtonKey::Notification] = std::make_unique<Button>(textures->logoNotification, sf::Vector2f(WINDOW_WIDTH - 190, 20), sf::Vector2f(45, 45));
        buttons[ButtonKey::Profile] = std::make_unique<Button>("", font, 24, sf::Color::Transparent, sf::Color::White,
                                    sf::Vector2f(WINDOW_WIDTH - 70, 20), sf::Vector2f(35, 35), sf::Color::Transparent);
    }
    if (texts.empty()) {
        texts[TextFieldKey::SearchField] = std::make_unique<TextField>(font, 20, sf::Color::Black, sf::Color(250, 250, 250),
                                        sf::Vector2f(50, 20), sf::Vector2f(300, 35), "Search");
    }
    drawButtons();
    // Affichage de la liste d'amis (exemple visuel)
    float startY = 80;
    float spacing = 70;
    auto serverData = client.getServerData();
    if(serverData.contains("dataPair"))
        ranking = client.getServerData()["dataPair"]; // Récupérer la liste d'amis
    for (size_t i = 0; i < ranking.size() && i < 10; ++i) {
        // Rectangle fictif pour la carte du joueur
        Rectangle friendCard(sf::Vector2f(90, startY + i * spacing), sf::Vector2f(600, 60), sf::Color::Transparent, sf::Color(200, 200, 200));
        friendCard.draw(*window);
        //Nom du joueur
        Text name(ranking[i].first, font, 20, sf::Color::Black, sf::Vector2f(150, startY + i * spacing + 15));
        name.draw(*window);
        //Score du joueur
        Text score(std::to_string(ranking[i].second), font, 20, sf::Color::Black, sf::Vector2f(570, startY + i * spacing + 15));
        score.draw(*window);
        // Avatar fictif
        //Circle avatar();
        //avatar.draw(*window);
    }
    if(buttons.count(ButtonKey::Retour) && buttons[ButtonKey::Retour]->isClicked(*window)) {
        json j;
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
    drawAvatar(avatarIndex, WINDOW_WIDTH - 70, 20, 45.0f);

}



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
        Button backButton(textures->logoMain,sf::Vector2f(7, 50), sf::Vector2f(25, 35));       // Bouton pour envoyer le message
        // Bouton pour envoyer le message
        Button sendButton(">", font, 20, sf::Color::White, sf::Color(70, 200, 70),
                          sf::Vector2f(WINDOW_WIDTH - 40, WINDOW_HEIGHT - 40), sf::Vector2f(35, 35));
        // Ajout des boutons au vecteur
        buttons[ButtonKey::Retour] = std::make_unique<Button>(backButton);
        buttons[ButtonKey::Send] = std::make_unique<Button>(sendButton);
    }

    // Dessiner les champs de texte et les boutons
    drawTextFields();
    drawButtons();
    
    // Gérer les clics sur les contacts
    handleContacts();
    //dessiner les contacts
    drawContacts();

    // Vérifier d'abord le backButton
    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = "main";
        network->sendData(j.dump() + "\n", client.getClientSocket());
        clickedContact.clear();
        return; // Sortir immédiatement après avoir traité le clic
    }

    // Ensuite vérifier le sendButton
    if (!clickedContact.empty() && buttons[ButtonKey::Send]->isClicked(*window) && !texts[TextFieldKey::MessageField]->getText().empty()) {
        json j = {
            {"message", texts[TextFieldKey::MessageField]->getText()},
            {"receiver", clickedContact},
            {"sender", "You"}

        };
        network->sendData(j.dump() + "\n", client.getClientSocket());
        messages.emplace_back(std::move(j));

        texts[TextFieldKey::MessageField]->clear(); // Effacer le champ de texte après l'envoi
        return;
    }
    // recevoir les messages du serveur et les stocker
    getMessagesFromServer();
    // Afficher les messages
    drawMessages();
}

void SFMLGame::handleContacts() {
    const auto& contacts = client.getContacts(); 
    if (contacts.empty()) return;
    static std::vector<sf::Texture> avatarTextures(20);
    const float contactHeight = 50.0f;
    
    // Gestion des contacts
    for (size_t i = 0; i < std::min(contacts.size(), avatarTextures.size()); ++i) {
        const auto& [contactName, avatarIndex] = contacts[i];
        float contactY = 100 + i * contactHeight;

        if (!chatContacts.count(contactName)) {
            chatContacts[contactName] = std::make_unique<Button>(
                contactName, font, 20, sf::Color::White, sf::Color::Transparent,
                sf::Vector2f(0, contactY), sf::Vector2f(200, contactHeight), sf::Color::Transparent);
        }

        if (avatarIndex >= 0 && avatarIndex < static_cast<int>(avatarPaths.size())) {
            if (avatarTextures[avatarIndex].loadFromFile(avatarPaths[avatarIndex])) {
                Circle(sf::Vector2f(7, contactY + 8), 30.0f, sf::Color::White, sf::Color::Transparent)
                    .drawPhoto(avatarTextures[avatarIndex], *window);
            }
        }
        else{
            sf::CircleShape circle(20.0f); // Rayon du cercle
            circle.setFillColor(sf::Color(100, 100, 200)); // Couleur de fond
            circle.setPosition(10.0f, contactY + 10.0f); // Position du cercle
            window->draw(circle);

            // Dessiner la première lettre du nom dans le cercle
            sf::Text initial;
            initial.setFont(font);
            initial.setString(contactName.substr(0, 1)); // Première lettre du nom
            initial.setCharacterSize(20);
            initial.setFillColor(sf::Color::White);

            // Centrer la lettre dans le cercle
            sf::FloatRect textBounds = initial.getLocalBounds();
            initial.setPosition(
                circle.getPosition().x + circle.getRadius() - textBounds.width / 2 - textBounds.left,
                circle.getPosition().y + circle.getRadius() - textBounds.height / 2 - textBounds.top
            );

            window->draw(initial);
        }
        //gestion des clicks
        const auto& Button = chatContacts[contactName];
        if(Button->isClicked(*window)){
            clickedContact = contactName;
            avatarClickedContact = avatarIndex;
            network->sendData(json{{"action", "openChat"}, {"contact", contactName}}.dump() + "\n", 
                        client.getClientSocket());
        }
        
    }

    // Affichage du chat sélectionné
    if (!clickedContact.empty()) {
        Rectangle(sf::Vector2f(202, 0), sf::Vector2f(WINDOW_WIDTH - 200, 50),
                 sf::Color(50, 50, 70), sf::Color(100, 100, 120)).draw(*window);
        Text(clickedContact, font, 24, sf::Color::White, sf::Vector2f(250, 10)).draw(*window);
        Circle(sf::Vector2f(215, 10), 30.0f, sf::Color::White, sf::Color::Transparent)
            .drawPhoto(avatarTextures[avatarClickedContact], *window);
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
    for (auto& msg : messages) {
        auto sender = msg["sender"]; auto receiver = msg["receiver"];
        if((sender == clickedContact) || (sender == "You" && receiver == clickedContact)) {
            displayMessage(sender, msg["message"]);
        MessagesY += 40; // Adjust the Y position for each message
        }
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
    bubble.draw(*window);buttons[ButtonKey::Quit] = std::make_unique<Button>(textures->logoExit,
        sf::Vector2f(10, 20),
        sf::Vector2f(40, 40));

    // Draw message text
    Text messageText(message, font, 16, sf::Color::White, sf::Vector2f(bubbleX + cornerRadius + 5.f, MessagesY));
    messageText.draw(*window);
}


/*
Game Menus
*/

void SFMLGame::CreateOrJoinGame() {
    displayBackground(textures->rejoindre);

    if (buttons.empty()) {
        std::cout << "Creating buttons" << std::endl;

        // Création des boutons
        buttons[ButtonKey::Create] = std::make_unique<Button>("Create", font, 50, sf::Color::White, sf::Color(34, 139, 34),
                                                              sf::Vector2f(400, 500), sf::Vector2f(250, 60));
        buttons[ButtonKey::Join] = std::make_unique<Button>("Join", font, 50, sf::Color::White, sf::Color(184, 134, 11),
                                                            sf::Vector2f(850, 500), sf::Vector2f(250, 60));

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
    
    else if (buttons.count(ButtonKey::Join) && buttons[ButtonKey::Join]->isClicked(*window)){
        j[jsonKeys::ACTION] = "Rejoindre";
        client.sendInputFromSFML("2");
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
    else if(buttons.count(ButtonKey::Quit) && buttons[ButtonKey::Quit]->isClicked(*window)){
        j[jsonKeys::ACTION] = "main";
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
    drawAvatar(avatarIndex, WINDOW_WIDTH - 70, 20, 45.0f);
}

void SFMLGame::ChoiceGameMode(){
    displayBackground(textures->mode);
    if (buttons.empty()) {
        std::cout << "Creating buttons" << std::endl;

        // Création des boutons
        buttons[ButtonKey::Endless] = std::make_unique<Button>("ENDLESS", font, 30, sf::Color::White, sf::Color(255, 255, 255, 0),
                                                               sf::Vector2f(335, 425), sf::Vector2f(160, 45), sf::Color(255, 255, 255, 0));

        buttons[ButtonKey::Duel] = std::make_unique<Button>("DUEL", font, 30, sf::Color::White, sf::Color(255, 255, 255, 0),
                                                            sf::Vector2f(375, 560), sf::Vector2f(100, 45), sf::Color(255, 255, 255, 0));

        buttons[ButtonKey::Classic] = std::make_unique<Button>("CLASSIC", font, 30, sf::Color::White, sf::Color(255, 255, 255, 0),
                                                               sf::Vector2f(980, 425), sf::Vector2f(150, 45), sf::Color(255, 255, 255, 0));

        buttons[ButtonKey::Royale] = std::make_unique<Button>("ROYALE", font, 30, sf::Color::White, sf::Color(255, 255, 255, 0),
                                                              sf::Vector2f(980, 560), sf::Vector2f(150, 45), sf::Color(255, 255, 255, 0));

        buttons[ButtonKey::Quit] = std::make_unique<Button>(textures->logoExit, sf::Vector2f(10, 20), sf::Vector2f(40, 40));
        //buttons[ButtonKey::Quit] = std::make_unique<Button>(textures->logoExit,sf::Vector2f(10, 20),sf::Vector2f(40, 40));
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
    drawAvatar(avatarIndex, WINDOW_WIDTH - 70, 20, 45.0f);

}
void SFMLGame::displayGame(){
    
    GameState gameData = client.getGameState();

    if (client.isGameStateUpdated()) {
        
        if(gameData.isGame){
            drawGrid(gameData.gridData);
            drawTetramino(gameData.currentPieceData);
            drawTetramino(gameData.nextPieceData);
            drawScore(gameData.scoreData);
            drawMessageMalusBonus(gameData.message);
            if(gameData.miniUpdate){
                int i = 0;
                for(const auto& miniGrid : gameData.miniGrid) {
                    drawMiniGrid(miniGrid["grid"], miniGridPositions[i]);
                    drawMiniTetra(miniGrid["tetra"], miniGridPositions[i]);
                    i++;
                }
            }
        } 
    }
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

    for (size_t row = 0; row < shape.size(); ++row) {
        for (size_t col = 0; col < shape[row].size(); ++col) {
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
        client.setGameStateUpdated(false);
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


void SFMLGame::displayWaitingRoom() {
    window->clear(sf::Color(30, 30, 60));
    /*float startY = 80;
    float spacing = 70;

    auto ranking = client.getServerData()["data"]; // Récupérer la liste d'amis
    for (size_t i = 0; i < ranking.size() && i < 10; ++i) {
        // Rectangle fictif pour la carte du joueur
        Rectangle friendCard(sf::Vector2f(90, startY + i * spacing), sf::Vector2f(600, 60), sf::Color::Transparent, sf::Color(200, 200, 200));
        friendCard.draw(*window);
        //Nom du joueur
        Text name(ranking[i], font, 20, sf::Color::Black, sf::Vector2f(150, startY + i * spacing + 15));
        name.draw(*window);
        
    }*/
    //displayBackground(textures->lobby);
    

    Text title("WAITING IN THE LOBBY", font, 50, sf::Color::White, sf::Vector2f(WINDOW_WIDTH/2 - 250, 30));
    
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
            /*std::cout << "Friends: " << friendsLobby.size() << std::endl;
            for (const auto& friendName : friendsLobby) {
                std::cout << "Friend: " << friendName << std::endl;
            }*/
        }
        
        
        /*if(!(std::find(pseudos.begin(), pseudos.end(), pseudo) != pseudos.end())){
            pseudos.push_back(pseudo);
            
        }*/
        Text player("Players: " , font, 20, sf::Color::White, sf::Vector2f(1065, startY));
        player.draw(*window);
        Text observer("Observers: " , font, 20, sf::Color::White, sf::Vector2f(1280, startY));
        observer.draw(*window);
        if(!pseudos.empty()){
            for(size_t i = 0; i < pseudos["player"].size(); ++i){
            
                Text pseudoText(pseudos["player"][i], font, 20, sf::Color::White, sf::Vector2f(1070, spacing + startY + i * spacing));
                pseudoText.draw(*window);
                
            }
            for(size_t i = 0; i < pseudos["observer"].size(); ++i){
                
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

            size_t colonPos = line.find(":");
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

        for (size_t i = 0; i < friendsLobby.size(); ++i) {
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

        if(invite){
            buttons[ButtonKey::esc] = std::make_unique<Button>(textures->esc, sf::Vector2f(995, 90), sf::Vector2f(30, 30));
        }
        texts[TextFieldKey::Speed] = std::make_unique<TextField>(font, 30, sf::Color::Black, sf::Color::White,
            sf::Vector2f(50, 150), sf::Vector2f(200, 50), "Speed");
        buttons[ButtonKey::Valider] = std::make_unique<Button>("Valider", font, 24, sf::Color::White, sf::Color(70, 200, 70),
            sf::Vector2f(270, 150), sf::Vector2f(100, 50));
        

        if (classic || royale) {
            texts[TextFieldKey::NbreJoueurs] = std::make_unique<TextField>(font, 30, sf::Color::Black, sf::Color::White,
                sf::Vector2f(50, 220), sf::Vector2f(200, 50), "Nbre joueurs");
            buttons[ButtonKey::ValiderNb] = std::make_unique<Button>("Valider Nb", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                sf::Vector2f(270, 220), sf::Vector2f(100, 50));

           
            
        }

        if (royale) {
            texts[TextFieldKey::Energie] = std::make_unique<TextField>(font, 30, sf::Color::Black, sf::Color::White,
                sf::Vector2f(50, 290), sf::Vector2f(200, 50), "Énergie");
            buttons[ButtonKey::ValiderEnergie] = std::make_unique<Button>("Valider Energie", font, 24, sf::Color::White, sf::Color(255, 165, 0),
                sf::Vector2f(270, 290), sf::Vector2f(150, 50));

            
        }

        float yPos = 250;
        if (royale) yPos = 360;
        else if (classic) yPos = 290;

        buttons[ButtonKey::InviteP_O] = std::make_unique<Button>(textures->P_O, sf::Vector2f(200, yPos), sf::Vector2f(120, 120));

        yPos += 120;


        /*buttons[ButtonKey::retour] = std::make_unique<Button>("Quitter", font, 24, sf::Color::White, sf::Color::Red,
                sf::Vector2f(200, yPos), sf::Vector2f(150, 50));*/

        

        
        

        
    }

    drawTextFields();
    drawButtons();
    quitter->draw(*window);


    if (buttons.count(ButtonKey::esc) && invite) {
    //std::cout << "Button esc exists" << std::endl;
        if (buttons[ButtonKey::esc]->isClicked(*window)) {
            std::cout << "Esc button clicked" << std::endl;
            invite = false;
            inviteScrollOffset = 0;
            inviteMaxScroll = 0;
            //buttons.erase(ButtonKey::esc);
            cleanup();
            inviteFriends.clear();
            isInvite.clear();
            return;
        }
    }


    if (!invite && buttons.count(ButtonKey::Valider) && buttons[ButtonKey::Valider]->isClicked(*window)) {
            std::string speed = texts[TextFieldKey::Speed]->getText();
            if(speed.empty()) {
                std::cerr << "Rempliez le champs de Vitesse " << std::endl;
                return;
            }
            std::cout << "Vitesse: " << "/speed/" + speed << std::endl;
            client.sendInputFromSFML("/speed/" + speed);
            texts[TextFieldKey::Speed]->setText("");
            return;
            
    }

   
    if (!invite && quitter->isClicked(*window)) {
            
        std::cout << "quitter: " << "/back"<< std::endl;
        client.sendInputFromSFML("/back");
        return;
        
    }

    if (!invite && (classic || royale)  && buttons.count(ButtonKey::ValiderNb) && buttons[ButtonKey::ValiderNb]->isClicked(*window)) {
            std::string nbPlayers = texts[TextFieldKey::NbreJoueurs]->getText();
            if(nbPlayers.empty()) {
                std::cerr << "Rempliez le champs de Nombre de joueurs " << std::endl;
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
                std::cerr << "Rempliez le champs d'énergie " << std::endl;
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
    drawAvatar(avatarIndex, WINDOW_WIDTH - 70, 20, 45.0f);
    
}

void SFMLGame::displayJoinGame() {
    window->clear(sf::Color(30, 30, 60)); // Fond bleu nuit
    int avatarIndex = client.getAvatarIndex();
    drawAvatar(avatarIndex, WINDOW_WIDTH - 70, 20, 45.0f);

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
        //std::cout << "requests: " << requests.size() << std::endl;

        Text t(titre, font, 40, sf::Color::White, sf::Vector2f(20, 100));
        t.draw(*window);

        std::string line;
        if (requests.empty()) {
            line = "Aucune demande de jeu";
            Text requ(line, font, 40, sf::Color::Red, sf::Vector2f(20, 170));
            requ.draw(*window);
        } else {
            // Synchroniser acceptInvite avec requests
            std::set<std::string> currentInvitations;
            int i = 0;

            for (auto& [key, value] : requests.items()) {
                line = key + value.get<std::string>();
                std::string message = line;
                //std::cout << "Message: " << message << std::endl;

                // Extraire les informations de l'invitation
                size_t startPos = message.find("GameRoom '");
                std::string gameRoomNumber, inviter, status;
                if (startPos != std::string::npos) {
                    startPos += std::string("GameRoom '").length();
                    size_t endPos = message.find("'", startPos);
                    if (endPos != std::string::npos) {
                        gameRoomNumber = message.substr(startPos, endPos - startPos);
                    }

                    // Extraire le nom de l'invitant
                    size_t inviterStart = message.find("par '");
                    if (inviterStart != std::string::npos) {
                        inviterStart += std::string("par '").length();
                        size_t inviterEnd = message.find("'", inviterStart);
                        if (inviterEnd != std::string::npos) {
                            inviter = message.substr(inviterStart, inviterEnd - inviterStart);
                        }
                    }

                    // Extraire le statut (joueur/observateur)
                    size_t statusStart = message.find("en tant que '");
                    if (statusStart != std::string::npos) {
                        statusStart += std::string("en tant que '").length();
                        size_t statusEnd = message.find("'", statusStart);
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
                    acceptInvite[invitationKey].push_back(std::make_unique<Button>(textures->accept, sf::Vector2f(buttonX, buttonY), sf::Vector2f(25, 25)));
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
                for (const auto& buttonA : buttonsA) {
                    buttonA->draw(*window);
                }
            }

            // Gérer les clics sur les boutons
            for (const auto& [invitationKey, buttonsA] : acceptInvite) {
                for (const auto& buttonA : buttonsA) {
                    if (buttonA->isClicked(*window)) {
                        

                        // Extraire les informations de la clé
                        size_t firstDelim = invitationKey.find("|");
                        std::string gameRoomNumber = invitationKey.substr(0, firstDelim);
                        std::cout << "accept." << gameRoomNumber << std::endl;

                        // Envoyer la réponse au serveur
                        client.sendInputFromSFML("accept." + gameRoomNumber);
                        j[jsonKeys::ACTION] = "AcceptRejoindre";
                        network->sendData(j.dump() + "\n", client.getClientSocket());
                        acceptInvite.clear();
                        break;
                    }
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
        std::cout << "Joueur Cible nkhtar ana" << std::endl;

        if (buttons.empty() && texts.empty()) {
            
            //std::cout<<"ana hna bach tkhtar" << std::endl;

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
                std::cerr << "Rempliez le champs de Victime " << std::endl;
                return;
            }
            //std::cout << "Victime choisi: " << id << std::endl;
            client.sendInputFromSFML(id);
            texts[TextFieldKey::Victime]->setText("");
            return;
        }
        
        sf::Text messageText(message, font, 24);
        messageText.setFillColor(sf::Color::White);
        messageText.setPosition(1,650);
        window->draw(messageText);

    }
    
    
    /*--------------------------------------------------------------------------------*/
    else if(msg[jsonKeys::PROPOSITION_CIBLE]){
        int id = msg[jsonKeys::CIBLE_ID];
        std::string message = std::string("Joueur Cible") + " "+  std::to_string(id);

        if (buttons.empty()) {
            //std::cout<<"ana yes or no" << std::endl;
            buttons[ButtonKey::Yes] = std::make_unique<Button>("Yes", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                sf::Vector2f(290, 670), sf::Vector2f(90, 30));

            buttons[ButtonKey::No] = std::make_unique<Button>("No", font, 24, sf::Color::White, sf::Color::Red,
                sf::Vector2f(410, 670), sf::Vector2f(90, 30));
            
            
        }

        drawButtons();

        if (buttons.count(ButtonKey::Yes) && buttons[ButtonKey::Yes]->isClicked(*window)) {
            //std::cout << "ana Yes" << std::endl;
            client.sendInputFromSFML("y");
            return;
        }

        if (buttons.count(ButtonKey::No) && buttons[ButtonKey::No]->isClicked(*window)) {
            //std::cout << "ana No" << std::endl;
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
            //std::cout << "ana malus" << std::endl;
            client.sendInputFromSFML("1");
            return;
        }

        if (buttons.count(ButtonKey::Bonus) && buttons[ButtonKey::Bonus]->isClicked(*window)) {
            //std::cout << "ana bonus" << std::endl;
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
            //std::cout << "ana inversion" << std::endl;
            client.sendInputFromSFML("1");
            return;
        }

        if (buttons.count(ButtonKey::Blocage) && buttons[ButtonKey::Blocage]->isClicked(*window)) {
            //std::cout << "ana blocage" << std::endl;
            client.sendInputFromSFML("2");
            return;
        }

        if (buttons.count(ButtonKey::ChuteRapide) && buttons[ButtonKey::ChuteRapide]->isClicked(*window)) {
            //std::cout << "ana chute" << std::endl;
            client.sendInputFromSFML("3");
            return;
        }

        if (buttons.count(ButtonKey::Suppression) && buttons[ButtonKey::Suppression]->isClicked(*window)) {
            //std::cout << "ana suppri" << std::endl;
            client.sendInputFromSFML("4");
            return;
        }

        if (buttons.count(ButtonKey::EcranNoir) && buttons[ButtonKey::EcranNoir]->isClicked(*window)) {
            //std::cout << "ana noir" << std::endl;
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
            //std::cout << "ana ralentir" << std::endl;
            client.sendInputFromSFML("1");
            return;
        }

        if (buttons.count(ButtonKey::MiniBlocs) && buttons[ButtonKey::MiniBlocs]->isClicked(*window)) {
            client.sendInputFromSFML("2");
            return;
        }
    }
}

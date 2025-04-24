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

    // Load textures
    textures->loadTextures();
}
void SFMLGame::update() {
    // Autres mises à jour de logique du jeu...
    
    std::string tempMessage = client.getTemporaryMessage();
    //std::cout << "----->Message temporaire: " << tempMessage << std::endl;
    if (!tempMessage.empty()) {
        afficherErreur(tempMessage);  // Appelle ta méthode d'affichage
        std::cout << "----->Message temporaire: " << tempMessage << std::endl;
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
    panneau.setPosition(150, 250);

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
    drawErreurMessage(); // 👈 affiche le panneau si actif

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

        std::cout << "Requête envoyée pour ajouter l'ami : " << friendName << std::endl;

        texts[TextFieldKey::AddFriendField]->clear();
        return;
    }

    // Traitement du bouton retour
    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = jsonKeys::FRIENDS;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
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

    // Requête au serveur pour les amis si data reçu
    /*if (!client.getServerData().empty() && client.getServerData().contains("amis")) {
        amis = client.getServerData()["amis"];
        client.clearServerData();
    }*/
    amis= { "Alice", "Bob", "Charlie", "Diana" }; // Exemple de données des demandes d'amis
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
        case MenuState::ManageRoom:
            //teamsMenu();
            break;
        case MenuState::CreateRoom:
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
    window->display();
}
void SFMLGame::friendListMenu() {
    // Fond
    displayBackground(textures->chat);

    // Barre latérale "Amis"
    Rectangle sidebar(sf::Vector2f(0, 0), sf::Vector2f(200, WINDOW_HEIGHT), sf::Color(30, 30, 50), sf::Color(80, 80, 100));
    sidebar.draw(*window);

    // Titre
    Text header("Mes amis", font, 24, sf::Color::White, sf::Vector2f(20, 10));
    header.draw(*window);

    // Chargement de la liste des amis (à remplacer par les données serveur si besoin)
    amis = { "Alice", "Bob", "Charlie", "Diana" };
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

        if (buttons[ButtonKey::CancelRemoveFriend]->isClicked(*window)) {
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

    // Affichage du message d'erreur s'il est actif

    // Gérer les clics sur les boutons
    if (buttons.count(ButtonKey::Retour) && buttons[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = jsonKeys::WELCOME;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
        return;
    }

    if (buttons.count(ButtonKey::Registre) && buttons[ButtonKey::Registre]->isClicked(*window)) {
        std::string username = texts[TextFieldKey::Username]->getText();
        std::string password = texts[TextFieldKey::Password]->getText();
        std::string confirmPassword = texts[TextFieldKey::ConfirmPassword]->getText();
        if (username.empty() || password.empty() || confirmPassword.empty()) {
            afficherErreur("Tous les champs doivent etre remplis.");
            return;
        }
        
        if (password != confirmPassword) {
            afficherErreur("Les mots de passe ne correspondent pas.");
            return;
        }

        json j = {
            {jsonKeys::ACTION, "register"},
            {jsonKeys::USERNAME, username},
            {jsonKeys::PASSWORD, password}
        };
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
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



void SFMLGame::mainMenu() {
    // Afficher l'arrière-plan du menu principal
    displayBackground(textures->logoConnexion);

    float buttonWidth = 120;
    float buttonHeight = 50;
    float spacing = 40;
    float startX = 100;
    float y = 560;

    // Couleurs harmonisées
    sf::Color background = sf::Color(30, 30, 30, 180); // noir semi-transparent
    sf::Color outline = sf::Color(135, 206, 250);        // bleu ciel
    sf::Color text = sf::Color::White;

    // Ajouter les boutons s'ils n'existent pas
    if (buttons.empty()) {
    // Boutons principaux
        buttons[ButtonKey::Ranking] = std::make_unique<Button>("Ranking", font, 26, text, background, sf::Vector2f(sf::Vector2f(startX, y)), sf::Vector2f(buttonWidth, buttonHeight), outline);
        buttons[ButtonKey::Play] = std::make_unique<Button>("Play", font, 26, text, background, sf::Vector2f(startX + (buttonWidth + spacing), y + 30), sf::Vector2f(buttonWidth, buttonHeight), outline);
        buttons[ButtonKey::Chat] = std::make_unique<Button>("Chat", font, 26, text, background, sf::Vector2f(startX + 2 * (buttonWidth + spacing), y + 30), sf::Vector2f(buttonWidth, buttonHeight), outline);
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
        std::cout << "Friends button clicked" << std::endl;
        std::cout << j.dump() << std::endl;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    } 
    else if(buttons.count(ButtonKey::Ranking) && buttons[ButtonKey::Ranking]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::RANKING;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
    else if(buttons.count(ButtonKey::Settings) && buttons[ButtonKey::Settings]->isClicked(*window)) {
        // Action pour le bouton "Settings"
    } else if (buttons.count(ButtonKey::Notification) && buttons[ButtonKey::Notification]->isClicked(*window)) {
        // Action pour le bouton "Notification"
    } else if (buttons.count(ButtonKey::Profile) && buttons[ButtonKey::Profile]->isClicked(*window)) {
        // Action pour le bouton "Profile"
    }
    else if (buttons.count(ButtonKey::Quit) && buttons[ButtonKey::Quit]->isClicked(*window)) {
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
        Button backButton(textures->logoMain,sf::Vector2f(7, 50), sf::Vector2f(25, 35));        // Bouton pour envoyer le message
        Button sendButton(">", font, 20, sf::Color::White, sf::Color(70, 200, 70),
                          sf::Vector2f(WINDOW_WIDTH - 40, WINDOW_HEIGHT - 40), sf::Vector2f(35, 35));
        // Ajout des boutons au vecteur
        buttons[ButtonKey::Retour] = std::make_unique<Button>(backButton);
        buttons[ButtonKey::Send] = std::make_unique<Button>(sendButton);
    }

    // Dessiner les champs de texte et les boutons
    drawTextFields();
    drawButtons();
    

    if (!client.getServerData().empty() && client.getServerData().contains("data")) {
        contacts = client.getServerData()["data"];
        client.clearServerData();
    }
    // Gérer les clics sur les contacts
    handleContacts();
    //dessiner les contacts
    drawContacts();

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
    // Gérer les messages du serveur
    getMessagesFromServer();
    // Afficher les messages
    drawMessages();
}

void SFMLGame::handleContacts() {
    static std::vector<sf::Texture> avatarTextures(20);
    const float contactHeight = 50.0f;

    // Gestion des contacts
    for (size_t i = 0; i < std::min(contacts.size(), avatarTextures.size()); ++i) {
        float contactY = 100 + i * contactHeight;
        if (!chatContacts.count(contacts[i])) {
            chatContacts[contacts[i]] = std::make_unique<Button>(
                contacts[i], font, 20, sf::Color::White, sf::Color::Transparent,
                sf::Vector2f(0, contactY), sf::Vector2f(200, contactHeight), sf::Color::Transparent);
        }

        if (avatarTextures[i].loadFromFile("../../res/avatar/avatar" + std::to_string(i+1) + ".png")) {
            Circle(sf::Vector2f(7, contactY + 8), 30.0f, sf::Color::White, sf::Color::Transparent)
                .drawPhoto(avatarTextures[i], *window);
        }
    }

    // Gestion des clics
    for (const auto& [contact, button] : chatContacts) {
        if (button->isClicked(*window)) {
            clickedContact = contact;
            network->sendData(json{{"action", "openChat"}, {"contact", contact}}.dump() + "\n", 
                            client.getClientSocket());
            break;
        }
    }

    // Affichage du chat sélectionné
    if (!clickedContact.empty()) {
        Rectangle(sf::Vector2f(202, 0), sf::Vector2f(WINDOW_WIDTH - 200, 50),
                 sf::Color(50, 50, 70), sf::Color(100, 100, 120)).draw(*window);
        Text(clickedContact, font, 24, sf::Color::White, sf::Vector2f(250, 10)).draw(*window);
        Circle(sf::Vector2f(215, 10), 30.0f, sf::Color::White, sf::Color::Transparent)
            .drawPhoto(avatarTextures[0], *window);
    }
}

void SFMLGame::getMessagesFromServer() {
    // Récupérer les messages du serveur
    auto message = client.getServerData();
    if (message.contains("sender") && message["sender"] == clickedContact) {
        messages.push_back({message["sender"], message["message"]});
    }
    client.clearServerData();
}

void SFMLGame::drawContacts() {
    // Afficher la liste des contacts
    for(auto& [contact, button] : chatContacts) {
        button->draw(*window);
    }
}


void SFMLGame::drawMessages() {
    for (auto& [sender, message] : messages) {
        if(sender == clickedContact || sender == "You") {
            displayMessage(sender, message);
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
    
    else if (buttons.count(ButtonKey::Join) && buttons[ButtonKey::Join]->isClicked(*window)){
        j[jsonKeys::ACTION] = "Rejoindre";
        client.sendInputFromSFML("2");
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
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
    }

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
                cell.setFillColor(sf::Color(50, 50, 50));
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


void SFMLGame::displayWaitingRoom() {
    window->clear(sf::Color(30, 30, 60));
    

    sf::Text title("SALLE D'ATTENTE", font, 50);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setPosition((WINDOW_WIDTH - title.getLocalBounds().width)/2, 50);
    window->draw(title);

    if (buttons.empty() && texts.empty()) {
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

        texts[TextFieldKey::NomJoueur] = std::make_unique<TextField>(font, 30, sf::Color::Black, sf::Color::White,
            sf::Vector2f(50, yPos), sf::Vector2f(420, 50), "Nom du joueur");

        yPos += 70;
        buttons[ButtonKey::InvitePlayer] = std::make_unique<Button>("Invite Player", font, 24, sf::Color::White, sf::Color(65, 105, 225),
                sf::Vector2f(50, yPos), sf::Vector2f(200, 50));
        buttons[ButtonKey::InviteObserver] = std::make_unique<Button>("Invite Observer", font, 24, sf::Color::White, sf::Color(100, 149, 237),
            sf::Vector2f(270, yPos), sf::Vector2f(200, 50));

        
    }

    drawTextFields();
    drawButtons();


    


    if (buttons.count(ButtonKey::Valider) && buttons[ButtonKey::Valider]->isClicked(*window)) {
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

    if ((classic || royale)  && buttons.count(ButtonKey::ValiderNb) && buttons[ButtonKey::ValiderNb]->isClicked(*window)) {
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

    if (royale  && buttons.count(ButtonKey::ValiderEnergie) && buttons[ButtonKey::ValiderEnergie]->isClicked(*window)) {
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

    

    if (buttons.count(ButtonKey::InvitePlayer) && buttons[ButtonKey::InvitePlayer]->isClicked(*window)) {
        std::string name = texts[TextFieldKey::NomJoueur]->getText();
        if(name.empty()) {
            std::cerr << "Rempliez le champs de Nom du joueur " << std::endl;
            return;
        }
        std::cout << "Invite Player: " <<"/invite/player/" + name<< std::endl;
        client.sendInputFromSFML("/invite/player/" + name);
        texts[TextFieldKey::NomJoueur]->setText("");
        return;
        
    }

    if (buttons.count(ButtonKey::InviteObserver) && buttons[ButtonKey::InviteObserver]->isClicked(*window)) {
        std::string name = texts[TextFieldKey::NomJoueur]->getText();
        if(name.empty()) {
            std::cerr << "Rempliez le champs de Nom du joueur " << std::endl;
            return;
        }
        std::cout << "Invite Observer: " <<"/invite/observer/" + name<< std::endl;
        client.sendInputFromSFML("/invite/observer/" + name);
        texts[TextFieldKey::NomJoueur]->setText("");
        return;
    }
}

void SFMLGame::displayJoinGame() {
    // Effacer la fenêtre avec un fond uni
    window->clear(sf::Color(30, 30, 60)); // Fond bleu nuit

    // Créer et configurer le texte du titre
    sf::Text title("DEMANDES DE JEU", font, 40); // Taille réduite à 40
    title.setFillColor(sf::Color::White); // Couleur blanche simple
    title.setStyle(sf::Text::Bold); // Gras seulement

    // Centrer horizontalement en haut de l'écran
    sf::FloatRect titleRect = title.getLocalBounds();
    title.setOrigin(titleRect.left + titleRect.width/2.0f, 0);
    title.setPosition(WINDOW_WIDTH/2.0f, 30); // 30px depuis le haut

    if (client.isGameStateUpdated()) {
        
        GameState gameData = client.getGameState();
        json requests = gameData.menu[jsonKeys::OPTIONS];
        std::string titre = gameData.menu[jsonKeys::TITLE];

        sf::Text t(titre, font, 40);
        t.setFillColor(sf::Color::White);
        t.setPosition(20, 100); // Positionner le texte à 100 pixels du haut
        window->draw(t);


        std::string line;
        if(requests.empty()) {
            line = "Aucune demande de jeu";
            sf::Text requ(line, font, 40);
            requ.setFillColor(sf::Color::White);
            requ.setPosition(20, 150); // Positionner le texte à 100 pixels du haut
            window->draw(requ);
        }

        int i = 0;
        for (auto& [key, value] : requests.items()) {
            line = key + value.get<std::string>();
            std::string message = line;
            sf::Text requ(message, font, 20);
            requ.setFillColor(sf::Color::White);
            requ.setPosition(20, 150 + i); // Positionner le texte à 100 pixels du haut
            window->draw(requ);
            i+=50;
        }


        if (buttons.empty() && texts.empty()) {
            texts[TextFieldKey::Room] = std::make_unique<TextField>(font, 30, sf::Color::Black, sf::Color::White,
                sf::Vector2f(50, 150 + i), sf::Vector2f(200, 50), "room");

            buttons[ButtonKey::Valider] = std::make_unique<Button>("Valider", font, 24, sf::Color::White, sf::Color(70, 200, 70),
                sf::Vector2f(270, 150 + i), sf::Vector2f(100, 50));
            
        }
    
        drawTextFields();
        drawButtons();
        json j;

        if (buttons.count(ButtonKey::Valider) && buttons[ButtonKey::Valider]->isClicked(*window)) {
            std::string room = texts[TextFieldKey::Room]->getText();
            if(room.empty()) {
                std::cerr << "Rempliez le champs de Room " << std::endl;
                return;
            }
            std::cout << "Room choisi: " << room << std::endl;
            client.sendInputFromSFML("accept." + room);
            texts[TextFieldKey::Room]->setText("");
            j[jsonKeys::ACTION] = "AcceptRejoindre";
            network->sendData(j.dump() + "\n", client.getClientSocket());
            return;
        }
    }


    // Dessiner le titre
    window->draw(title);
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
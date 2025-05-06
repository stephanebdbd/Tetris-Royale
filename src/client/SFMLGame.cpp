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

    // Load font
    if (!font.loadFromFile(FONT_PATH)) {
        std::cerr << "Erreur: Impossible de charger la police." << std::endl;
    }
    std::vector<std::string> paths;
    for (int i = 1; i <= 10; ++i) {
        paths.push_back("../../res/avatar/avatar" + std::to_string(i) + ".png");
    }
    avatarManager->loadAvatarPaths(paths);

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
        /*for(const auto& buttonA : buttonsA) {
            
        }*/
        
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

    json j;
    // Traitement du bouton envoyer
    if (buttons[ButtonKey::Send]->isClicked(*window) && !texts[TextFieldKey::AddFriendField]->getText().empty()) {
        std::string friendName = texts[TextFieldKey::AddFriendField]->getText();
        j[jsonKeys::ACTION] = jsonKeys::ADD_FRIEND;
        j["friend"] = friendName;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        //afficherErreur("Demande d'ami envoyée à " + friendName + ". .");
        texts[TextFieldKey::AddFriendField]->clear();
        return;
    }

    // Traitement du bouton retour  
    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
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
            Circle circle(sf::Vector2f(7, y + 8), 30.0f, sf::Color::White, sf::Color::Transparent);
            circle.setTexture(avatarTextures[i]);
            circle.draw(*window);
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
        j[jsonKeys::ACTION] = jsonKeys::FRIENDS;
        network->sendData(j.dump() + "\n", client.getClientSocket());
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

    json j;
    // Actions associées aux boutons
    if (buttons[ButtonKey::AddFriend]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::ADD_FRIEND_MENU;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    if (buttons[ButtonKey::FriendList]->isClicked(*window)) {
        // Afficher la liste des amis
        j[jsonKeys::ACTION] = jsonKeys::FRIEND_LIST;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
        return;
    }

    if (buttons[ButtonKey::FriendRequestList]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::FRIEND_REQUEST_LIST;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
        return;
    }

    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::MAIN;
        network->sendData(j.dump() + "\n", client.getClientSocket());
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
        case MenuState::Team:
            teamsMenu();
            break;
        case MenuState::CreatTeamMenu:
            createRoomMenu();
            break;
        case MenuState::JoinTeam:
            joinTeamMenu();
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
void SFMLGame::createRoomMenu() {
    // Afficher l'arrière-plan
    displayBackground(textures->chat);

    // Titre principal
    Text title("Créer une équipe", font, 30, sf::Color::White, sf::Vector2f(250, 30));
    title.draw(*window);

    // Slogan ou aide en dessous du titre
    Text subtitle("Entrez le nom de votre équipe", font, 18, sf::Color(200, 200, 220), sf::Vector2f(250, 80));
    subtitle.draw(*window);

    // Création du champ de texte si pas encore fait
    if (texts.empty()) {
        TextField teamNameField(font, 20, sf::Color::Black, sf::Color::White,
                                sf::Vector2f(250, 130), sf::Vector2f(300, 40), "Nom de l'équipe");
        texts[TextFieldKey::TeamNameField] = std::make_unique<TextField>(teamNameField);
    }

    // Création des boutons si pas encore fait
    if (buttons.empty()) {
        Button createButton("Créer", font, 20, sf::Color::White, sf::Color(70, 170, 250),
                            sf::Vector2f(250, 190), sf::Vector2f(140, 45));

        Button backButton("Retour", font, 20, sf::Color::White, sf::Color(180, 70, 70),
                          sf::Vector2f(410, 190), sf::Vector2f(140, 45));

        buttons[ButtonKey::Create] = std::make_unique<Button>(createButton);
        buttons[ButtonKey::Retour] = std::make_unique<Button>(backButton);
    }

    // Dessin des éléments
    drawTextFields();
    drawButtons();

    json j;
    // Traitement du bouton "Créer"
    if (buttons[ButtonKey::Create]->isClicked(*window) && !texts[TextFieldKey::TeamNameField]->getText().empty()) {
        std::string teamName = texts[TextFieldKey::TeamNameField]->getText();
        j[jsonKeys::ACTION] = jsonKeys::CREATE_TEAM;
        j[jsonKeys::TEAM_NAME] = teamName;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        //afficherErreur("Équipe créée : " + teamName);
        texts[TextFieldKey::TeamNameField]->clear();
        return;
    }

    // Traitement du bouton "Retour"
    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::TEAMS;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
        return;
    }
}
void SFMLGame::joinTeamMenu() {
    // Afficher l'arrière-plan
    displayBackground(textures->chat);

    // Titre principal
    Text title("Rejoindre une équipe", font, 30, sf::Color::White, sf::Vector2f(250, 30));
    title.draw(*window);

    // Slogan ou aide en dessous du titre
    Text subtitle("Entrez le nom de l'équipe que vous souhaitez rejoindre", font, 18, sf::Color(200, 200, 220), sf::Vector2f(250, 80));
    subtitle.draw(*window);

    // Création du champ de texte si pas encore fait
    if (texts.empty()) {
        TextField teamNameField(font, 20, sf::Color::Black, sf::Color::White,
                                sf::Vector2f(250, 130), sf::Vector2f(300, 40), "Nom de l'équipe");
        texts[TextFieldKey::TeamNameField] = std::make_unique<TextField>(teamNameField);
    }

    // Création des boutons si pas encore fait
    if (buttons.empty()) {
        Button joinButton("Rejoindre", font, 20, sf::Color::White, sf::Color(70, 170, 250),
                          sf::Vector2f(250, 190), sf::Vector2f(140, 45));

        Button backButton("Retour", font, 20, sf::Color::White, sf::Color(180, 70, 70),
                          sf::Vector2f(410, 190), sf::Vector2f(140, 45));

        buttons[ButtonKey::Join] = std::make_unique<Button>(joinButton);
        buttons[ButtonKey::Retour] = std::make_unique<Button>(backButton);
    }

    // Dessin des éléments
    drawTextFields();
    drawButtons();

    json j;
    // Traitement du bouton "Rejoindre"
    if (buttons[ButtonKey::Join]->isClicked(*window) && !texts[TextFieldKey::TeamNameField]->getText().empty()) {
        std::string teamName = texts[TextFieldKey::TeamNameField]->getText();
        j[jsonKeys::ACTION] = jsonKeys::JOIN_TEAM;
        j[jsonKeys::TEAM_NAME] = teamName;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        afficherErreur("Demande envoyée pour rejoindre l'équipe : " + teamName);
        texts[TextFieldKey::TeamNameField]->clear();
        return;
    }

    // Traitement du bouton "Retour"
    if (buttons[ButtonKey::Retour]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::TEAMS;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        cleanup();
        return;
    }
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
    // Affichage des amis
    auto amis = client.getAmis();
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
            Circle circle(sf::Vector2f(7, y + 8), 30.0f, sf::Color::White, sf::Color::Transparent);
            circle.setTexture(avatarTextures[i]);
            circle.draw(*window);
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
        j[jsonKeys::ACTION] = jsonKeys::FRIENDS;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
}


// Compléter le switch dans run()
void SFMLGame::run() {
    // Start client threads (they're now managed by the Client class)
    std::thread clientThread([this]() { client.run("gui"); });
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

    if (buttons[ButtonKey::Close]->isClicked(*window)) {
        // pour fermer la boîte d'informations
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


void SFMLGame::rankingMenu(){
    //afficher la background
    displayBackground(textures->ranking);
    // Header for ranking list
    Text header("LeaderBoard", font, 24, sf::Color::White, sf::Vector2f(400, 20));
    header.draw(*window);


    if(buttons.empty()){
        buttons[ButtonKey::Retour] = std::make_unique<Button>(textures->logoExit, sf::Vector2f(10, 20), sf::Vector2f(40, 40));
        buttons[ButtonKey::Settings] = std::make_unique<Button>(textures->logoSettings,sf::Vector2f(WINDOW_WIDTH - 130, 20), sf::Vector2f(35, 35));
        buttons[ButtonKey::Notification] = std::make_unique<Button>(textures->logoNotification, sf::Vector2f(WINDOW_WIDTH - 190, 20), sf::Vector2f(45, 45));
        buttons[ButtonKey::Profile] = std::make_unique<Button>("", font, 24, sf::Color::Transparent, sf::Color::White,
                                    sf::Vector2f(WINDOW_WIDTH - 70, 20), sf::Vector2f(35, 35), sf::Color::Transparent);
    }

    drawButtons();

    // Affichage de la liste d'amis (exemple visuel)
    float startY = 80;
    float spacing = 70;
    auto ranking1 = client.getRanking();
        //currentState = MenuState::classement;
        if(ranking1.empty() ) {
            std::cout << "Aucun classement trouvé." << std::endl;
            return;
        }
        int i = 0;
        //std::cout << "Liste des joueurs reçue avec succès." << std::endl;
        for (const auto& [username, details] : ranking1) {
            std::string bestScore = details[0];      // Score
            std::string avatarNumber = details[1];   // ID d'avatar
        
            //std::cout << "------->   Joueur: " << username << " avatar " << avatarNumber << std::endl;
        
            // Position de base pour cette ligne
            float yOffset = startY + i * spacing;
        
            // Avatar
            int avatarId = std::stoi(avatarNumber);
            if (avatarId >= 0 && avatarId < static_cast<int>(avatarManager->getAvatarPath().size())) {
                sf::Texture avatarTexture;
                if (avatarTexture.loadFromFile(avatarManager->getAvatarPath()[avatarId])) {
                    sf::CircleShape avatarCircle(30);
                    avatarCircle.setPosition(100, yOffset);  // Centré à gauche
                    avatarCircle.setTexture(&avatarTexture);
                    avatarCircle.setOutlineThickness(2);
                    avatarCircle.setOutlineColor(sf::Color::White);
                    window->draw(avatarCircle);
                }
            }
        
            // Nom du joueur (à droite de l’avatar)
            Text name(username, font, 22, sf::Color::White, sf::Vector2f(190, yOffset + 10));
            name.draw(*window);
        
            // Score du joueur (à droite complètement)
            Text score(bestScore, font, 20, sf::Color::Black, sf::Vector2f(300, yOffset + 10));
            score.draw(*window);
        
            i++;
        }
        
    if(buttons.count(ButtonKey::Retour) && buttons[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = jsonKeys::MAIN;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
}


void SFMLGame::teamsMenu() {
    displayBackground(textures->logoConnexion); 

    float buttonWidth = 300;
    float buttonHeight = 60;
    float spacing = 30;
    float centerX = WINDOW_WIDTH / 2.0f - buttonWidth / 2.0f;
    float startY = 250;

    sf::Color background = sf::Color(40, 40, 40, 200);
    sf::Color outline = sf::Color(135, 206, 250);
    sf::Color text = sf::Color::White;

    // Ajouter les boutons s'ils n'existent pas
    if (buttons.empty()) {
        buttons[ButtonKey::CreateTeam] = std::make_unique<Button>("Créer une équipe", font, 28, text, background,
            sf::Vector2f(centerX, startY),
            sf::Vector2f(buttonWidth, buttonHeight), outline);

        buttons[ButtonKey::JoinTeam] = std::make_unique<Button>("Rejoindre une équipe", font, 28, text, background,
            sf::Vector2f(centerX, startY + buttonHeight + spacing),
            sf::Vector2f(buttonWidth, buttonHeight), outline);

        buttons[ButtonKey::TeamInvites] = std::make_unique<Button>("Invitations reçues", font, 28, text, background,
            sf::Vector2f(centerX, startY + 2 * (buttonHeight + spacing)),
            sf::Vector2f(buttonWidth, buttonHeight), outline);

        buttons[ButtonKey::ManageTeams] = std::make_unique<Button>("Gérer mes équipes", font, 28, text, background,
            sf::Vector2f(centerX, startY + 3 * (buttonHeight + spacing)),
            sf::Vector2f(buttonWidth, buttonHeight), outline);
    }

    drawButtons();

    json j;
    if (buttons[ButtonKey::CreateTeam]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::CREATE_TEAM_MENU;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;

    } else if (buttons[ButtonKey::JoinTeam]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::JOIN_TEAM_MENU;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        //client.setCurrentMenuState(MenuState::JoinTeam);
        return;

    } else if (buttons[ButtonKey::TeamInvites]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::TEAM_INVITES;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        //j[jsonKeys::ACTION] = jsonKeys::TEAMS;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        return;

    } else if (buttons[ButtonKey::ManageTeams]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::MANAGE_TEAMS;
        network->sendData(j.dump() + "\n", client.getClientSocket());
        //client.setCurrentMenuState(MenuState::ManageTeams);
        return;
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
        j[jsonKeys::ACTION] = jsonKeys::MAIN;
        network->sendData(j.dump() + "\n", client.getClientSocket());
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

        if (avatarIndex >= 0 && avatarIndex < static_cast<int>(avatarManager->getAvatarPath().size())) {
            if (avatarTextures[avatarIndex].loadFromFile(avatarManager->getAvatarPath()[avatarIndex])) {
                Circle circle(sf::Vector2f(7, contactY + 8), 30.0f, sf::Color::White, sf::Color::Transparent);
                circle.setTexture(avatarTextures[avatarIndex]);
                circle.draw(*window);
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

        Circle circle(sf::Vector2f(215, 10), 30.0f, sf::Color::White, sf::Color::Transparent);
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

        if(!gameData.showCommand.empty()){
            if(gameData.showCommand == "player")
                showCommand = false;
            else if(gameData.showCommand == "observer"){
                showCommand = false;
                showInviteCommand = false;
            }
                
            
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

        if(invite && showInviteCommand){
            buttons[ButtonKey::esc] = std::make_unique<Button>(textures->esc, sf::Vector2f(995, 90), sf::Vector2f(30, 30));
        }
        if(showCommand){
            texts[TextFieldKey::Speed] = std::make_unique<TextField>(font, 30, sf::Color::Black, sf::Color::White,
                sf::Vector2f(50, 150), sf::Vector2f(200, 50), "Speed");
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
                sf::Vector2f(50, 290), sf::Vector2f(200, 50), "Énergie");
            buttons[ButtonKey::ValiderEnergie] = std::make_unique<Button>("Valider Energie", font, 24, sf::Color::White, sf::Color(255, 165, 0),
                sf::Vector2f(270, 290), sf::Vector2f(150, 50));

            
        }

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
        //std::cout << "requests: " << requests.size() << std::endl;

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
                    //acceptInvite[invitationKey].push_back(std::make_unique<Button>(textures->accept, sf::Vector2f(buttonX, buttonY), sf::Vector2f(25, 25)));
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
                /*for (const auto& buttonA : buttonsA) {
                    buttonA->draw(*window);
                }*/
            }

            // Gérer les clics sur les boutons
            for (const auto& [invitationKey, buttonsA] : acceptInvite) {
                if (buttonsA->isClicked(*window)) {
                        

                    // Extraire les informations de la clé
                    size_t firstDelim = invitationKey.find("|");
                    size_t secondDelim = invitationKey.find("|", firstDelim + 1);
                    std::string gameRoomNumber = invitationKey.substr(0, firstDelim);
                    std::string inviter = invitationKey.substr(firstDelim + 1, secondDelim - firstDelim - 1);
                    std::string status = invitationKey.substr(secondDelim + 1);

                    std::cout << "GameRoom Number: " << gameRoomNumber << std::endl;
                    std::cout << "Inviter: " << inviter << std::endl;
                    std::cout << "Status: " << status << std::endl;

                    std::cout << "accept." <<status+"."<<gameRoomNumber << std::endl;

                    // Envoyer la réponse au serveur
                    client.sendInputFromSFML("accept."+status+"." + gameRoomNumber);
                    j[jsonKeys::ACTION] = "AcceptRejoindre";
                    network->sendData(j.dump() + "\n", client.getClientSocket());
                    acceptInvite.clear();
                    break;
                }
                /*for (const auto& buttonA : buttonsA) {
                    
                }*/
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

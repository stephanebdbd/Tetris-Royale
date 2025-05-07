#include "MenuManager.hpp"
#include "SFMLGame.hpp"

const unsigned int WINDOW_WIDTH = 1500;
const unsigned int WINDOW_HEIGHT = 850;

MenuManager::MenuManager(sf::RenderWindow* window, sf::Font& font, Client& client, ClientNetwork& network, 
    Textures& textures, SFMLGame* sfmlGame, AvatarManager& avatarManager, std::map<ButtonKey, std::unique_ptr<Button>>& buttons, 
    std::map<TextFieldKey, std::unique_ptr<TextField>>& texts)
    : window(window), font(font), client(client), network(network), textures(&textures), sfmlGame(sfmlGame), 
    avatarManager(&avatarManager), buttons(&buttons), texts(&texts) {}


void MenuManager::drawButtons() {
    sfmlGame->drawErreurMessage();

    for (const auto& [_, button] : *buttons) {
        button->draw(*window);
    }
}

void MenuManager::welcomeMenu() {
    // Afficher l'arrière-plan du menu de bienvenue
    sfmlGame->displayBackground(textures->connexion);

    // Ajouter les boutons s'ils n'existent pas
    if (buttons->empty()) {
        (*buttons)[ButtonKey::Login] = std::make_unique<Button>("Login", font, 24, sf::Color::White, sf::Color(100, 149, 237),
                                                            sf::Vector2f(WINDOW_WIDTH / 2 - 220, 600), sf::Vector2f(200, 35));
        (*buttons)[ButtonKey::Registre] = std::make_unique<Button>("Registre", font, 24, sf::Color::White, sf::Color(255, 165, 0),
                                                            sf::Vector2f(WINDOW_WIDTH / 2 + 20, 600), sf::Vector2f(200, 35));
        (*buttons)[ButtonKey::Quit] = std::make_unique<Button>("Exit", font, 24, sf::Color::White, sf::Color(255, 99, 71),
                                                            sf::Vector2f(WINDOW_WIDTH / 2 - 100, 660), sf::Vector2f(200, 35));
    }

    // Utilise la méthode centrale de SFMLGame pour dessiner les boutons
    drawButtons();

    // Gérer les clics sur les boutons
    if ((*buttons)[ButtonKey::Quit]->isClicked(*window)) {
        sfmlGame->cleanup();
        window->close();
        return;
    }

    if ((*buttons)[ButtonKey::Registre]->isClicked(*window)) {
        client.setCurrentMenuState(MenuState::Register);
        return;
    }

    if ((*buttons)[ButtonKey::Login]->isClicked(*window)) {
        client.setCurrentMenuState(MenuState::Login);
        return;
    }
}

void MenuManager::connexionMenu() {
    // Afficher l'arrière-plan du menu de connexion
    sfmlGame->displayBackground(textures->connexion);

    // Ajouter les champs de texte et les boutons s'ils n'existent pas
    if (texts->empty()) {
        (*texts)[TextFieldKey::Username] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                    sf::Vector2f(WINDOW_WIDTH/2 - 200/2, 560), sf::Vector2f(200, 35), "Username.");
        (*texts)[TextFieldKey::Password] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                    sf::Vector2f(WINDOW_WIDTH/2 - 200/2, 610), sf::Vector2f(200, 35), "Password", true);
    }

    if (buttons->empty()) {
        (*buttons)[ButtonKey::Login] = std::make_unique<Button>("Se connecter", font, 24, sf::Color::White, sf::Color(100, 149, 237),
                                                             sf::Vector2f(WINDOW_WIDTH/2 - 200/2, 660), sf::Vector2f(200, 35));
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>("Retour", font, 24, sf::Color::White, sf::Color(255, 99, 71),
                                                              sf::Vector2f(WINDOW_WIDTH/2 - 200/2, 710), sf::Vector2f(200, 35));
    }

    // Dessiner les champs de texte et les boutons
    sfmlGame->drawTextFields();
    drawButtons();

    // Gérer les clics sur les boutons
    if (buttons->count(ButtonKey::Retour) && (*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        //currentState = MenuState::Welcome; // Retourner au menu de bienvenue
        sfmlGame->cleanup();
        json j;
        j[jsonKeys::ACTION] = jsonKeys::WELCOME;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    if (buttons->count(ButtonKey::Login) && (*buttons)[ButtonKey::Login]->isClicked(*window)) {
        // Récupérer les données des champs de texte
        std::string username = (*texts)[TextFieldKey::Username]->getText();
        std::string password = (*texts)[TextFieldKey::Password]->getText();

        if (username.empty() || password.empty()) {
            std::cerr << "Tous les champs doivent être remplis !" << std::endl;
            return;
        }

        json j = {
            {jsonKeys::ACTION, jsonKeys::LOGIN},
            {jsonKeys::USERNAME, username},
            {jsonKeys::PASSWORD, password}
        };
        network.sendData(j.dump() + "\n", client.getClientSocket());
        sfmlGame->cleanup();
    }
}


void MenuManager::mainMenu() {
    // Afficher l'arrière-plan du menu principal
    sfmlGame->displayBackground(textures->logoConnexion);

    float centerX = WINDOW_WIDTH / 2.0f;
    float y = 780;
    float spacing = 25;
    float buttonWidth = 120;
    float buttonHeight = 50;
    float decallage = 20;

    // Couleurs harmonisées
    sf::Color background = sf::Color(30, 30, 30, 180); // noir semi-transparent
    sf::Color outline = sf::Color(135, 206, 250);      // bleu ciel
    sf::Color text = sf::Color::White;

    // Ajouter les boutons s'ils n'existent pas
    if (buttons->empty()) {
        // Boutons principaux
        (*buttons)[ButtonKey::Play] = std::make_unique<Button>("Play", font, 26, text, background,
            sf::Vector2f(centerX - buttonWidth / 2.0f, y),  // un peu plus bas pour variation
            sf::Vector2f(buttonWidth, buttonHeight), outline);
        (*buttons)[ButtonKey::Chat] = std::make_unique<Button>("Chat", font, 26, text, background,
            sf::Vector2f(centerX + (buttonWidth/2.0f + spacing) , y-decallage),
            sf::Vector2f(buttonWidth, buttonHeight), outline);

        (*buttons)[ButtonKey::Ranking] = std::make_unique<Button>("Ranking", font, 26, text, background,
            sf::Vector2f(centerX + 2 * spacing + 1.5f*buttonWidth , y- 2.0f *decallage),
            sf::Vector2f(buttonWidth, buttonHeight), outline);
        
        (*buttons)[ButtonKey::Teams] = std::make_unique<Button>("Teams", font, 26, text, background,
            sf::Vector2f(centerX - (buttonWidth * 1.5f + spacing ), y - decallage),  // symétrique à Chat
            sf::Vector2f(buttonWidth, buttonHeight), outline);
        
        (*buttons)[ButtonKey::Friends] = std::make_unique<Button>("Friends", font, 26, text, background,
            sf::Vector2f(centerX - (2.5f * buttonWidth + 2 * spacing), y - 2.0f * decallage),  // symétrique à Ranking
            sf::Vector2f(buttonWidth, buttonHeight), outline);
            

        (*buttons)[ButtonKey::Quit] = std::make_unique<Button>(textures->logoExit,
                                                                sf::Vector2f(10, 20),
                                                                sf::Vector2f(40, 40));

        (*buttons)[ButtonKey::Settings] = std::make_unique<Button>(textures->logoSettings,
                                                                sf::Vector2f(WINDOW_WIDTH - 130, 20),
                                                                sf::Vector2f(35, 35));

        (*buttons)[ButtonKey::Notification] = std::make_unique<Button>(textures->logoNotification,
                                                                sf::Vector2f(WINDOW_WIDTH - 190, 20),
                                                                sf::Vector2f(45, 45));

        (*buttons)[ButtonKey::Profile] = std::make_unique<Button>("", font, 24, sf::Color::Transparent, sf::Color::White,
                                                                sf::Vector2f(WINDOW_WIDTH - 70, 20),
                                                                sf::Vector2f(45, 45), sf::Color::Transparent);
    }

    // Dessiner les boutons
    drawButtons();
    int avatarIndex = client.getAvatarIndex();
    avatarManager->drawAvatar(avatarIndex, WINDOW_WIDTH - 70, 20, 45.0f);
    

    // Gérer les clics sur les boutons
    json j;
    if (buttons->count(ButtonKey::Play) && (*buttons)[ButtonKey::Play]->isClicked(*window)) {
        j[jsonKeys::ACTION] = "createjoin";
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;

    } else if (buttons->count(ButtonKey::Chat) && (*buttons)[ButtonKey::Chat]->isClicked(*window)) {
        j[jsonKeys::ACTION] = "chat";
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;

    } else if (buttons->count(ButtonKey::Friends) && (*buttons)[ButtonKey::Friends]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::FRIENDS;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    } else if (buttons->count(ButtonKey::Teams) && (*buttons)[ButtonKey::Teams]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::TEAMS;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;

    } else if (buttons->count(ButtonKey::Ranking) && (*buttons)[ButtonKey::Ranking]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::RANKING;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;

    } else if (buttons->count(ButtonKey::Settings) && (*buttons)[ButtonKey::Settings]->isClicked(*window)) {
        // Action pour le bouton "Settings"

    } else if (buttons->count(ButtonKey::Notification) && (*buttons)[ButtonKey::Notification]->isClicked(*window)) {
        // Action pour le bouton "Notification"

    } else if (buttons->count(ButtonKey::Profile) && (*buttons)[ButtonKey::Profile]->isClicked(*window)) {
        // Action pour le bouton "Profile"
        j[jsonKeys::ACTION] = "player_info";
        network.sendData(j.dump() + "\n", client.getClientSocket());

    } else if (buttons->count(ButtonKey::Quit) && (*buttons)[ButtonKey::Quit]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::WELCOME;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
    auto data  = client.getServerData();
    if (data["message"] == "player_info") {
        client.setShow(true); // Active l'affichage des informations
    }
    
    if (client.getShow()) {
        sfmlGame->displayCurrentPlayerInfo();
    }
}


void MenuManager::registerMenu() {
    // Afficher l'arrière-plan du formulaire d'inscription
    sfmlGame->displayBackground(textures->connexion);

    // Titre principal
    Text title("Rejoignez la communaute", font, 30, sf::Color::White, sf::Vector2f(WINDOW_WIDTH / 2 - 150, 20));
    title.draw(*window);


    // Centrage des champs
    float fieldWidth = 250;
    float fieldHeight = 35;
    float centerX = WINDOW_WIDTH / 2 - fieldWidth / 2;
    float startY = 500;
    float fieldSpacing = 60;

    // Ajouter les champs de texte s'ils n'existent pas
    if (texts->empty()) {
        (*texts)[TextFieldKey::Username] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                     sf::Vector2f(centerX - 300, startY), sf::Vector2f(fieldWidth, fieldHeight), "Username");
        (*texts)[TextFieldKey::Password] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                     sf::Vector2f(centerX + 270, startY), sf::Vector2f(fieldWidth, fieldHeight), "Password", true);
        (*texts)[TextFieldKey::ConfirmPassword] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                            sf::Vector2f(centerX - 20, startY + fieldSpacing), sf::Vector2f(fieldWidth, fieldHeight), "Confirm Password", true);
    }

    // Section Sélection d'avatar
    float avatarTitleY = startY + 2 * fieldSpacing + 10;
    Text avatarTitle("Choisissez votre avatar:", font, 20, sf::Color::White, sf::Vector2f(WINDOW_WIDTH / 2 - 130, avatarTitleY));
    avatarTitle.draw(*window);

    // afficher la galerie d'avatar
    float avatarSize = 50;
    float avatarSpacing = 15;

    
    std::vector<std::string> avatarPaths = avatarManager->getAvatarPath();
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
            avatarCircle.setOutlineColor((avatarManager->getSelectedAvatar() == static_cast<int>(i)) ? sf::Color::Yellow : sf::Color::Transparent);
            avatarCircle.setFillColor(sf::Color(100, 100, 100));
            avatarCircle.setTexture(&avatarTex);

            window->draw(avatarCircle);

            sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
                std::sqrt(std::pow(mousePos.x - (avatarX + avatarSize / 2), 2) +
                          std::pow(mousePos.y - (currentY + avatarSize / 2), 2)) <= avatarSize / 2) {
                avatarManager->setSelectedAvatar(static_cast<int>(i)); 
                std::cout << "Avatar sélectionné : " << avatarManager->getSelectedAvatar() << std::endl;
            }
        }
    }

    // Ajouter les boutons s'ils n'existent pas
    float buttonWidth = 200;
    float buttonHeight = 35;
    float buttonX = WINDOW_WIDTH / 2 - buttonWidth / 2;
    float avatarRows = ( avatarPaths.size() + avatarsPerRow - 1) / avatarsPerRow;
    float buttonY = avatarY + avatarRows * (avatarSize + avatarSpacing);

    if (buttons->empty()) {
        (*buttons)[ButtonKey::Registre] = std::make_unique<Button>("S'inscrire", font, 24, sf::Color::White, sf::Color(100, 149, 237),
                                                                 sf::Vector2f(buttonX - 150, buttonY), sf::Vector2f(buttonWidth, buttonHeight));
                                                                 (*buttons)[ButtonKey::Retour] = std::make_unique<Button>("Retour", font, 24, sf::Color::White, sf::Color(255, 99, 71),
                                                               sf::Vector2f(buttonX + 150, buttonY), sf::Vector2f(buttonWidth, buttonHeight));
    }

    // Dessiner les champs de texte et les boutons
    sfmlGame->drawTextFields();
    drawButtons();

    // Affichage du message d'erreur s'il est actif
    sfmlGame->drawErreurMessage();

    // Gérer les clics sur les boutons
    if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        avatarManager->setSelectedAvatar(-1); // Réinitialiser la sélection
        json j;
        j[jsonKeys::ACTION] = jsonKeys::WELCOME;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        sfmlGame->cleanup();
        return;
    }

    if ((*buttons)[ButtonKey::Registre]->isClicked(*window)) {
        std::string username = (*texts)[TextFieldKey::Username]->getText();
        std::string password = (*texts)[TextFieldKey::Password]->getText();
        std::string confirmPassword = (*texts)[TextFieldKey::ConfirmPassword]->getText();

        // Validation des champs
        if (username.empty() || password.empty() || confirmPassword.empty()) {
            sfmlGame->afficherErreur("Tous les champs doivent être remplis.");
            return;
        }

        if (password != confirmPassword) {
            sfmlGame->afficherErreur("Les mots de passe ne correspondent pas.");
            return;
        }

        int selectedAvatar = avatarManager->getSelectedAvatar();
        if (selectedAvatar < 0 || selectedAvatar >= static_cast<int>(avatarPaths.size())) {
            sfmlGame->afficherErreur("Veuillez sélectionner un avatar.");
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

        network.sendData(j.dump() + "\n", client.getClientSocket());

        // Réinitialisation des champs
        (*texts)[TextFieldKey::Username]->clear();
        (*texts)[TextFieldKey::Password]->clear();
        (*texts)[TextFieldKey::ConfirmPassword]->clear();
        selectedAvatar = -1;
    }
}

void MenuManager::rankingMenu(){
    //afficher la background
    sfmlGame->displayBackground(textures->ranking);
    // Header for ranking list
    Text header("LeaderBoard", font, 24, sf::Color::White, sf::Vector2f(400, 20));
    header.draw(*window);


    if(buttons->empty()){
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(textures->logoExit, sf::Vector2f(10, 20), sf::Vector2f(40, 40));
        (*buttons)[ButtonKey::Settings] = std::make_unique<Button>(textures->logoSettings,sf::Vector2f(WINDOW_WIDTH - 130, 20), sf::Vector2f(35, 35));
        (*buttons)[ButtonKey::Notification] = std::make_unique<Button>(textures->logoNotification, sf::Vector2f(WINDOW_WIDTH - 190, 20), sf::Vector2f(45, 45));
        (*buttons)[ButtonKey::Profile] = std::make_unique<Button>("", font, 24, sf::Color::Transparent, sf::Color::White,
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
        for (const auto& [username, details] : ranking1) {
            std::string bestScore = details[0];      // Score
            std::string avatarNumber = details[1];   // ID d'avatar
        
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
        
    if(buttons->count(ButtonKey::Retour) && (*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = jsonKeys::MAIN;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
}

void MenuManager::teamsMenu() {
    sfmlGame->displayBackground(textures->logoConnexion); 

    float buttonWidth = 300;
    float buttonHeight = 60;
    float spacing = 30;
    float centerX = WINDOW_WIDTH / 2.0f - buttonWidth / 2.0f;
    float startY = 250;

    sf::Color background = sf::Color(40, 40, 40, 200);
    sf::Color outline = sf::Color(135, 206, 250);
    sf::Color text = sf::Color::White;

    // Ajouter les boutons s'ils n'existent pas
    if (buttons->empty()) {
        (*buttons)[ButtonKey::CreateTeam] = std::make_unique<Button>("Créer une équipe", font, 28, text, background,
            sf::Vector2f(centerX, startY),
            sf::Vector2f(buttonWidth, buttonHeight), outline);

        (*buttons)[ButtonKey::JoinTeam] = std::make_unique<Button>("Rejoindre une équipe", font, 28, text, background,
            sf::Vector2f(centerX, startY + buttonHeight + spacing),
            sf::Vector2f(buttonWidth, buttonHeight), outline);

        (*buttons)[ButtonKey::TeamInvites] = std::make_unique<Button>("Invitations reçues", font, 28, text, background,
            sf::Vector2f(centerX, startY + 2 * (buttonHeight + spacing)),
            sf::Vector2f(buttonWidth, buttonHeight), outline);

        (*buttons)[ButtonKey::ManageTeams] = std::make_unique<Button>("Gérer mes équipes", font, 28, text, background,
            sf::Vector2f(centerX, startY + 3 * (buttonHeight + spacing)),
            sf::Vector2f(buttonWidth, buttonHeight), outline);
    }

    drawButtons();

    json j;
    if ((*buttons)[ButtonKey::CreateTeam]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::CREATE_TEAM_MENU;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;

    } else if ((*buttons)[ButtonKey::JoinTeam]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::JOIN_TEAM_MENU;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;

    } else if ((*buttons)[ButtonKey::TeamInvites]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::TEAM_INVITES;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;

    } else if ((*buttons)[ButtonKey::ManageTeams]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::MANAGE_TEAM_MENU;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
}



// Gestion des champs de texte
void MenuManager::handleTextFieldEvents(sf::Event& event) {
    for (const auto& [_, field] : *texts) {
        field->handleInput(event);
    }
}
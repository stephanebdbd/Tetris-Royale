#include "MenuManager.hpp"
#include "SFMLGame.hpp"

const unsigned int WINDOW_WIDTH = 1500;
const unsigned int WINDOW_HEIGHT = 850;

MenuManager::MenuManager(sf::RenderWindow* window, sf::Font& font, Client& client, ClientNetwork& network, 
    Textures& textures, SFMLGame* sfmlGame, AvatarManager& avatarManager, std::map<ButtonKey, std::unique_ptr<Button>>& buttons, 
    std::map<TextFieldKey, std::unique_ptr<TextField>>& texts)
    : window(window), font(font), client(client), network(network), textures(&textures), sfmlGame(sfmlGame), 
    avatarManager(&avatarManager), buttons(&buttons), texts(&texts) {}

void MenuManager::welcomeMenu() {
    // Afficher l'arrière-plan du menu de bienvenue
    sfmlGame->displayBackground(textures->connexion);

    // Ajouter les boutons s'ils n'existent pas
    if (buttons->empty()) {
        (*buttons)[ButtonKey::Login] = std::make_unique<Button>("Connexion", font, 24, sf::Color::White, sf::Color(100, 149, 237),
                                                            sf::Vector2f(WINDOW_WIDTH / 2 - 220, 650), sf::Vector2f(200, 35));
        (*buttons)[ButtonKey::Registre] = std::make_unique<Button>("Inscription", font, 24, sf::Color::White, sf::Color(255, 165, 0),
                                                            sf::Vector2f(WINDOW_WIDTH / 2 + 20, 650), sf::Vector2f(200, 35));
        (*buttons)[ButtonKey::Quit] = std::make_unique<Button>("Quitter", font, 24, sf::Color::White, sf::Color(255, 99, 71),
                                                            sf::Vector2f(WINDOW_WIDTH / 2 - 100, 710), sf::Vector2f(200, 35));
    }

    // Utilise la méthode centrale de SFMLGame pour dessiner les boutons
    sfmlGame->drawButtons();

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
                                                                    sf::Vector2f(WINDOW_WIDTH/2 - 210, 650), sf::Vector2f(200, 35), "Pseudo");
        (*texts)[TextFieldKey::Password] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                    sf::Vector2f(WINDOW_WIDTH/2 +10, 650), sf::Vector2f(200, 35), "Mot de passe", true);
    }

    if (buttons->empty()) {
        (*buttons)[ButtonKey::Login] = std::make_unique<Button>("Se connecter", font, 24, sf::Color::White, sf::Color(100, 149, 237),
                                                             sf::Vector2f(WINDOW_WIDTH/2 - 210, 700), sf::Vector2f(200, 35));
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>("Retour", font, 24, sf::Color::White, sf::Color(255, 99, 71),
                                                              sf::Vector2f(WINDOW_WIDTH/2 + 10, 700), sf::Vector2f(200, 35));
    }

    // Dessiner les champs de texte et les boutons
    sfmlGame->drawTextFields();
    sfmlGame->drawButtons();

    // Gérer les clics sur les boutons
    if (buttons->count(ButtonKey::Retour) && (*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        // Retourner au menu de bienvenue
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
        (*buttons)[ButtonKey::Play] = std::make_unique<Button>("Jouer", font, 26, text, background,
            sf::Vector2f(centerX - buttonWidth / 2.0f, y),  // un peu plus bas
            sf::Vector2f(buttonWidth, buttonHeight), outline);
        (*buttons)[ButtonKey::Chat] = std::make_unique<Button>("Chat", font, 26, text, background,
            sf::Vector2f(centerX + (buttonWidth/2.0f + spacing) , y-decallage),
            sf::Vector2f(buttonWidth, buttonHeight), outline);

        (*buttons)[ButtonKey::Ranking] = std::make_unique<Button>("Classement", font, 26, text, background,
            sf::Vector2f(centerX + 2 * spacing + 1.5f*buttonWidth , y- 2.0f *decallage),
            sf::Vector2f(buttonWidth, buttonHeight), outline);
        
        (*buttons)[ButtonKey::Teams] = std::make_unique<Button>("Teams", font, 26, text, background,
            sf::Vector2f(centerX - (buttonWidth * 1.5f + spacing ), y - decallage),  // symétrique à Chat
            sf::Vector2f(buttonWidth, buttonHeight), outline);
        
        (*buttons)[ButtonKey::Friends] = std::make_unique<Button>("Amis", font, 26, text, background,
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
    sfmlGame->drawButtons();
    int avatarIndex = client.getAvatarIndex();
    avatarManager->drawAvatar(avatarIndex, WINDOW_WIDTH - 70, 20, 45.0f);
    

    // Gérer les clics sur les boutons
    json j;
    if (buttons->count(ButtonKey::Play) && (*buttons)[ButtonKey::Play]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::CREATE_JOIN;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;

    } else if (buttons->count(ButtonKey::Chat) && (*buttons)[ButtonKey::Chat]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::CHAT_PRIVATE;
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
        j[jsonKeys::ACTION] = jsonKeys::PLAYER_INFO;
        network.sendData(j.dump() + "\n", client.getClientSocket());

    } else if (buttons->count(ButtonKey::Quit) && (*buttons)[ButtonKey::Quit]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::WELCOME;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
    auto data  = client.getServerData();
    if (data["message"] == jsonKeys::PLAYER_INFO) {
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
    Text title("Rejoignez la communaute", font, 30, sf::Color::White, sf::Vector2f(WINDOW_WIDTH / 2 - 200, 20));
    title.draw(*window);


    // Centrage des champs
    float fieldWidth = 250;
    float fieldHeight = 35;
    float centerX = WINDOW_WIDTH / 2 - fieldWidth / 2;
    float startY = 550;
    float fieldSpacing = 60;

    // Ajouter les champs de texte s'ils n'existent pas
    if (texts->empty()) {
        (*texts)[TextFieldKey::Username] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                    sf::Vector2f(centerX - 320, startY ), sf::Vector2f(fieldWidth, fieldHeight), "Username");
        (*texts)[TextFieldKey::Password] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                    sf::Vector2f(centerX , startY), sf::Vector2f(fieldWidth, fieldHeight), "Password", true);
        (*texts)[TextFieldKey::ConfirmPassword] = std::make_unique<TextField>(font, 24, sf::Color::Black, sf::Color::White,
                                                                    sf::Vector2f(centerX + 320, startY),  sf::Vector2f(fieldWidth, fieldHeight), "Confirm Password", true);
    }

    // Section Sélection d'avatar
    float avatarTitleY = startY + fieldSpacing + 10;
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

    for (std::size_t i = 0; i < avatarPaths.size(); ++i) {
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
                std::cout << "Avatar selectionne : " << avatarManager->getSelectedAvatar() << std::endl;
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
    sfmlGame->drawButtons();

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
            sfmlGame->afficherErreur("Veuillez selectionner un avatar.");
            return;
        }
        client.setAvatarIndex(selectedAvatar);

        // Envoi des données au serveur
        json j = {
            {jsonKeys::ACTION, jsonKeys::REGISTER},
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
    Text header("Classement", font, 30, sf::Color::White, sf::Vector2f(600, 20));
    header.draw(*window);


    if(buttons->empty()){
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(textures->logoExit, sf::Vector2f(10, 20), sf::Vector2f(40, 40));
        (*buttons)[ButtonKey::Settings] = std::make_unique<Button>(textures->logoSettings,sf::Vector2f(WINDOW_WIDTH - 130, 20), sf::Vector2f(35, 35));
        (*buttons)[ButtonKey::Notification] = std::make_unique<Button>(textures->logoNotification, sf::Vector2f(WINDOW_WIDTH - 190, 20), sf::Vector2f(45, 45));
        (*buttons)[ButtonKey::Profile] = std::make_unique<Button>("", font, 24, sf::Color::Transparent, sf::Color::White,
                                    sf::Vector2f(WINDOW_WIDTH - 70, 20), sf::Vector2f(35, 35), sf::Color::Transparent);
    }

    sfmlGame->drawButtons();

    // Affichage de la liste d'amis (exemple visuel)
    float startY = 80;
    float spacing = 90;

    auto ranking = client.getRanking();
        if(ranking.empty() ) {
            std::cout << "Aucun classement trouve." << std::endl;
            return;
        }
        int i = 0;
        for (const auto& [username, details] : ranking) {
            std::string bestScore = details[0];      // Score
            std::string avatarNumber = details[1];   // ID d'avatar
        
            // Position de base pour cette ligne
            float yOffset = startY + i * spacing;
        
            // Avatar
            int avatarId = std::stoi(avatarNumber);
            if (avatarId >= 0 && avatarId < static_cast<int>(avatarManager->getAvatarPath().size())) {
                sf::Texture avatarTexture;
                if (avatarTexture.loadFromFile(avatarManager->getAvatarPath()[avatarId])) {
                    sf::CircleShape avatarCircle(25);
                    avatarCircle.setPosition(190, yOffset + 15);  // Centré à gauche
                    avatarCircle.setTexture(&avatarTexture);
                    avatarCircle.setOutlineThickness(2);
                    avatarCircle.setOutlineColor(sf::Color::White);
                    window->draw(avatarCircle);
                }
            }
        
            // Rectangle fictif pour la carte du joueur
            Rectangle friendCard(sf::Vector2f(150, startY + i * spacing), sf::Vector2f(1000, 80), sf::Color::Transparent, sf::Color::Black);
            friendCard.draw(*window);
            
            //Nom du joueur
            Text name(username, font, 20, sf::Color::Black, sf::Vector2f(280, startY + i * spacing + 25));
            name.draw(*window);
            
            //Score du joueur
            Text score(bestScore, font, 20, sf::Color::Black, sf::Vector2f(1100, startY + i * spacing + 25));
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
    sfmlGame->displayBackground(textures->teams); 

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
        (*buttons)[ButtonKey::Chat] = std::make_unique<Button>("Teams Chat", font, 28, text, background,
            sf::Vector2f(centerX, startY),
            sf::Vector2f(buttonWidth, buttonHeight), outline);
        (*buttons)[ButtonKey::CreateTeam] = std::make_unique<Button>("Create Team", font, 28, text, background,
            sf::Vector2f(centerX, startY + (buttonHeight + spacing)),
            sf::Vector2f(buttonWidth, buttonHeight), outline);

        (*buttons)[ButtonKey::JoinTeam] = std::make_unique<Button>("Join Team", font, 28, text, background,
            sf::Vector2f(centerX, startY + 2*(buttonHeight + spacing)),
            sf::Vector2f(buttonWidth, buttonHeight), outline);

        (*buttons)[ButtonKey::TeamInvites] = std::make_unique<Button>("Teams Invitation", font, 28, text, background,
            sf::Vector2f(centerX, startY + 3 * (buttonHeight + spacing)),
            sf::Vector2f(buttonWidth, buttonHeight), outline);

        (*buttons)[ButtonKey::ManageTeams] = std::make_unique<Button>("Manage My Teams", font, 28, text, background,
            sf::Vector2f(centerX, startY + 4 * (buttonHeight + spacing)),
            sf::Vector2f(buttonWidth, buttonHeight), outline);
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(textures->logoExit, sf::Vector2f(10, 20), sf::Vector2f(40, 40));
        (*buttons)[ButtonKey::Settings] = std::make_unique<Button>(textures->logoSettings,sf::Vector2f(WINDOW_WIDTH - 130, 20), sf::Vector2f(35, 35));
        (*buttons)[ButtonKey::Notification] = std::make_unique<Button>(textures->logoNotification, sf::Vector2f(WINDOW_WIDTH - 190, 20), sf::Vector2f(45, 45));
        (*buttons)[ButtonKey::Profile] = std::make_unique<Button>("", font, 24, sf::Color::Transparent, sf::Color::White,
                                    sf::Vector2f(WINDOW_WIDTH - 70, 20), sf::Vector2f(35, 35), sf::Color::Transparent);
        
    }

    sfmlGame->drawButtons();

    json j;
    if((*buttons)[ButtonKey::Chat]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::CHAT_TEAMS;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;

    } else
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
        j[jsonKeys::ACTION] = jsonKeys::MANAGE_TEAMS_MENU;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }else if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::MAIN;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
}


void MenuManager::friendsMenu() {
    // Affichage de l'arrière-plan
    sfmlGame->displayBackground(textures->friends);

    if (buttons->empty()) {
        // Bouton "Ajouter un ami"
        Button addFriendButton("Ajouter un ami", font, 20, sf::Color::White, sf::Color(100, 200, 250),
                               sf::Vector2f(550, 250), sf::Vector2f(400, 60));
        
        // Bouton "Liste des amis"
        Button listFriendsButton("Liste des amis", font, 20, sf::Color::White, sf::Color(70, 180, 100),
                                 sf::Vector2f(550, 320), sf::Vector2f(400, 60));
        
        // Bouton "Demandes d'amis"
        Button requestsButton("Demandes d'amis", font, 20, sf::Color::White, sf::Color(200, 180, 70),
                              sf::Vector2f(550, 390), sf::Vector2f(400, 60));

        // Bouton retour
        Button backButton("Retour", font, 20, sf::Color::White, sf::Color(180, 70, 70),
                          sf::Vector2f(550, 460), sf::Vector2f(400, 60));

        (*buttons)[ButtonKey::AddFriend] = std::make_unique<Button>(addFriendButton);
        (*buttons)[ButtonKey::FriendList] = std::make_unique<Button>(listFriendsButton);
        (*buttons)[ButtonKey::FriendRequestList] = std::make_unique<Button>(requestsButton);
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(backButton);
    }

    sfmlGame->drawButtons();

    json j;
    // Actions associées aux boutons
    if ((*buttons)[ButtonKey::AddFriend]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::ADD_FRIEND_MENU;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    if ((*buttons)[ButtonKey::FriendList]->isClicked(*window)) {
        // Afficher la liste des amis
        j[jsonKeys::ACTION] = jsonKeys::FRIEND_LIST;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        sfmlGame->cleanup();
        return;
    }

    if ((*buttons)[ButtonKey::FriendRequestList]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::FRIEND_REQUEST_LIST;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        sfmlGame->cleanup();
        return;
    }

    if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::MAIN;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }
}


void MenuManager::friendListMenu() {
    // Fond
    sfmlGame->displayBackground(textures->chat);

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

    for (std::size_t i = 0; i < std::min(amis.size(), avatarTextures.size()); ++i) {
        float y = 100 + i * contactHeight;

        if (!friendButtons.count(amis[i])) {
            friendButtons[amis[i]] = std::make_unique<Button>(
                amis[i], font, 20, sf::Color::White, sf::Color::Transparent,
                sf::Vector2f(0, y), sf::Vector2f(200, contactHeight), sf::Color::Transparent);
        }

        if (avatarTextures[i].loadFromFile("../../res/avatar/avatar" + std::to_string(i + 1) + ".png")) {
            Circle circle(sf::Vector2f(7, y + 8), 20.0f, sf::Color::White, sf::Color::Transparent);
            circle.setTexture(avatarTextures[i]);
            circle.draw(*window);
        }

        friendButtons[amis[i]]->draw(*window);
        //verifier si le button est clique
        if (friendButtons[amis[i]]->isClicked(*window)) {
            selectedFriend = amis[i];
            break;
        }
    }

    // Si un ami est sélectionné
    if (!selectedFriend.empty()) {
        Rectangle(sf::Vector2f(220, 200), sf::Vector2f(300, 150), sf::Color(60, 60, 90), sf::Color::White);
        Text("Voulez-vous supprimer :", font, 20, sf::Color::White, sf::Vector2f(230, 210)).draw(*window);
        Text(selectedFriend, font, 24, sf::Color::Cyan, sf::Vector2f(230, 240)).draw(*window);

        // Boutons supprimer / annuler
        if (!buttons->count(ButtonKey::RemoveFriend)) {
            (*buttons)[ButtonKey::RemoveFriend] = std::make_unique<Button>(
                "Supprimer", font, 18, sf::Color::White, sf::Color(200, 70, 70),
                sf::Vector2f(230, 280), sf::Vector2f(100, 40));
        }
        if (!buttons->count(ButtonKey::Cancel)) {
            (*buttons)[ButtonKey::Cancel] = std::make_unique<Button>(
                "Annuler", font, 18, sf::Color::White, sf::Color(100, 100, 100),
                sf::Vector2f(350, 280), sf::Vector2f(100, 40));
        }

        (*buttons)[ButtonKey::RemoveFriend]->draw(*window);
        (*buttons)[ButtonKey::Cancel]->draw(*window);

        // Action sur clic
        if ((*buttons)[ButtonKey::RemoveFriend]->isClicked(*window)) {
            json j = {
                {jsonKeys::ACTION, jsonKeys::REMOVE_FRIEND},
                {"friend", selectedFriend}
            };
            network.sendData(j.dump() + "\n", client.getClientSocket());
            selectedFriend.clear();
        }

        if ((*buttons)[ButtonKey::Cancel]->isClicked(*window)) {
            selectedFriend.clear();
        }
    }

    // Bouton retour
    if (!buttons->count(ButtonKey::Retour)) {
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(
            textures->logoMain, sf::Vector2f(7, 50), sf::Vector2f(25, 35));
    }
    (*buttons)[ButtonKey::Retour]->draw(*window);

    //barre de recherche
    if(!texts->count(TextFieldKey::SearchField)){
        TextField searchField(font, 20, sf::Color::Black, sf::Color(250, 250, 250),
            sf::Vector2f(40, 50), sf::Vector2f(155, 35), "Recherche");
        (*texts)[TextFieldKey::SearchField] = std::make_unique<TextField>(searchField);
    }
    (*texts)[TextFieldKey::SearchField]->draw(*window);

    if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = jsonKeys::FRIENDS;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        client.setCurrentMenuState(MenuState::Friends);
        selectedFriend.clear();
        return;
    }
}

void MenuManager::addFriendMenu() {
    // Affichage de l'arrière-plan
    sfmlGame->displayBackground(textures->addFriend);


    // Création du champ de texte si pas encore fait
    if (texts->empty()) {
        TextField pseudoField(font, 30, sf::Color::White, sf::Color::Transparent,
                              sf::Vector2f(550, 400), sf::Vector2f(750, 50), "Pseudo");
        (*texts)[TextFieldKey::AddFriendField] = std::make_unique<TextField>(pseudoField);
    }

    // Création des boutons si pas encore fait
    if (buttons->empty()) {
        Button sendButton("", font, 20, sf::Color::Transparent, sf::Color::Transparent,
                          sf::Vector2f(880, 620), sf::Vector2f(300, 60));

        Button backButton("", font, 20, sf::Color::Transparent, sf::Color::Transparent,
                          sf::Vector2f(360, 620), sf::Vector2f(300, 60));

        (*buttons)[ButtonKey::Send] = std::make_unique<Button>(sendButton);
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(backButton);
    }

    // Dessin des éléments
    sfmlGame->drawTextFields();
    sfmlGame->drawButtons();

    json j;
    // Traitement du bouton envoyer
    if ((*buttons)[ButtonKey::Send]->isClicked(*window) && !(*texts)[TextFieldKey::AddFriendField]->getText().empty()) {
        std::string friendName = (*texts)[TextFieldKey::AddFriendField]->getText();
        j[jsonKeys::ACTION] = jsonKeys::ADD_FRIEND;
        j["friend"] = friendName;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        (*texts)[TextFieldKey::AddFriendField]->clear();
        return;
    }

    // Traitement du bouton retour  
    if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::FRIENDS;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        client.setCurrentMenuState(MenuState::Friends);
        return;
    }
}


void MenuManager::friendRequestListMenu() {
    // Fond
    sfmlGame->displayBackground(textures->chat);

    // Barre latérale "Liste d'amis"
    Rectangle sidebar(sf::Vector2f(0, 0), sf::Vector2f(200, WINDOW_HEIGHT), sf::Color(30, 30, 50), sf::Color(80, 80, 100));
    sidebar.draw(*window);

    // Titre
    Text header("Demandes D'amis", font, 21, sf::Color::White, sf::Vector2f(15, 10));
    header.draw(*window);

    auto serverData = client.getServerData();
    // Requête au serveur pour les amis si data reçu
    if (!serverData.empty() && serverData.contains("data") && serverData["message"] == jsonKeys::FRIEND_REQUEST_LIST) {
        amis = client.getServerData()["data"];
    }
    // Affichage des amis
    static std::vector<sf::Texture> avatarTextures(20);
    const float contactHeight = 50.0f;

    for (std::size_t i = 0; i < std::min(amis.size(), avatarTextures.size()); ++i) {
        float y = 100 + i * contactHeight;

        if (!friendButtons.count(amis[i])) {
            friendButtons[amis[i]] = std::make_unique<Button>(
                amis[i], font, 20, sf::Color::White, sf::Color::Transparent,
                sf::Vector2f(0, y), sf::Vector2f(200, contactHeight), sf::Color::Transparent);
        }

        if (avatarTextures[i].loadFromFile("../../res/avatar/avatar" + std::to_string(i + 1) + ".png")) {
            Circle circle(sf::Vector2f(7, y + 8), 20.0f, sf::Color::White, sf::Color::Transparent);
            circle.setTexture(avatarTextures[i]);
            circle.draw(*window);
        }

        friendButtons[amis[i]]->draw(*window);
        //verifier si le bouton est cliqué
        const auto& button = friendButtons[amis[i]];
        if (button->isClicked(*window)) {
            selectedFriend = amis[i];
            break;
        }
    }

    // Si un ami est sélectionné
    if (!selectedFriend.empty()) {
        Rectangle(sf::Vector2f(220, 200), sf::Vector2f(300, 150), sf::Color(60, 60, 90), sf::Color::White);
        Text("Voulez-vous accepter l’ami :", font, 20, sf::Color::White, sf::Vector2f(230, 210)).draw(*window);
        Text(selectedFriend, font, 24, sf::Color::Cyan, sf::Vector2f(230, 240)).draw(*window);

        // Boutons accepter / refuser
        if (!buttons->count(ButtonKey::AcceptFriendRequest)) {
            (*buttons)[ButtonKey::AcceptFriendRequest] = std::make_unique<Button>(
                "Accepter", font, 18, sf::Color::White, sf::Color(70, 200, 70),
                sf::Vector2f(230, 280), sf::Vector2f(100, 40));
        }
        if (!buttons->count(ButtonKey::RefuseFriendRequest)) {
            (*buttons)[ButtonKey::RefuseFriendRequest] = std::make_unique<Button>(
                "Refuser", font, 18, sf::Color::White, sf::Color(200, 70, 70),
                sf::Vector2f(350, 280), sf::Vector2f(100, 40));
        }

        (*buttons)[ButtonKey::AcceptFriendRequest]->draw(*window);
        (*buttons)[ButtonKey::RefuseFriendRequest]->draw(*window);

        // Action sur clic
        if ((*buttons)[ButtonKey::AcceptFriendRequest]->isClicked(*window)) {
            json j = {{jsonKeys::ACTION, jsonKeys::ACCEPT_FRIEND_REQUEST}, {"friend", selectedFriend}};
            network.sendData(j.dump() + "\n", client.getClientSocket());
            selectedFriend.clear();
        }
        if ((*buttons)[ButtonKey::RefuseFriendRequest]->isClicked(*window)) {
            json j = {{jsonKeys::ACTION, jsonKeys::REJECT_FRIEND_REQUEST}, {"friend", selectedFriend}};
            network.sendData(j.dump() + "\n", client.getClientSocket());
            selectedFriend.clear();
        }
    }

    // Bouton retour
    if (!buttons->count(ButtonKey::Retour)) {
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(
            textures->logoMain, sf::Vector2f(7, 50), sf::Vector2f(25, 35));
    }
    (*buttons)[ButtonKey::Retour]->draw(*window);
    //barre de recherche
    if(!texts->count(TextFieldKey::SearchField)){
        TextField searchField(font, 20, sf::Color::Black, sf::Color(250, 250, 250),
            sf::Vector2f(40, 50), sf::Vector2f(155, 35), "Recherche");
        (*texts)[TextFieldKey::SearchField] = std::make_unique<TextField>(searchField);
    }
    (*texts)[TextFieldKey::SearchField]->draw(*window);

    // Action sur clic du bouton retour
    if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = jsonKeys::FRIENDS;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        client.setCurrentMenuState(MenuState::Friends);
        selectedFriend.clear();
        return;
    }
}


void MenuManager::createRoomMenu() {
    // Afficher l'arrière-plan
    sfmlGame->displayBackground(textures->teams);

    // Titre principal
    Text title("Creer une nouvelle Team", font, 40, sf::Color::White, sf::Vector2f(550, 100));
    title.draw(*window);

    // Slogan ou aide en dessous du titre
    Text subtitle("Entrer le nom de votre Team", font, 18, sf::Color::Black, sf::Vector2f(610, 150));
    subtitle.draw(*window);

    // Création du champ de texte si pas encore fait
    if (texts->empty()) {
        TextField teamNameField(font, 20, sf::Color::Black, sf::Color::White,
                               sf::Vector2f(600, 450), sf::Vector2f(350, 40), "Nom de Team");
        (*texts)[TextFieldKey::TeamNameField] = std::make_unique<TextField>(teamNameField);
    }

    // Création des boutons si pas encore fait
    if (buttons->empty()) {
        Button createButton("Creer", font, 20, sf::Color::White, sf::Color(70, 170, 250),
                                           sf::Vector2f(600, 510), sf::Vector2f(170, 45));

        Button backButton("Retour", font, 20, sf::Color::White, sf::Color(180, 70, 70),
                                            sf::Vector2f(780, 510), sf::Vector2f(170, 45));

        (*buttons)[ButtonKey::Create] = std::make_unique<Button>(createButton);
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(backButton);
    }

    // Dessin des éléments
    sfmlGame->drawTextFields();
    sfmlGame->drawButtons();

    json j;
    // Traitement du bouton "Créer"
    if ((*buttons)[ButtonKey::Create]->isClicked(*window) && !(*texts)[TextFieldKey::TeamNameField]->getText().empty()) {
        std::string teamName = (*texts)[TextFieldKey::TeamNameField]->getText();
        j[jsonKeys::ACTION] = jsonKeys::CREATE_TEAM;
        j[jsonKeys::TEAM_NAME] = teamName;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        //afficherErreur("Équipe créée : " + teamName);
        (*texts)[TextFieldKey::TeamNameField]->clear();
        return;
    }

    // Traitement du bouton "Retour"
    if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::TEAMS;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        client.setCurrentMenuState(MenuState::Team);
        return;
    }
}


void MenuManager::joinTeamMenu() {
    // Afficher l'arrière-plan
    sfmlGame->displayBackground(textures->teams);

    // Titre principal
    Text title("Rejoindre Team", font, 40, sf::Color::White, sf::Vector2f(600, 100));
    title.draw(*window);

    Text subtitle("Entrer le nom de la Team a rejoindre", font, 20, sf::Color::Black, sf::Vector2f(575, 150));
    subtitle.draw(*window);

    // Création du champ de texte si pas encore fait
    if (texts->empty()) {
        TextField teamNameField(font, 20, sf::Color::Black, sf::Color::White,
                                sf::Vector2f(600, 450), sf::Vector2f(350, 40), "Nom de Team");
        (*texts)[TextFieldKey::TeamNameField] = std::make_unique<TextField>(teamNameField);
    }

    // Création des boutons si pas encore fait
    if (buttons->empty()) {
        Button joinButton("Rejoindre", font, 20, sf::Color::White, sf::Color(70, 170, 250),
                                          sf::Vector2f(600, 510), sf::Vector2f(170, 45));

        Button backButton("Retour", font, 20, sf::Color::White, sf::Color(180, 70, 70),
                                           sf::Vector2f(780, 510), sf::Vector2f(170, 45));

        (*buttons)[ButtonKey::Join] = std::make_unique<Button>(joinButton);
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(backButton);
    }

    // Dessin des éléments
    sfmlGame->drawTextFields();
    sfmlGame->drawButtons();

    json j;
    // Traitement du bouton "Rejoindre"
    if ((*buttons)[ButtonKey::Join]->isClicked(*window) && !(*texts)[TextFieldKey::TeamNameField]->getText().empty()) {
        std::string teamName = (*texts)[TextFieldKey::TeamNameField]->getText();
        j[jsonKeys::ACTION] = jsonKeys::JOIN_TEAM;
        j[jsonKeys::TEAM_NAME] = teamName;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        sfmlGame->afficherErreur("Demande envoyée pour rejoindre l'équipe : " + teamName);
        (*texts)[TextFieldKey::TeamNameField]->clear();
        return;
    }

    // Traitement du bouton "Retour"
    if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::TEAMS;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        client.setCurrentMenuState(MenuState::Team);
        client.setCurrentMenuState(MenuState::Team);
        return;
    }
}

void MenuManager::chatMenu() {
    // Display the chat background
    sfmlGame->displayBackground(textures->chat);

    // Sidebar for contacts list
    Rectangle sidebar(sf::Vector2f(0, 0), sf::Vector2f(200, WINDOW_HEIGHT), sf::Color(50, 50, 70), sf::Color(100, 100, 120));
    sidebar.draw(*window);

    // Header for contacts list
    Text header("Contacts", font, 24, sf::Color::White, sf::Vector2f(20, 10));
    header.draw(*window);

    if (texts->empty()) {
        // Création des champs de texte
        TextField searchField(font, 20, sf::Color::Black, sf::Color(250, 250, 250),
            sf::Vector2f(40, 50), sf::Vector2f(155, 35), "Recherche");
        // Champ de texte pour envoyer un message
        TextField messageField(font, 20, sf::Color::Black, sf::Color(250, 250, 250),
            sf::Vector2f(205, WINDOW_HEIGHT - 40), sf::Vector2f(WINDOW_WIDTH - 250, 35), "Enter un message");

        // Ajout des champs de texte au vecteur
        (*texts)[TextFieldKey::SearchField] = std::make_unique<TextField>(searchField);
        (*texts)[TextFieldKey::MessageField] = std::make_unique<TextField>(messageField);
    }

    if (buttons->empty()) {
        // Bouton pour revenir au menu principal
        Button backButton(textures->logoMain,sf::Vector2f(7, 50), sf::Vector2f(25, 35));
        // Bouton pour envoyer le message
        Button sendButton(">", font, 20, sf::Color::White, sf::Color(70, 200, 70),
                          sf::Vector2f(WINDOW_WIDTH - 40, WINDOW_HEIGHT - 40), sf::Vector2f(35, 35));
        // Ajout des boutons au vecteur
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(backButton);
        (*buttons)[ButtonKey::Send] = std::make_unique<Button>(sendButton);
    }

    // Dessiner les champs de texte et les boutons
    sfmlGame->drawTextFields();
    sfmlGame->drawButtons();
    
    // Gérer les clics sur les contacts
    sfmlGame->handleContacts();
    //dessiner les contacts
    sfmlGame->drawContacts();

    // Vérifier d'abord le backButton
    if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        if(sfmlGame->getPreviousState() == MenuState::Settings){
            j[jsonKeys::ACTION] = "AcceptRejoindre";
            sfmlGame->resetAcceptInvite();
        }
        else{
            j[jsonKeys::ACTION] = jsonKeys::MAIN;
        }
        network.sendData(j.dump() + "\n", client.getClientSocket());
        client.setCurrentMenuState(sfmlGame->getPreviousState());
        sfmlGame->clearClickedContact();
        return; // Sortir immédiatement après avoir traité le clic
    }

    // Ensuite vérifier le sendButton
    if (!sfmlGame->getClickedContact().empty()&& !(*texts)[TextFieldKey::MessageField]->getText().empty() && (*buttons)[ButtonKey::Send]->isClicked(*window) ) {
        json j = {
            {"message", (*texts)[TextFieldKey::MessageField]->getText()},
            {"receiver", sfmlGame->getClickedContact()},
            {"sender", "You"}

        };
        network.sendData(j.dump() + "\n", client.getClientSocket());
        sfmlGame->addMessage(j);

        (*texts)[TextFieldKey::MessageField]->clear(); // Effacer le champ de texte après l'envoi
        return;
    }
    // recevoir les messages du serveur et les stocker
    sfmlGame->getMessagesFromServer();
    // Afficher les messages
    sfmlGame->drawMessages();
}

void MenuManager::chooseTeamMenu() {
    // Afficher l'arrière-plan
    sfmlGame->displayBackground(textures->team);

    if (texts->empty()) {
        // Champ de texte pour entrer le nom de la team
        TextField teamNameField(font, 20, sf::Color::Black, sf::Color(250, 250, 250),
            sf::Vector2f(360, 220), sf::Vector2f(780, 30), "Recherche");
        (*texts)[TextFieldKey::SearchField] = std::make_unique<TextField>(teamNameField);
    }

    sfmlGame->drawTextFields();

    // Récupérer les équipes depuis le client
    auto teams = client.getTeams(); // Supposons que cette méthode retourne une liste d'équipes
    if(teams.empty()) {
        Text noTeamsText("Aucune équipe disponible", font, 24, sf::Color::White, sf::Vector2f(600, 300));
        noTeamsText.draw(*window);
        return;
    }
    if(buttons->empty()) {
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(
            textures->logoMain, sf::Vector2f(7, 50), sf::Vector2f(25, 35));
        
    }
    const float cardHeight = 60.0f;
    const float cardWidth = 780.0f;
    const float spacing = 20.0f;
    const float startX = 360.0f;
    const float startY = 260.0f;

    // Affichage des équipes sous forme de cartes
    for (std::size_t i = 0; i < std::min<std::size_t>(5, teams.size()); ++i) {
        float y = startY + i * (cardHeight + spacing);

        if (!sfmlGame->getTEAMSbuttons().count(teams[i])) {
            sfmlGame->getTEAMSbuttons()[teams[i]] = std::make_unique<Button>(
                teams[i], font, 24, sf::Color::White, sf::Color(50, 50, 150),
                sf::Vector2f(startX, y), sf::Vector2f(cardWidth, cardHeight), sf::Color::White);
        }

        // Dessiner un rectangle pour représenter la carte
        Rectangle card(sf::Vector2f(startX, y), sf::Vector2f(cardWidth, cardHeight), sf::Color(30, 30, 50), sf::Color::White);
        card.draw(*window);

        // Dessiner le bouton (nom de l'équipe) sur la carte
        sfmlGame->getTEAMSbuttons()[teams[i]]->draw(*window);
    }
    // Dessiner le bouton de retour
    (*buttons)[ButtonKey::Retour]->draw(*window);
    // Gérer les clics sur les cartes
    for (const auto& team : teams) {
        if (sfmlGame->getTEAMSbuttons()[team]->isClicked(*window)) {
            json j;
            j[jsonKeys::ACTION] = jsonKeys::MANAGE_TEAM;
            sfmlGame->setSelectedTeam(team);
            network.sendData(j.dump() + "\n", client.getClientSocket());
            client.setCurrentMenuState(MenuState::ManageTeam);
            return;
        }
    }
    // Gérer le clic sur le bouton de retour
    if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        json j;
        j[jsonKeys::ACTION] = jsonKeys::TEAMS;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        client.setCurrentMenuState(MenuState::Main);
        return;
    }
}

void MenuManager::manageTeamMenu(const std::string& teamName) {
    // Afficher l'arrière-plan
    sfmlGame->displayBackground(textures->manageTeam);

    // Titre principal
    Text title(teamName, font, 30, sf::Color::White, sf::Vector2f(700, 110));
    title.draw(*window);

    // Création des boutons si pas encore fait
    if (buttons->empty()) {
        float buttonWidth = 300;
        float buttonHeight = 50;
        float spacing = 20;
        float centerX = WINDOW_WIDTH / 2.0f - buttonWidth / 2.0f;
        float startY = 250;

        // Bouton "Liste des membres"
        (*buttons)[ButtonKey::ListMembers] = std::make_unique<Button>("Members List", font, 20, sf::Color::White, sf::Color(70, 170, 250),
                                                                   sf::Vector2f(centerX, startY), sf::Vector2f(buttonWidth, buttonHeight));

        // Bouton "Ajouter un membre"
        (*buttons)[ButtonKey::AddMember] = std::make_unique<Button>("Add Member", font, 20, sf::Color::White, sf::Color(70, 170, 250),
                                                                 sf::Vector2f(centerX, startY + (buttonHeight + spacing)), sf::Vector2f(buttonWidth, buttonHeight));

        // Bouton "Ajouter un admin"
        (*buttons)[ButtonKey::AddAdmin] = std::make_unique<Button>("Add Admin", font, 20, sf::Color::White, sf::Color(70, 170, 250),
                                                                sf::Vector2f(centerX, startY + 2 * (buttonHeight + spacing)), sf::Vector2f(buttonWidth, buttonHeight));

        // Bouton "Les demandes de rejoindre"
        (*buttons)[ButtonKey::JoinRequests] = std::make_unique<Button>("Join Request", font, 20, sf::Color::White, sf::Color(70, 170, 250),
                                                                    sf::Vector2f(centerX, startY + 3 * (buttonHeight + spacing)), sf::Vector2f(buttonWidth, buttonHeight));

        // Bouton "Supprimer la room"
        (*buttons)[ButtonKey::DeleteRoom] = std::make_unique<Button>("Delete Team", font, 20, sf::Color::White, sf::Color(200, 70, 70),
                                                                  sf::Vector2f(centerX, startY + 4 * (buttonHeight + spacing)), sf::Vector2f(buttonWidth, buttonHeight));

        // Bouton "Retour"
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>("Back", font, 20, sf::Color::White, sf::Color(180, 70, 70),
                                                              sf::Vector2f(centerX, startY + 5 * (buttonHeight + spacing)), sf::Vector2f(buttonWidth, buttonHeight));
    }

    // Dessiner les boutons
    sfmlGame->drawButtons();

    json j;

    // Gérer les clics sur les boutons
    if ((*buttons)[ButtonKey::ListMembers]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::LIST_MEMBERS;
        j[jsonKeys::TEAM_NAME] = sfmlGame->getSelectedTeam();
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    if ((*buttons)[ButtonKey::AddMember]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::ADD_MEMBER_MENU;
        j[jsonKeys::TEAM_NAME] = sfmlGame->getSelectedTeam();
    
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    if ((*buttons)[ButtonKey::AddAdmin]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::ADD_ADMIN_MENU;
        j[jsonKeys::TEAM_NAME] = sfmlGame->getSelectedTeam();
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    if ((*buttons)[ButtonKey::JoinRequests]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::JOIN_REQUESTS;
        j[jsonKeys::TEAM_NAME] = sfmlGame->getSelectedTeam();
        network.sendData(j.dump() + "\n", client.getClientSocket());
        return;
    }

    if ((*buttons)[ButtonKey::DeleteRoom]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::DELETE_ROOM;
        j[jsonKeys::TEAM_NAME] = sfmlGame->getSelectedTeam();
        network.sendData(j.dump() + "\n", client.getClientSocket());
        sfmlGame->cleanup();
        return;
    }

    if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::TEAMS;
        sfmlGame->clearSelectedTeam();
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


void MenuManager::addMemberMenu(const std::string& teamName) {
        // Afficher l'arrière-plan
        sfmlGame->displayBackground(textures->teams);

        // Titre principal
        Text title("Ajouter un membre à la Team", font, 40, sf::Color::White, sf::Vector2f(550, 100));
        title.draw(*window);
    
        // Slogan ou aide en dessous du titre
        Text subtitle("Entrer le nom du Membre", font, 18, sf::Color::Black, sf::Vector2f(610, 150));
        subtitle.draw(*window);
    
        // Création du champ de texte si pas encore fait
        if (texts->empty()) {
            TextField teamNameField(font, 20, sf::Color::Black, sf::Color::White,
                                   sf::Vector2f(600, 450), sf::Vector2f(350, 40), "Nom de Team");
            (*texts)[TextFieldKey::TeamNameField] = std::make_unique<TextField>(teamNameField);
        }
    
        // Création des boutons si pas encore fait
        if (buttons->empty()) {
            Button createButton("Ajouter", font, 20, sf::Color::White, sf::Color(70, 170, 250),
                                               sf::Vector2f(600, 510), sf::Vector2f(170, 45));
    
            Button backButton("Retour", font, 20, sf::Color::White, sf::Color(180, 70, 70),
                                                sf::Vector2f(780, 510), sf::Vector2f(170, 45));
    
            (*buttons)[ButtonKey::AddMember] = std::make_unique<Button>(createButton);
            (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(backButton);
        }
    
        // Dessin des éléments
        sfmlGame->drawTextFields();
        sfmlGame->drawButtons();
    
        json j;
        // Traitement du bouton "Créer"
        if ((*buttons)[ButtonKey::AddMember]->isClicked(*window) && !(*texts)[TextFieldKey::TeamNameField]->getText().empty()) {
            std::string member = (*texts)[TextFieldKey::TeamNameField]->getText();
            j[jsonKeys::ACTION] = jsonKeys::ADD_MEMBER;
            j[jsonKeys::TEAM_NAME] = teamName;
            j[jsonKeys::MEMBER] = member;
            network.sendData(j.dump() + "\n", client.getClientSocket());
            //afficherErreur("Équipe créée : " + teamName);
            (*texts)[TextFieldKey::TeamNameField]->clear();
            return;
        }
    
        // Traitement du bouton "Retour"
        if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
            j[jsonKeys::ACTION] = jsonKeys::MANAGE_TEAM;
            network.sendData(j.dump() + "\n", client.getClientSocket());
            client.setCurrentMenuState(MenuState::ManageTeam);
            return;
        }
}

void MenuManager::addAdminMenu(const std::string& teamName) {
    // Afficher l'arrière-plan
    sfmlGame->displayBackground(textures->teams);

    // Titre principal
    Text title("Ajouter un admin à la Team", font, 40, sf::Color::White, sf::Vector2f(550, 100));
    title.draw(*window);

    // Slogan ou aide en dessous du titre
    Text subtitle("Entrer le nom de l'admin", font, 18, sf::Color::Black, sf::Vector2f(610, 150));
    subtitle.draw(*window);

    // Création du champ de texte si pas encore fait
    if (texts->empty()) {
        TextField teamNameField(font, 20, sf::Color::Black, sf::Color::White,
                               sf::Vector2f(600, 450), sf::Vector2f(350, 40), "Nom de Team");
        (*texts)[TextFieldKey::TeamNameField] = std::make_unique<TextField>(teamNameField);
    }

    // Création des boutons si pas encore fait
    if (buttons->empty()) {
        Button createButton("Ajouter", font, 20, sf::Color::White, sf::Color(70, 170, 250),
                                           sf::Vector2f(600, 510), sf::Vector2f(170, 45));

        Button backButton("Retour", font, 20, sf::Color::White, sf::Color(180, 70, 70),
                                            sf::Vector2f(780, 510), sf::Vector2f(170, 45));

        (*buttons)[ButtonKey::AddAdmin] = std::make_unique<Button>(createButton);
        (*buttons)[ButtonKey::Retour] = std::make_unique<Button>(backButton);
    }

    // Dessin des éléments
    sfmlGame->drawTextFields();
    sfmlGame->drawButtons();

    json j;
    // Traitement du bouton "Créer"
    if ((*buttons)[ButtonKey::AddAdmin]->isClicked(*window) && !(*texts)[TextFieldKey::TeamNameField]->getText().empty()) {
        std::string admin = (*texts)[TextFieldKey::TeamNameField]->getText();
        j[jsonKeys::ACTION] = jsonKeys::ADD_ADMIN;
        j[jsonKeys::TEAM_NAME] = teamName;
        j[jsonKeys::ADMIN] = admin;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        (*texts)[TextFieldKey::TeamNameField]->clear();
        return;
    }
    // Traitement du bouton "Retour"
    if ((*buttons)[ButtonKey::Retour]->isClicked(*window)) {
        j[jsonKeys::ACTION] = jsonKeys::MANAGE_TEAM;
        network.sendData(j.dump() + "\n", client.getClientSocket());
        client.setCurrentMenuState(MenuState::ManageTeam);
        return;
    }
}
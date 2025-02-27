#include "Server.hpp"

#include "../common/json.hpp"
#include <ncurses.h>
#include <unistd.h>
#include <csignal>
#include <thread>
#include <netinet/in.h>
#include <fstream>
#include <iostream>


Server::Server(int port, Game* game) 
    : port(port), serverSocket(-1), game(game), clientIdCounter(0)
{ userManager = std::make_unique<UserManager>("./users.txt"); }


bool Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Erreur: Impossible de créer le socket du serveur." << std::endl;
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Erreur: Impossible de binder le socket." << std::endl;
        close(serverSocket);
        return false;
    }

    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Erreur: Impossible d'écouter sur le port." << std::endl;
        close(serverSocket);
        return false;
    }

    std::cout << "Serveur en attente de connexions sur le port " << port << std::endl;
    return true;
}

void Server::acceptClients() {
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientSocket < 0) {
        std::cerr << "Erreur: Échec de l'acceptation du client." << std::endl;
        return;
    }

    int clientId = clientIdCounter.fetch_add(1);  // Attribuer un ID unique et incrémenter le compteur
    std::cout << "Client #" << clientId << " connecté." << std::endl;
    
    clientStates[clientId] = MenuState::Welcome;

    clear();
    sendMenuToClient(clientSocket, game->getMainMenu0()); 
    refresh();

    // Lancer un thread pour gérer ce client
    std::thread clientThread(&Server::handleClient, this, clientSocket, clientId);
    clientThread.detach();
}

void Server::handleClient(int clientSocket, int clientId) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Client #" << clientId << " déconnecté." << std::endl;
            close(clientSocket);
            break;
        }

        try {
            json receivedData = json::parse(buffer);
            
            if (!receivedData.contains("action") || !receivedData["action"].is_string()) {
                std::cerr << "Erreur: 'action' manquant ou invalide dans le JSON reçu." << std::endl;
                return;
            }
        
            std::string action = receivedData["action"];
        
            handleMenu(clientSocket, clientId, action);
             
            // Si le joueur est en jeu, lancer un thread pour recevoir les inputs
            if (runningGame) {
                receiveInputFromClient(clientSocket, clientId);
            }
        
        } catch (json::parse_error& e) {
            std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
        }
        
    }
}


void Server::handleMenu(int clientSocket, int clientId, const std::string& action) {
    auto state = clientStates[clientId];
    switch (state) {
        case MenuState::Welcome:
            keyInputWelcomeMenu(clientSocket, clientId, action);
            break;
        case MenuState::RegisterPseudo:
            keyInputRegisterPseudoMenu(clientSocket, clientId, action);
            break;
        case MenuState::RegisterPassword:
            keyInputRegisterPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::LoginPseudo:
            keyInputLoginPseudoMenu(clientSocket, clientId, action);
            break;
        case MenuState::LoginPassword:
            keyInputLoginPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::Main:
            keyInputMainMenu(clientSocket, clientId, action);
            break;
        case MenuState::classement:
            // Classement
            break;
        case MenuState::chat:
            keyInputChatMenu(clientSocket, clientId, action);
            break;
        case MenuState::JoinOrCreateGame:
            keyInputJoinOrCreateGameMenu(clientSocket, clientId, action);
            break;
        case MenuState::GameMode:
            keyInputModeGameMenu(clientSocket, clientId, action);
            break;
        case MenuState::Game:
            keyInputGameMenu(clientSocket, clientId, action);
            break;
    }
}


void Server::keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {  // Se connecter
        clientStates[clientId] = MenuState::LoginPseudo;
        sendMenuToClient(clientSocket, game->getLoginMenu1());
    }
    else if (action == "2") { // Créer un compte
        clientStates[clientId] = MenuState::RegisterPseudo; // Passage à l'état Register
        sendMenuToClient(clientSocket, game->getRegisterMenu1());
    }
    else if (action == "3") {
        // Quitter
        close(clientSocket);
    }
}

void Server::keyInputRegisterPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    if (userManager->userNotExists(action)) { 
        // Si le pseudo n'existe pas, on stock en tmp
        clientPseudo[clientId] = action;
        pseudoTosocket[action] = clientSocket;
        clientStates[clientId] = MenuState::RegisterPassword;
        sendMenuToClient(clientSocket, game->getRegisterMenu2());
    } 
    else {
        sendMenuToClient(clientSocket, game->getRegisterMenuFailed());
    }
}

void Server::keyInputRegisterPseudoMenuFailed(int clientSocket, int clientId, const std::string& action) {
    if (userManager->userNotExists(action)) { 
        // Si le pseudo n'existe pas, on stock en tmp
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::RegisterPassword;
        sendMenuToClient(clientSocket, game->getRegisterMenu2());
    } 
    else {
        sendMenuToClient(clientSocket, game->getRegisterMenuFailed());
    }
}

void Server::keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    userManager->registerUser(clientPseudo[clientId], action);
    clientPseudo.erase(clientId);
    clientStates[clientId] = MenuState::Main;
    sendMenuToClient(clientSocket, game->getMainMenu1());
}

void Server::keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    if (!userManager->userNotExists(action)) { // Si le pseudo existe
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::LoginPassword;
        sendMenuToClient(clientSocket, game->getLoginMenu2());
    } 
    else {
        // si pseudo n'existe pas on annule et on retourne à l'étape 1 (dc dmd de pseudo)
        sendMenuToClient(clientSocket, game->getLoginMenuFailed1());
    }
}

void Server::keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    if (userManager->authenticateUser(clientPseudo[clientId], action)) { // Si le mot de passe est correct
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
    } 
    else {
        // Si le mot de passe est incorrect, on retourne à l'étape 2 (dmd de mdp)
        sendMenuToClient(clientSocket, game->getLoginMenuFailed2());
    }
}


void Server::keyInputMainMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        clientStates[clientId] = MenuState::JoinOrCreateGame;
        sendMenuToClient(clientSocket, game->getJoinOrCreateGame());
    }
    if (action == "2") {
        // Amis => à implémenter 
    }

    else if (action == "3") {
        // classement => à implémenter 
        
    }

    else if (action == "4") {
        // Chat
        clientStates[clientId] = MenuState::chat;
        sendMenuToClient(clientSocket, game->getChatMenu());

    }
    
    else if (action == "5") { // Retour à l'écran précédent
        clientStates[clientId] = MenuState::Welcome;
        sendMenuToClient(clientSocket, game->getMainMenu0());
    }
}

void Server::keyInputJoinOrCreateGameMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        // Créer une partie
        clientStates[clientId] = MenuState::GameMode;
        sendMenuToClient(clientSocket, game->getGameMode());
    }
    else if (action == "1") {
        // Rejoindre une partie
    }
}

void Server::keyInputChatMenu(int clientSocket, int clientId, const std::string& action) {
    if(action == "1") {
        // a implémenter
    }else if(action == "2") {
        // a implémenter
    }else if(action == "3") {
        // a implémenter
    }else if(action == "4") {
        sendChatModeToClient(clientSocket);
        // Lancer un thread pour gérer le chat du client
        std::thread chatThread(&ServerChat::processClientChat, chat.get(), clientSocket, std::ref(pseudoTosocket));
        chatThread.detach();
    }else if(action == "5") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
    }
}

void Server::keyInputModeGameMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        // endless
    }
    else if (action == "2") {
        // classic
    }
    else if (action == "3") {
        // duel
    } 
    else if (action == "4") {
        // royal competition
    }
    receiveInputFromClient(clientSocket, clientId); // lance un thread pour recevoir les inputs
    clientStates[clientId] = MenuState::Game;
    runningGame = true;
    games[clientId] = std::make_unique<Game>(10, 20);
    loopGame(clientSocket, clientId);
}



void Server::keyInputGameMenu(int clientSocket, int clientId,const std::string& unicodeAction) {
    std::string action = convertUnicodeToText(unicodeAction);  // Convertir \u0005 en "right"

    auto& game = games[clientId];

    if (action == "right") { 
        game->moveCurrentPieceRight();
        game->setNeedToSendGame(true);
    }
    else if (action == "left") { 
        game->moveCurrentPieceLeft();
        game->setNeedToSendGame(true);
    }
    else if (action == "up") { 
        game->rotateCurrentPiece();
        game->setNeedToSendGame(true);
    }
    else if (action == "down"){
        game->moveCurrentPieceDown();
        game->setNeedToSendGame(true);
    }
    else if(action == "drop") { // space
        game->dropCurrentPiece();
        game->setNeedToSendGame(true);
    }
    if (game->getNeedToSendGame()){ 
        sendGameToClient(clientSocket, clientId);
        game->setNeedToSendGame(false);
    }
}

std::string Server::convertUnicodeToText(const std::string& unicode) {
    auto action = unicodeToText.find(unicode);
    return (action != unicodeToText.end()) ? action->second : "///"; 
}

void Server::stop() {
    if (serverSocket != -1) {
        shutdown(serverSocket, SHUT_RDWR);
        close(serverSocket);
        serverSocket = -1;
    }
}

void Server::sendMenuToClient(int clientSocket, const std::string& screen) {
    send(clientSocket, screen.c_str(), screen.size(), 0);
}

void Server::sendGameToClient(int clientSocket, int clientId) { //TODO: Deplacer le main pour faire un fichier game ??? 
    auto& game = games[clientId];

    json message;
    
    message["score"] = game->getScore().scoreToJson();
    message["grid"] = game->getGrid().gridToJson();
    message["tetraPiece"] = game->getCurrentPiece().tetraminoToJson(); // Ajout du tétrimino dans le même message

    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0); // Un seul envoi
}

//recuperer les inputs du client
void Server::receiveInputFromClient(int clientSocket, int clientId) {
    std::thread inputThread([this, clientSocket, clientId]() {
        char buffer[1024];

        while (runningGame) {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); 
            if (bytesReceived > 0) {
                try {
                    json receivedData = json::parse(buffer);
                    std::string action = receivedData["action"];

                    std::cout << "Action reçue du client " << clientId << " : " << action << std::endl;
                    keyInputGameMenu(clientSocket, clientId, action);
                } 
                catch (json::parse_error& e) {
                    std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
                }
            }
        }
    });

    inputThread.detach(); // Permet d’exécuter en parallèle
}

void Server::loopGame(int clientSocket, int clientId) {
    auto& game = games[clientId]; // Récupérer la partie du client

    std::thread gameThread([this, clientId]() { // Lancer un thread pour mettre à jour le jeu
        auto& gameInstance = games[clientId];
        while (runningGame) {
            gameInstance->update(); 
        }
    });

    gameThread.detach();

    while (runningGame) { // Envoi du jeu au client 
        if (game->getNeedToSendGame()) { 
            sendGameToClient(clientSocket, clientId);
            game->setNeedToSendGame(false);
        }
    }
}


void Server::sendChatModeToClient(int clientSocket) {
    json message;
    message["mode"] = "chat";
    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0);
}

int main() {
    // le client ne doit pas l'igniorer faudra sans doute faire un handler pour le SIGPIPE ? 
    signal(SIGPIPE, SIG_IGN);  // le client arrivait à crasher le serveur en fermant la connexion
    
    try {
        std::ofstream serverLog("server.log"); // Créer un fichier de log
        // Rediriger std::cout et std::cerr vers le fichier log
        std::cout.rdbuf(serverLog.rdbuf());
        std::cerr.rdbuf(serverLog.rdbuf());

        Game game(10, 20);
        Server server(12345, &game);
        if (!server.start()) {
            std::cerr << "Erreur: Impossible de démarrer le serveur." << std::endl;
            return 1;
        }

        while (true) {
            server.acceptClients();
        } 

        server.stop();
    } 
    catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }
    return 0;
}


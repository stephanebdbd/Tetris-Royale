#include "Server.hpp"
#include "../common/json.hpp"
#include <ncurses.h>
#include <unistd.h>
#include <csignal>
#include <netinet/in.h>
#include <fstream>
#include <iostream>

Server::Server(int port) : port(port), serverSocket(-1), clientIdCounter(0) {
    userManager = std::make_unique<UserManager>("./users.txt");
}

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
    sendMenuToClient(clientSocket, menu.getMainMenu0()); 
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
            if (clientStates[clientId] == MenuState::Play) {
                int roomId = clientGameRoomId[clientId];
                if ((roomId != -1) && (gameRooms[roomId].get() != nullptr)) {
                    gameRooms[roomId]->setGameIsOver(clientId);
                    if (!gameRooms[roomId]->getInProgress())
                        deleteGameRoom(roomId);
                }
            }
            clientStates.erase(clientId); // Supprimer l'état des menus du client
            close(clientSocket); // Fermer la connexion
            break;
        }

        try {
            json receivedData = json::parse(buffer);

            if (!receivedData.contains("action") || !receivedData["action"].is_string()) {
                std::cerr << "Erreur: 'action' manquant ou invalide dans le JSON reçu." << std::endl;
                return;
            }

            std::string action = receivedData["action"];
            handleMenu(clientSocket, clientId, action); // Gérer l'action du client
        } catch (json::parse_error& e) {
            std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
        }
    }
}

void Server::handleMenu(int clientSocket, int clientId, const std::string& action) {
    if (clientStates[clientId] == MenuState::Welcome) {
        keyInputWelcomeMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::RegisterPseudo) {
        keyInputRegisterPseudoMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::RegisterPassword) {
        keyInputRegisterPasswordMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::LoginPseudo) {
        keyInputLoginPseudoMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::LoginPassword) {
        keyInputLoginPasswordMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::Main) {
        keyInputMainMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::classement) {
        keyInputRankingMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::chat) {
        //keyInputChatMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::Game)
        clientStates[clientId] = MenuState::JoinOrCreateGame;
    if (clientStates[clientId] == MenuState::JoinOrCreateGame)
        keyInputJoinOrCreateGameMenu(clientSocket, clientId, action);
    if (clientStates[clientId] == MenuState::JoinGame){
        keyInputGameModeMenu(clientSocket, clientId);
        return;
    }
    if (clientStates[clientId] == MenuState::CreateGame){
        keyInputGameModeMenu(clientSocket, clientId);
        return;
    }
    if (clientStates[clientId] == MenuState::GameOver) {
        keyInputGameOverMenu(clientSocket, clientId, action);
        return;
    }
}

void Server::keyInputGameModeMenu(int clientSocket, int clientId, GameModeName gameMode) {
    //création de la gameRoom (partie Endless pour l'instant)
    clientStates[clientId] = MenuState::Play;
    clientGameRoomId[clientId] = gameRoomIdCounter;
    std::shared_ptr<GameRoom> gameRoom = std::make_shared<GameRoom>(gameRoomIdCounter, clientId, gameMode);
    gameRooms.push_back(gameRoom);
    
    gameRoomIdCounter++;
    
    
    std::thread loopgame(&Server::loopGame, this, clientSocket, clientId, gameRoom);
    std::thread inputThread(&Server::receiveInputFromClient, this, clientSocket, clientId, gameRoom);
    
    loopgame.join();
    inputThread.join();

    deleteGameRoom(gameRoom->getRoomId());
    
    clientStates[clientId] = MenuState::GameOver;
    sendMenuToClient(clientSocket, menu.getGameOverMenu());
}

void Server::deleteGameRoom(int roomId) {
    gameRooms.erase(gameRooms.begin() + roomId);
    for (int idx=0; idx < clientIdCounter; idx++) {
        if (clientGameRoomId[idx] == roomId) {
            clientGameRoomId[idx] = -1;
        }
    }
    std::cout << "GameRoom #" << roomId << " deleted." << std::endl;
}

void Server::sendInputToGameRoom(int clientId, const std::string& action, std::shared_ptr<GameRoom> gameRoom) {
    gameRoom->input(clientId, action);
}

//recuperer les inputs du client
void Server::receiveInputFromClient(int clientSocket, int clientId, std::shared_ptr<GameRoom> gameRoom) {
    char buffer[1024];
    while ((clientStates[clientId] != MenuState::Play) || (!gameRoom->getHasStarted())){
        continue;
    }
    std::cout << "reception des entrées du client " << clientId << std::endl;
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); 
        if (bytesReceived > 0) {
            try {
                json receivedData = json::parse(buffer);
                std::string action = receivedData["action"];
                
                std::cout << "Action reçue du client " << clientId << " : " << action << std::endl;
                if ((clientStates[clientId] == MenuState::Play) &&
                (gameRoom.get() != nullptr) && (!gameRoom->getGameIsOver(clientId))
                && gameRoom->getInProgress())
                    sendInputToGameRoom(clientId, action, gameRoom);
                else 
                    break;
            }
            catch (json::parse_error& e) {
                std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
            }
        }
    }
    std::cout << "fini les entrées" << std::endl;
}

void Server::loopGame(int clientSocket, int clientId, std::shared_ptr<GameRoom> gameRoom) {
    int gameRoomId = gameRoom->getRoomId();
    std::thread gameRoomThread(&GameRoom::startGame, gameRoom);
    while (!gameRoom->getHasStarted())
        continue;
    std::shared_ptr<Score> score = (gameRoom->getGameModeName() == GameModeName::Endless) ? gameRoom->getScore(clientId) : nullptr;
    std::shared_ptr<Game> game = gameRoom->getGame(clientId);
    std::cout << "Game #" << gameRoomId << " started." << std::endl;
    while (gameRoom->getInProgress()) { 
        if ((gameRoom->getGameIsOver(clientId))){
            if (gameRoom->getGameModeName() == GameModeName::Endless)
                break;
            else if ((gameRoom->getAmountOfPlayers() < 2))
                break;
        }
        if (gameRoom->getNeedToSendGame(clientId)) { 
            sendGameToPlayer(clientSocket, game);
            gameRoom->setNeedToSendGame(false, clientId);
        }
    }
    gameRoomThread.join();
    if (score != nullptr)
        userManager->updateHighscore(clientPseudo[clientId], score->getScore());
    std::cout << "Game #" << gameRoomId << " ended." << std::endl;
}



void Server::keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {  // Se connecter
        clientStates[clientId] = MenuState::LoginPseudo;
        sendMenuToClient(clientSocket, menu.getLoginMenu1());
    }
    else if (action == "2") { // Créer un compte
        clientStates[clientId] = MenuState::RegisterPseudo; // Passage à l'état Register
        sendMenuToClient(clientSocket, menu.getRegisterMenu1());
    }
    else if (action == "3") {
        // Quitter
        close(clientSocket);
    }
    else {
        sendMenuToClient(clientSocket, menu.getMainMenu0());
    }
}

void Server::keyInputRegisterPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    if (userManager->userNotExists(action)) { 
        // Si le pseudo n'existe pas, on stock en tmp
        clientPseudo[clientId] = action;
        pseudoTosocket[action] = clientSocket;
        clientStates[clientId] = MenuState::RegisterPassword;
        sendMenuToClient(clientSocket, menu.getRegisterMenu2());
    } 
    else {
        sendMenuToClient(clientSocket, menu.getRegisterMenu1());
    }
}

void Server::keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    userManager->registerUser(clientPseudo[clientId], action);
    clientPseudo.erase(clientId);
    clientStates[clientId] = MenuState::Main;
    sendMenuToClient(clientSocket, menu.getMainMenu1());
}

void Server::keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    std::cout << "Pseudo: " << action << std::endl;
    if (!userManager->userNotExists(action)) { // Si le pseudo existe
        std::cout << "Pseudo existe" << std::endl;
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::LoginPassword;
        sendMenuToClient(clientSocket, menu.getLoginMenu2());
    } 
    else {
        std::cout << "Pseudo n'existe pas" << std::endl;
        // si pseudo n'existe pas on annule et on retourne à l'étape 1 (dc dmd de pseudo)
        clientStates[clientId] = MenuState::RegisterPseudo;
        sendMenuToClient(clientSocket, menu.getLoginMenuFailed1());
    }
}

void Server::keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    if (userManager->authenticateUser(clientPseudo[clientId], action)) { // Si le mot de passe est correct
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    } 
    else {
        // Si le mot de passe est incorrect, on retourne à l'étape 2 (dmd de mdp)
        sendMenuToClient(clientSocket, menu.getLoginMenuFailed2());
    }
}

void Server::keyInputMainMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") { // créer une gameRoom (choisir s'il veut créer sa partie ou rejoindre une partie)
        clientStates[clientId] = MenuState::Game;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }
    else if (action == "2") {
        // Amis => à implémenter 
    }
    else if (action == "3") {
        // Classement
        clientStates[clientId] = MenuState::classement;
        // TODO: en gros c'est brouillon pour le moment parce que faudrait pas faire passser menu par
        // la game mais avoir une instance de menu dans le serveur comme ici
        sendMenuToClient(clientSocket, menu.getRankingMenu(userManager->getRanking()));   
    }
    else if (action == "4") {
        // Chat
        clientStates[clientId] = MenuState::chat;
        sendMenuToClient(clientSocket, menu.getchatMenu());
        /*sendChatModeToClient(clientSocket);
        // Lancer un thread pour gérer le chat du client
        std::thread chatThread(&ServerChat::processClientChat, chat.get(), clientSocket, std::ref(pseudoTosocket)); // initialiser la variable chat
        chatThread.detach();
        */
    }
    
    else if (action == "5") {
        // Retour à l'écran précédent
        clientStates[clientId] = MenuState::Welcome;
        sendMenuToClient(clientSocket, menu.getMainMenu0());
    }
    else {
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    }
}

void Server::keyInputJoinOrCreateGameMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        // Créer une partie => lobby
        clientStates[clientId] = MenuState::CreateGame;
        // Raccourci vers une game Endless car on doit implémenter le reste
        }
    else if (action == "2") {
        clientStates[clientId] = MenuState::JoinGame;
        // Rejoindre une partie
    }
    else if (action == "3") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    }
    else {
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }
}

/*
void Server::keyInputChatMenu(int clientSocket, int clientId, const std::string& action) {
    if(action == "1") {
        // a implémenter
    }
    else if(action == "2") {
        // a implémenter
    }
    else if(action == "3") {
        // a implémenter
    }
    else if(action == "4") {
        sendChatModeToClient(clientSocket);
        // Lancer un thread pour gérer le chat du client
        std::thread chatThread(&ServerChat::processClientChat, chat.get(), clientSocket, std::ref(pseudoTosocket));
        chatThread.detach();
    }
    else if(action == "5") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    }
}
*/


void Server::keyInputRankingMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") { // TODO: faudra qu'on le voit dans le menu
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    }
    else {
        sendMenuToClient(clientSocket, menu.getRankingMenu(userManager->getRanking())); 
    }
}

void Server::keyInputGameOverMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        clientStates[clientId] = MenuState::Game;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }
    else if (action == "2") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    }
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

void Server::sendGameToPlayer(int clientSocket, std::shared_ptr<Game> game) {
    json message;
    
    message["score"] = game->getScore()->scoreToJson();
    message["grid"] = game->getGrid()->gridToJson();
    message["tetraPiece"] = game->getCurrentPiece().tetraminoToJson(); // Ajout du tétrimino dans le même message

    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0); // Un seul envoi
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


        Server server(12345);
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
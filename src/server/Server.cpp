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
            std::string action = receivedData["action"];
            std::string pseudo;

            // verifier la taille de action car si plus de 1 c'est pseudo ou mdp
            if (action.size() > 1 && currentMenu[clientId] == 2) { // c'est la verif du pseudo
                std::cout << "Pseudo " << clientId << " : " << action << std::endl;    
                if (userManager->userExists(action)){
                    pseudo = action;
                    handleMenu(clientSocket, clientId, action);
                }
            }
            if (action.size() > 1 && currentMenu[clientId] == 1) { 
                std::cout << "mdp " << clientId << " : " << action << std::endl;
                userManager->registerUser(pseudo, action);
                handleMenu(clientSocket, clientId, action);
            }

            else {
                handleMenu(clientSocket, clientId, action);
            }
        

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

    if (currentMenu[clientId] == 0) { // Menu de base => se co, créer compte, quitter 
        sendMenuToClient(clientSocket, game->getMainMenu0());
        keyInuptWelcomeMenu(clientSocket, clientId, action);
    }
    else if (currentMenu[clientId] == 1) { // Second menu => jouer, classement, amis,... 
        sendMenuToClient(clientSocket, game->getMainMenu1());
        keyInuptMainMenu(clientSocket, clientId, action);
    }
    else if (currentMenu[clientId] == 2){ // créer un compte => On dmd le mdp
        sendMenuToClient(clientSocket, game->getRegisterMenu2());
        currentMenu[clientId]--;
    }
    else if (runningGame) {
        std::cout << "Client #" << clientId << " est en jeu." << std::endl;
        keyInuptGameMenu(clientSocket, clientId, action);
    }
}

void Server::keyInuptWelcomeMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {  
        // se connectet => à implémenter
    }
    else if (action == "2") { // créer un compte => On dmd le pseudo
        sendMenuToClient(clientSocket, game->getRegisterMenu1());
        currentMenu[clientId]+=2;
    }
    else if (action == "3") {
        close(clientSocket);
        // Quitter => à implémenter
    }
}

void Server::keyInuptMainMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        runningGame = true;
        receiveInputFromClient(clientSocket, clientId); // lancer un thread pour recevoir les inputs
        games[clientId] = std::make_unique<Game>(10, 20); // Créer une nouvelle instance de Game pour chaque client
        loopGame(clientSocket, clientId);
    }
    else if (action == "2") {
        // Amis => à implémenter
    }
    else if (action == "3") {
        // Classements => à implémenter
    }
    else if (action == "4") {
        // Chat => à implémenter
    }
    if (action == "5") {
        sendMenuToClient(clientSocket, game->getMainMenu0());
        currentMenu[clientId]--;
    }
}

void Server::keyInuptGameMenu(int clientSocket, int clientId,const std::string& unicodeAction) {
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
                    keyInuptGameMenu(clientSocket, clientId, action);
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


void Server::handleRegisterMenu(int clientSocket, int clientId, const json& data) {
    // Récupérer les informations envoyées par le client
    std::string username = data["username"];
    std::string password = data["password"];

    // Utilisation directe de l'instance UserManager dans Server
    bool success = userManager->registerUser(username, password);

    nlohmann::json response;
    if (success) {
        response["title"] = "Inscription réussie";
        response["message"] = "Votre compte a été créé avec succès !";
        response["input"] = "Appuyez sur entrée pour retourner au menu.";
        currentMenu[clientId]--;
        sendMenuToClient(clientSocket, response.dump() + "\n");

        // Retour au menu précédent (par exemple, menu de connexion)
        sendMenuToClient(clientSocket, game->getMainMenu0());
    }
    else {
        response["title"] = "Erreur";
        response["message"] = "Le nom d'utilisateur existe déjà. Veuillez réessayer.";
        response["input"] = "Appuyez sur entrée pour retourner au menu.";
        sendMenuToClient(clientSocket, response.dump() + "\n");

        // Renvoyer le menu d'inscription pour réessayer
        sendMenuToClient(clientSocket, game->getRegisterMenu1());
    }
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



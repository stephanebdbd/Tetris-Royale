#include "Server.hpp"
#include "../common/json.hpp"
#include <ncurses.h>
#include <unistd.h>
#include <csignal>
#include <netinet/in.h>
#include <fstream>
#include <iostream>


Server::Server(int port) 
    : port(port), serverSocket(-1), clientIdCounter(0)
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
    sendMenuToClient(clientSocket, menus[clientId].getMainMenu0()); 
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
        
            handleMenu(clientSocket, clientId, action);
             
            // Si le joueur est en jeu, lancer un thread pour recevoir les inputs
            if (runningGame) {// changer pour qu'il puisse recevoir les inputs du joueur n'importe quand
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
            std::cout << "LoginPassword" << std::endl;
            keyInputLoginPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::Main:
            keyInputMainMenu(clientSocket, clientId, action);
            break;
        case MenuState::Game:
            // Rejoindre ou créer une partie
            break;
        case MenuState::classement:
            // Classement
            break;
        case MenuState::chat:
            // Chat
            break;
    }
}

void Server::keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {  // Se connecter
        clientStates[clientId] = MenuState::LoginPseudo;
        sendMenuToClient(clientSocket, menus[clientId].getLoginMenu1());
    }
    else if (action == "2") { // Créer un compte
        clientStates[clientId] = MenuState::RegisterPseudo; // Passage à l'état Register
        sendMenuToClient(clientSocket, menus[clientId].getRegisterMenu1());
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
        clientStates[clientId] = MenuState::RegisterPassword;
        sendMenuToClient(clientSocket, menus[clientId].getRegisterMenu2());
    } 
    else {
        sendMenuToClient(clientSocket, menus[clientId].getRegisterMenu1());
    }
}

void Server::keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    userManager->registerUser(clientPseudo[clientId], action);
    clientPseudo.erase(clientId);
    clientStates[clientId] = MenuState::Main;
    sendMenuToClient(clientSocket, menus[clientId].getMainMenu1());
}

void Server::keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    std::cout << "Pseudo: " << action << std::endl;
    if (!userManager->userNotExists(action)) { // Si le pseudo existe
        std::cout << "Pseudo existe" << std::endl;
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::LoginPassword;
        sendMenuToClient(clientSocket, menus[clientId].getLoginMenu2());
    } 
    else {
        std::cout << "Pseudo n'existe pas" << std::endl;
        // si pseudo n'existe pas on annule et on retourne à l'étape 1 (dc dmd de pseudo)
        sendMenuToClient(clientSocket, menus[clientId].getLoginMenu1());
    }
}

void Server::keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    if (userManager->authenticateUser(clientPseudo[clientId], action)) { // Si le mot de passe est correct
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, menus[clientId].getMainMenu1());
    } 
    else {
        // Si le mot de passe est incorrect, on retourne à l'étape 2 (dmd de mdp)
        sendMenuToClient(clientSocket, menus[clientId].getLoginMenu2());
    }
}

void Server::keyInputMainMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") { // créer une gameRoom (choisir s'il veut créer sa partie ou rejoindre une partie)
        receiveInputFromClient(clientSocket, clientId); // lance un thread pour recevoir les inputs
        ////// redirection vers la création de la gameRoom ou la liste des gameRooms
        /*clientStates[clientId] = MenuState::Game;
        runningGame = true;
        games[clientId] = std::make_unique<Game>(10, 20);
        loopGame(clientSocket, clientId);
        */
    }
    if (action == "2") {
        // Amis => à implémenter 
    }
    else if (action == "3") {
        // classement => à implémenter 
        
    }
    else if (action == "4") {
        // chat => à implémenter 
    }
    
    else if (action == "5") {
        // Retour à l'écran précédent
        clientStates[clientId] = MenuState::Welcome;
        sendMenuToClient(clientSocket, menus[clientId].getMainMenu0());
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

//recuperer les inputs du client
void Server::receiveInputFromClient(int clientSocket, int clientId) {
    std::thread inputThread([this, clientSocket, clientId]() {
        char buffer[1024];

        while (runningGame) { //doit être un booléen de sa GameRoom
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); 
            if (bytesReceived > 0) {
                try {
                    json receivedData = json::parse(buffer);
                    std::string action = receivedData["action"];

                    std::cout << "Action reçue du client " << clientId << " : " << action << std::endl;
                    //keyInputGameMenu(clientSocket, clientId, action);  // à transférer vers GameRoom
                } 
                catch (json::parse_error& e) {
                    std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
                }
            }
        }
    });

    inputThread.detach(); // Permet d’exécuter en parallèle
}


int main() {
    // le client ne doit pas l'igniorer faudra sans doute faire un handler pour le SIGPIPE ? 
    signal(SIGPIPE, SIG_IGN);  // le client arrivait à crasher le serveur en fermant la connexion
    
    try {
        std::ofstream serverLog("server.log"); // Créer un fichier de log
        // Rediriger std::cout et std::cerr vers le fichier log
        std::cout.rdbuf(serverLog.rdbuf());
        std::cerr.rdbuf(serverLog.rdbuf());

        //Game game(10, 20);
        Server server(12345/*, &game*/);
        //Game game(10, 20);
        Server server(12345/*, &game*/);
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
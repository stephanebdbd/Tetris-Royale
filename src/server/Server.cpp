#include "Server.hpp"

#include "../common/json.hpp"
#include <ncurses.h>
#include <unistd.h>
#include <csignal>
#include <thread>
#include <netinet/in.h>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

Server::Server(int port, Game* game) : port(port), serverSocket(-1), game(game) {}

bool Server::start() {
    // Initialiser ncurses
    initscr(); 
    // Effacer l'écran
    clear();
    // Afficher un message de démarrage du serveur
    printw("Démarrage du serveur...\n");
    refresh();
    
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        printw("Erreur: Impossible de créer le socket du serveur.\n");
        refresh();
        endwin(); // Terminer ncurses
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printw("Erreur: Impossible de binder le socket.\n");
        refresh();
        close(serverSocket);
        endwin(); // Terminer ncurses
        return false;
    }

    if (listen(serverSocket, 5) < 0) {
        printw("Erreur: Impossible d'écouter sur le port.\n");
        refresh();
        close(serverSocket);
        endwin(); // Terminer ncurses
        return false;
    }

    printw("Serveur en attente de connexions sur le port %d\n", port);
    refresh();
    return true;
}

void Server::handleChatInput(int clientSocket, int clientId) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            printw("Client #%d déconnecté du chat.\n", clientId);
            refresh();
            close(clientSocket);
            //chatMgr.removeClient("Client #" + std::to_string(clientId));
            return;
        }
        std::string message(buffer, bytesReceived);
        std::string chatMessage = "Client #" + std::to_string(clientId) + ": " + message;
        printw("%s\n", chatMessage.c_str());
        refresh();
        //chatMgr.sendClientRequest("Client #" + std::to_string(clientId), chatMessage);
    }
}
void Server::acceptClients() {
    printw("En attente de connexion d'un client...\n");
    refresh();
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientSocket < 0) {
        printw("Erreur: Échec de l'acceptation du client.\n");
        refresh();
        return;
    }

    int clientId = clientIdCounter.fetch_add(1);  // Attribuer un ID unique et incrémenter le compteur
    refresh();
    std::shared_ptr<MenuNode> root = std::make_shared<MenuNode>("Connexion");
    root->makeNodeTree();
    clientMenuChoices[clientId] = root;  // Chaque client commence avec menuChoice = 0
    
    clear();
    sendMenuToClient(clientSocket, game->getMainMenu0()); 
    refresh();

    // Lancer un thread pour gérer ce client
    std::thread clientThread(&Server::handleClient, this, clientSocket, clientId);
    clientThread.detach();
}

void Server::handleClient(int clientSocket, int clientId) {
    char buffer[1024];

    // Envoyer le premier menu au client dès qu'il se connecte
    sendMenuToClient(clientSocket, game->getMainMenu0());

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Client #" << clientId << " déconnecté." << std::endl;
            clientMenuChoices.erase(clientId);
            close(clientSocket);
            break;
        }

        try {
            json receivedData = json::parse(buffer);
            std::string action = receivedData["action"];

            printw("Action reçue du client %d : %s\n", clientId, action.c_str());
            refresh();
            handleMenu(clientSocket, clientId, action);

            // Si le joueur est en jeu, lancer un thread pour recevoir les inputs
            if (clientMenuChoices[clientId]->getName() == "Jouer") {
                receiveInputFromClient(clientSocket, clientId);
            }

        } catch (json::parse_error& e) {
            std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
        }
    }
}

void Server::handleMenu(int clientSocket, int clientId, const std::string& action) {
    if (clientMenuChoices[clientId]->getName() == "Connexion") {
        keyInuptWelcomeMenu(clientSocket, clientId, action);
    }
    else if (clientMenuChoices[clientId]->getName() == "Menu principal") {
        keyInuptMainMenu(clientSocket, clientId, action);
    }
    else if (clientMenuChoices[clientId]->getName() == "Jouer") {
        std::cout << "Client #" << clientId << " est en jeu." << std::endl;
        keyInuptGameMenu(clientSocket, action);
    }
}

void Server::keyInuptWelcomeMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        clientMenuChoices[clientId] = clientMenuChoices[clientId]->getChild("Menu principal");
        sendMenuToClient(clientSocket, game->getMainMenu1());      
    }
    else if (action == "2") {
        clientMenuChoices[clientId] = clientMenuChoices[clientId]->getChild("Créer un compte");
        // Créer un compte => à implémenter
    }
    else if (action == "3") {
        clientMenuChoices.erase(clientId);
        close(clientSocket);
        // Quitter => à implémenter
    }
}

void Server::keyInuptMainMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        clientMenuChoices[clientId] = clientMenuChoices[clientId]->getChild("Jouer");
        runningGame = true;
        receiveInputFromClient(clientSocket, clientId); // lancer un thread pour recevoir les inputs
        loopGame(clientSocket);
    }
    else if (action == "2") {
        clientMenuChoices[clientId] = clientMenuChoices[clientId]->getChild("Amis");
        // Amis => à implémenter
    }
    else if (action == "3") {
        clientMenuChoices[clientId] = clientMenuChoices[clientId]->getChild("Classement");
        // Classements => à implémenter
    }
    else if (action == "4") {
        clientMenuChoices[clientId] = clientMenuChoices[clientId]->getChild("Chat");
        //sendMenuToClient(clientSocket, chatMgr.getChatMenu());
        //chatMgr.addClient("Client #" + std::to_string(clientId), clientSocket);
        std::thread chatThread(&Server::handleChatInput, this, clientSocket, clientId);
        chatThread.detach();
    }
    
    if (action == "5") { 
        clientMenuChoices[clientId] = clientMenuChoices[clientId]->getParent();
        sendMenuToClient(clientSocket, game->getMainMenu0());
    }
}

void Server::keyInuptGameMenu(int clientSocket, const std::string& unicodeAction) {
    std::string action = convertUnicodeToText(unicodeAction);  // Convertir \u0005 en "right"

    if (action == "right") { 
        game->getCurrentPiece().moveRight(game->getGrid());
    }
    else if (action == "left") { 
        game->getCurrentPiece().moveLeft(game->getGrid());
    }
    else if (action == "up") { 
        game->getCurrentPiece().rotate(game->getGrid());
    }
    else if (action == "down"){
        game->getCurrentPiece().moveDown(game->getGrid());
    }
    else if(action == "drop") { // space
        game->getCurrentPiece().dropTetrimino(game->getGrid());
    }
    sendGameToClient(clientSocket);
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
    // Terminer ncurses
    endwin();
}

void Server::sendMenuToClient(int clientSocket, const std::string& screen) {
    send(clientSocket, screen.c_str(), screen.size(), 0);
}

void Server::sendGameToClient(int clientSocket) {
    json message;
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
                    keyInuptGameMenu(clientSocket, action);
                } 
                catch (json::parse_error& e) {
                    std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
                }
            }
        }
    });

    inputThread.detach(); // Permet d’exécuter en parallèle
}

void Server::loopGame(int clientSocket) {
    std::thread gameThread([this]() { // Lancer un thread pour le jeu et le maj
        while (runningGame) {
            game->update(); 
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });

    gameThread.detach();

    while (runningGame) { // Envoi du jeu au client 
        sendGameToClient(clientSocket);
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Pause de 500 ms eviter un crash
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
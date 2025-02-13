#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <atomic>
#include <thread>
#include <fstream>
#include "../common/json.hpp"

using json = nlohmann::json;

Server::Server(int port, Game* game, Grid grid) : port(port), serverSocket(-1), clientIdCounter(0), game(game), grid(grid) {}

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
    sendMenuToClient(clientSocket, Menu::getMainMenu0()); 
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
            // Affichage côté serveur de l'action reçue
            if (menuChoice == 0) {
                keyInuptWelcomeMenu(clientSocket, action);
            }
            else if (menuChoice == 1) {
                keyInuptMainMenu(clientSocket, action);
            }
            if (menuChoice == 2) {
                keyInuptGameMenu(clientSocket, action);
            }
        } 
        catch (json::parse_error& e) {
            std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
        }
    }
}

void Server::keyInuptWelcomeMenu(int clientSocket, const std::string& action) {
    if (action == "1") {
        menuChoice++;
        sendMenuToClient(clientSocket, Menu::getMainMenu1());      
    }
    else if (action == "2") {
        // Créer un compte => à implémenter
    }
    else if (action == "3") {
        close(clientSocket);
    }
}

void Server::keyInuptMainMenu(int clientSocket, const std::string& action) {
    std::cout << "action: aa " << action << std::endl;
    if (action == "1") {
        menuChoice++;
        runningGame = true;
        sendGameToClient(clientSocket, "game");
    }
    else if (action == "2") {
        // Amis => à implémenter
    }
    else if (action == "3") {
        // Classements => à implémenter
    }
    else if (action == "4") {
        // Rejoindre => à implémenter
    }
    if (action == "5") { 
        std::cout << "retour" << std::endl; 
        menuChoice--;
        sendMenuToClient(clientSocket, Menu::getMainMenu0());
    }
}

void Server::keyInuptGameMenu(int clientSocket, const std::string& action) {
    if (action == "right"){
        //
    }
    if (action == "left"){
        //
    }
    if (action == "down"){
        //
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
    //if (runningGame) {
        //runningGame = false;
        //json message;
        //message["RunningGame"] = "true";
        //std::string msg = message.dump();
        //send(clientSocket, msg.c_str(), msg.size(), 0);
    //}
    //else{
        send(clientSocket, screen.c_str(), screen.size(), 0);
    //}
}

void Server::sendGameToClient(int clientSocket, const std::string& screen) {
    json message;

    message["grid"] = grid.gridToJson(); // Ajout de l'envoi de la grille
    
    std::string msg = message.dump();
    send(clientSocket, msg.c_str(), msg.size(), 0);
}

int main() {
    try {
        std::ofstream serverLog("server.log"); // Créer un fichier de log
        // Rediriger std::cout et std::cerr vers le fichier log
        std::cout.rdbuf(serverLog.rdbuf());
        std::cerr.rdbuf(serverLog.rdbuf());

        Game game(10, 20);
        Server server(12345, &game, game.getGrid());
        if (!server.start()) {
            std::cerr << "Erreur: Impossible de démarrer le serveur." << std::endl;
            return 1;
        }

        while (true) {
            server.acceptClients();
        } 

        server.stop();
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }

    return 0;
}

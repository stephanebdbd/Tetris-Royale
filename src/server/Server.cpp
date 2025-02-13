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

Server::Server(int port, Game* game) : port(port), serverSocket(-1), clientIdCounter(0), game(game) {}

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
            std::cout << "Client #" << clientId << " a envoyé l'action: " << action << std::endl;

            if (action == "right") {
                std::cout << "Go à droite pour" << clientId << std::endl;
                game->getCurrentPiece().moveRight(game->getGrid());
            } else if (action == "left") {
                std::cout << "Go à gauche pour" << clientId << std::endl;
                game->getCurrentPiece().moveLeft(game->getGrid());
            } else if (action == "up") {
                std::cout << "Rotation !" << clientId << std::endl;
                game->getCurrentPiece().rotate();
            } else if (action == "down") {
                std::cout << "Go en bas" << clientId << std::endl;
                game->getCurrentPiece().moveDown(game->getGrid());
            } else if (action == "quit") {
                std::cout << "Client #" << clientId << " a quitté." << std::endl;
                // faire quelque chose pour arrêter le jeu
            }
        } catch (json::parse_error& e) {
            std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
        }
    }
}


void Server::stop() {
    if (serverSocket != -1) {
        shutdown(serverSocket, SHUT_RDWR);
        close(serverSocket);
        serverSocket = -1;
    }
}

int main() {
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
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }

    return 0;
}

#include "ClientNetwork.hpp"

#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>


bool ClientNetwork::connectToServer(const std::string& serverIP, int port, int& clientSocket) {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Erreur: Impossible de créer le socket client." << std::endl;
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Erreur: Adresse IP invalide." << std::endl;
        close(clientSocket);
        return false;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Erreur: Connexion au serveur échouée." << std::endl;
        close(clientSocket);
        return false;
    }
    std::cout << "Connecté au serveur." << std::endl;
    return true;
}

void ClientNetwork::disconnect(int& clientSocket) {
    std::cout << "Déconnecté du serveur." << std::endl;
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
    }
}

void ClientNetwork::sendData(const std::string& data, int clientSocket) {
    if (send(clientSocket, data.c_str(), data.size(), 0) == -1)
        std::cerr << "Erreur: Impossible d'envoyer les données." << std::endl;
}

bool receivedData(int clientSocket, std::string& received, char *buffer) {

    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        return false;
    }
    buffer[bytesReceived] = '\0';
    received += buffer;
    return true;
    
}
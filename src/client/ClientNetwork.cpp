#include "ClientNetwork.hpp"

#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>


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

bool ClientNetwork::sendData(const std::string& data, int clientSocket) {
    if (send(clientSocket, data.c_str(), data.size(), 0) == -1) {
        std::cerr << "Erreur: Impossible d'envoyer les données." << std::endl;
        return false;
    }
    return true;
}
int ClientNetwork::receivedData(int clientSocket, char *buffer) {
    memset(buffer, 0, 1024); // 1024 is the actual size of the buffer
    int bytesReceived = recv(clientSocket, buffer, 1023, 0); // Adjust the size to 1023 to leave space for null terminator
    if (bytesReceived <= 0) {
        return -1;
    }
    buffer[bytesReceived] = '\0';
    return bytesReceived;
    
}
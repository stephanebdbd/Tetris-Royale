#include "Client.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <ncurses.h>
#include "../common/json.hpp"

using json = nlohmann::json;

Client::Client(const std::string& serverIP, int port) : serverIP(serverIP), port(port), clientSocket(-1) {}

bool Client::connectToServer() {
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

void Client::sendInput(const std::string& action) {
    if (clientSocket == -1) return;

    json message;
    message["action"] = action;
    std::string msg = message.dump();

    std::cout << "Envoi au serveur: " << msg << std::endl;

    if (send(clientSocket, msg.c_str(), msg.size(), 0) == -1) {
        std::cerr << "Erreur: Impossible d'envoyer le message." << std::endl;
    }
}

void Client::disconnect() {
    if (clientSocket != -1) {
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
        clientSocket = -1;
    }
}

void Client::run() {
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);

    while (true) {
        receiveAndDisplayMenu();
        char choice = getch();
        echo();
        if (choice == '1'){
            sendInput("1");
            break;
        }

    }
    while (true) {
        clear();
        receiveAndDisplayMenu();
    }
    

    endwin();
    
}


int main() {
    Client client("127.0.0.1", 12345);

    if (!client.connectToServer()) {
        return 1;
    }
    client.run();
    client.disconnect();
    return 0;
}//test



void Client::receiveAndDisplayMenu() {
    char buffer[1024]; // Stockage du texte reçu
    memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        std::cerr << "Erreur : Impossible de recevoir l'affichage du serveur." << std::endl;
        return;
    }

    clear();
    int y = 5;
    char* line = strtok(buffer, "\n");
    while (line) {
        mvprintw(y++, 10, "%s", line); // Affichage du texte reçu
        line = strtok(NULL, "\n"); 
    }
    refresh();
}

#include "Client.hpp"
#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>

#include "../common/json.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <ncurses.h>
#include <thread>

using json = nlohmann::json;

    // Création du socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Erreur: Impossible de créer le socket\n";
        exit(1);
    }

    // Configuration de l'adresse du serveur
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    // Connexion au serveur
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Erreur: Impossible de se connecter au serveur\n";
        exit(1);
    }
    isConnected = true;
    std::cout << "✅ Connecté au serveur !\n";
}
bool Client::login() {
    std::string userName, pseudonym, password;

    // Demander les informations
    std::cout << "Nom d'utilisateur: ";
    std::getline(std::cin, userName);
    std::cout << "Pseudonyme: ";
    std::getline(std::cin, pseudonym);
    std::cout << "🔒 Mot de passe: ";
    std::getline(std::cin, password);

    // Créer l'objet User avec les informations fournies
    user = std::make_shared<User>(userName, pseudonym, password);

    // Envoyer les infos d'authentification au serveur
    std::string authMessage = "LOGIN " + userName + " " + pseudonym + " " + password;
    send(sock, authMessage.c_str(), authMessage.size(), 0);

    // Attendre la réponse du serveur
    char buffer[256];
    int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::string response(buffer);
        if (response == "SUCCESS") {
            std::cout << "✅ Connexion réussie !\n";
            return true;
        } else {
            std::cout << "❌ Échec de l'authentification !\n";
            return false;
        }
    }

    return false;
}
// Envoi de message au serveur en préfixant avec le pseudonyme de l'utilisateur
void Client::sendMessage(const std::string& message) {
    std::string fullMessage = user->getPseudonym() + ": " + message;
    send(sock, fullMessage.c_str(), fullMessage.size(), 0);
}

void Client::handleUserInput() {
    halfdelay(1);  // Attend 100ms max pour stabiliser l'affichage
    while (true) {
        int ch = getch();
        if (ch != ERR) {  // Si une touche est pressée
            if (ch == 'q') {
                network.disconnect(clientSocket);
                endwin();
                exit(0); 
            }
            std::string action(1, ch);
            controller.sendInput(action, clientSocket);  // Envoyer au serveur
        }
    }
}

void Client::receiveDisplay() {
    std::string received;

    while (true) {
        char buffer[12000];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived > 0) {
            received += std::string(buffer, bytesReceived);

            // Vérifier si un JSON complet est reçu (fini par un '\n') 
            size_t pos = received.find("\n");
            while (pos != std::string::npos) {
                std::string jsonStr = received.substr(0, pos);  // Extraire un JSON complet
                received.erase(0, pos + 1);  // Supprimer le JSON traité

                try {
                    json data = json::parse(jsonStr);  // Parser uniquement un JSON complet

                    // Si c'est une grille de jeu
                    if (data.contains("grid")) {   
                        display.displayGame(data);
                    }
                    // Sinon, c'est un menu
                    else {
                        display.displayMenu(data);
                    }

                    refresh();  // Rafraîchir l'affichage après mise à jour du jeu ou menu
                } catch (json::parse_error& e) {
                    std::cerr << "Erreur de parsing JSON CLIENT: " << e.what() << std::endl;
                }

                pos = received.find("\n");  // Vérifier s'il reste d'autres JSON dans le buffer
            }
        }
    }
}

#include "Client.hpp"

#include "../common/json.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <ncurses.h>
#include <thread>

using json = nlohmann::json;

Client::Client(const std::string& serverIP, int port) : serverIP(serverIP), port(port), clientSocket(-1) {}

void Client::run() {
    if (!network.connectToServer(serverIP, port, clientSocket)) {
        std::cerr << "Erreur: Impossible de se connecter au serveur." << std::endl;
        return;
    }
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); // Permet de ne pas bloquer l'affichage en attendant un input

    // Lancer un thread pour écouter les touches et envoyer les inputs
    std::thread inputThread(&Client::handleUserInput, this);
    inputThread.detach(); // Permet au thread de fonctionner indépendamment

    // Boucle principale pour recevoir et afficher le jeu
    while (true) {
        receiveDisplay();
    }

    network.disconnect(clientSocket);
    delwin(stdscr);
    endwin();
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
```cpp
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
                    else if (data.contains("menu")) {
                        display.displayMenu(data);
                    }
                    else {
                        std::cerr << "Erreur : Type de données inconnu." << std::endl;
                    }

                    refresh();  // Rafraîchir l'affichage après mise à jour du jeu ou menu
                } catch (json::parse_error& e) {
                    std::cerr << "Erreur de parsing JSON CLIENT: " << e.what() << std::endl;
                }

                pos = received.find("\n");  // Vérifier s'il reste d'autres JSON dans le buffer
            }
        }
        else if (bytesReceived == 0) {
            std::cerr << "Erreur : Connexion avec le serveur perdue." << std::endl;
            network.disconnect(clientSocket);
            endwin();
            exit(0);
        }
        else {
            std::cerr << "Erreur : Réception de données échouée." << std::endl;
        }
    }
}
``````cpp
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
                    else if (data.contains("menu")) {
                        display.displayMenu(data);
                    }
                    else {
                        std::cerr << "Erreur : Type de données inconnu." << std::endl;
                    }

                    refresh();  // Rafraîchir l'affichage après mise à jour du jeu ou menu
                } catch (json::parse_error& e) {
                    std::cerr << "Erreur de parsing JSON CLIENT: " << e.what() << std::endl;
                }

                pos = received.find("\n");  // Vérifier s'il reste d'autres JSON dans le buffer
            }
        }
        else if (bytesReceived == 0) {
            std::cerr << "Erreur : Connexion avec le serveur perdue." << std::endl;
            network.disconnect(clientSocket);
            endwin();
            exit(0);
        }
        else {
            std::cerr << "Erreur : Réception de données échouée." << std::endl;
        }
    }
}
```
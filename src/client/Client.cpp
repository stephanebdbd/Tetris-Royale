#include "Client.hpp"

#include "../common/json.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <ncurses.h>
#include <thread>
#include <fstream>

using json = nlohmann::json;

Client::Client(const std::string& serverIP, int port) : serverIP(serverIP), port(port), clientSocket(-1), stopInputThread(false) {}

void Client::run() {
    if (!network.connectToServer(serverIP, port, clientSocket)) {
        std::cerr << "Erreur: Impossible de se connecter au serveur." << std::endl;
        return;
    }
    chat.setClientSocket(clientSocket);

    // Lancer un thread pour écouter les touches et envoyer les inputs
    std::thread inputThread(&Client::handleUserInput, this);
    inputThread.detach(); // Permet au thread de fonctionner indépendamment

    // Rediriger std::cout vers un fichier avant de lancer le chat
    std::ofstream file("chat.txt", std::ios::app);
    if (!file) {
        std::cerr << "Erreur: Impossible d'ouvrir chat.txt" << std::endl;
        return;
    }
    std::streambuf* coutbuf = std::cout.rdbuf(); // Sauvegarder le buffer de cout
    std::cout.rdbuf(file.rdbuf()); // Rediriger cout vers le fichier

    // Boucle principale pour recevoir et afficher le jeu
    while (true) {
        receiveDisplay();
    }

    std::cout.rdbuf(coutbuf); // Restaurer le buffer de cout
    file.close();

    network.disconnect(clientSocket);
    delwin(stdscr);
    endwin();
}


void Client::handleUserInput() {
    halfdelay(1);  // Attend 100ms max pour stabiliser l'affichage
    while (true) {
        if (chatMode) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Attendre 100ms avant de vérifier à nouveau
            continue;
        }

        int ch = getch();
        if (ch != ERR) {  // Si une touche est pressée
            if (ch == 'q') {
                network.disconnect(clientSocket);
                endwin();
                exit(0); 
            }
            std::string action(1, ch);
            controller.sendInput(action, clientSocket);
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
                        isPlaying = true;
                        chatMode = false;
                        display.displayGame(data);

                    }
                    // Si c'est un message de chat
                    else if (data.contains("mode") && data["mode"] == "chat") {
                        chatMode = true;
                        isPlaying = false;
                        // Lancer le chat dans un thread
                        std::thread chatThread(&ClientChat::run, &chat);
                        chatThread.detach();
                        std::cout << "Chat thread launched" << std::endl;  // Ajout de message de débogage
                        display.displayChat(data);
        
                    }
                    // Sinon, c'est un menu
                    else {
                        chatMode = false;
                        isPlaying = false;
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
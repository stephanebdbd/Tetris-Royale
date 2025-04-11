#include "Client.hpp"

#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <ncurses.h>
#include <thread>
#include <fstream>


Client::Client(const std::string& serverIP, int port) : serverIP(serverIP), port(port), clientSocket(-1), stopInputThread(false) {}

void Client::run() {
    if (!connect()) {
        std::cerr << "Erreur: Impossible de se connecter au serveur." << std::endl;
        return;
    }

    // Lancer un thread pour écouter les touches et envoyer les inputs
    std::thread inputThread(&Client::handleUserInput, this);
    inputThread.detach(); // Permet au thread de fonctionner indépendamment

    // Boucle principale pour recevoir et afficher le jeu

    receiveDisplay();
    network.disconnect(clientSocket);
    delwin(stdscr);
    endwin();
}

bool Client::connect() {
    if (!network.connectToServer(serverIP, port, clientSocket)) {
        std::cerr << "Erreur: Impossible de se connecter au serveur." << std::endl;
        return false;
    }

    chat.setClientSocket(clientSocket);
    return true;
}


void Client::handleUserInput() {
    halfdelay(1);  // Attend 100ms max pour stabiliser l'affichage
    std::string inputBuffer;  // Buffer pour stocker l'entrée utilisateur
    
    while (true) {
        
        if (chatMode) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Attendre 100ms avant de vérifier à nouveau
            continue;
        }

        int ch = getch();
        if (ch != ERR) {  // Si une touche est pressée
            // Gestion directe pour les touches pour jouer
            if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT || ch == ' ') {
                // Si le buffer contient déjà une commande en cours, on l'envoie d'abord
                if (!inputBuffer.empty()) {
                    if (inputBuffer == "q") {
                        network.disconnect(clientSocket);
                        exit(0);
                    }
                    controller.sendInput(inputBuffer, clientSocket);
                    inputBuffer.clear();
                }
                // Envoie immédiat des touches pour jouer
                std::string specialAction(1, static_cast<char>(ch));
                controller.sendInput(specialAction, clientSocket);
            } 
            else if (ch == '\n') {  // Si le joueur appuie sur Enter
                if (!inputBuffer.empty()) {
                    if (inputBuffer == "q") {
                        network.disconnect(clientSocket);
                        exit(0);
                    }
                    controller.sendInput(inputBuffer, clientSocket);
                    inputBuffer.clear();
                }
            }
            else if (ch == KEY_BACKSPACE || ch == 127) { // Gestion de la touche Backspace
                if (!inputBuffer.empty()) {
                    inputBuffer.pop_back(); // Supprime le dernier caractère
                    clrtoeol();
                    refresh();
                }
            }
            else {
                // Ajout des caractères sur le buffer
                inputBuffer += static_cast<char>(ch);
            }
        }
    }
}

void Client::receiveDisplay() {
    std::string received;

    while (true) {
        char buffer[12000];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            delwin(stdscr);
            endwin();  // Quitte ncurses proprement
            std::cout << "\033[0m\033[2J\033[H" << std::flush;  // Réinitialise couleurs + clear écran
            std::cout << "Déconnecté du serveur. Fin du programme." << std::endl;            
            return;
        }

        received += std::string(buffer, bytesReceived);

        // Vérifier si un JSON complet est reçu (fini par un '\n') 
        std::size_t pos = received.find("\n");
        while (pos != std::string::npos) {
            std::string jsonStr = received.substr(0, pos);  // Extraire un JSON complet
            received.erase(0, pos + 1);  // Supprimer le JSON traité

            try {
                json data = json::parse(jsonStr);

                if (data.contains(jsonKeys::GRID)) {
                    isPlaying = true;
                    chatMode = false;
                    display.displayGame(data);

                } else if (data.contains(jsonKeys::MODE) && data[jsonKeys::MODE] == "chat") {
                    chatMode = true;
                    isPlaying = false;
                    std::thread chatThread(&ClientChat::run, &chat);
                    chatThread.detach();

                } else if (data.contains("sender")) {
                    chat.receiveChatMessages(data);

                } else {
                    chatMode = false;
                    isPlaying = false;
                    display.displayMenu(data);
                }

                refresh();  // Rafraîchir l'affichage après mise à jour du jeu ou menu
            } catch (json::parse_error& e) {
                std::cerr << "Erreur de parsing JSON CLIENT: " << e.what() << std::endl;
                    break;
            }

                pos = received.find("\n");  // Vérifier s'il reste d'autres JSON dans le buffer
            }
        }
    }


void Client::sendSFMLInput(sf::Keyboard::Key key) {
    std::string action;
    std::cout << "Key pressed: " << key << std::endl;

    if (key == sf::Keyboard::Up) {
        action = "UP";
    }
    else if (key == sf::Keyboard::Down) {
        action = "DOWN";
    }
    else if (key == sf::Keyboard::Left) {
        action = "LEFT";
    }
    else if (key == sf::Keyboard::Right) {
        action = "RIGHT";
    }
    else if (key == sf::Keyboard::Space) {
        action = " ";
    }
    else {
        action = std::string(1, static_cast<char>(key));
    }
    
    if (!action.empty()) {
        controller.sendInput(action, clientSocket);
    }
}

#include "Client.hpp"

#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <ncurses.h>
#include <thread>
#include <fstream>


Client::Client(const std::string& serverIP, int port) : serverIP(serverIP), port(port), clientSocket(-1) {}

Client::~Client() {
    stopThreads();
    if (inputThread.joinable()) {
        inputThread.join();
    }
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
}

void Client::run() {
    if (!connect()) {
        std::cerr << "Error: Could not connect to server." << std::endl;
        return;
    }

    // Start threads (no longer detached)
    inputThread = std::thread(&Client::handleUserInput, this);
    receiveThread = std::thread(&Client::receiveDisplay, this);

    // Wait for threads to finish (they won't unless stop_threads is set)
    inputThread.join();
    receiveThread.join();

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
    
    while (!stop_threads) {
        
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
    // Make received a member variable instead of local
    while (!stop_threads) {
        char buffer[12000];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived > 0) {
            std::lock_guard<std::mutex> lock(receiveMutex); // Add mutex for thread safety
            receivedData += std::string(buffer, bytesReceived); // Use member variable

            // Process complete JSON messages
            size_t pos = receivedData.find("\n");
            while (pos != std::string::npos) {
                try {

                    std::string jsonStr = receivedData.substr(0, pos);
                    receivedData.erase(0, pos + 1);
                    json data = json::parse(jsonStr);
                    
                    // Si c'est une grille de jeu
                    if (data.contains(jsonKeys::GRID)) {
                        isPlaying = true;
                        chatMode = false;
                        serverData = data;
                        //display.displayGame(data);
                    }
                    // Si c'est un message de chat
                    else if (data.contains(jsonKeys::MODE) && data[jsonKeys::MODE] == "chat") {
                        chatMode = true;
                        isPlaying = false;
                        // Lancer le chat dans un thread
                        std::thread chatThread(&ClientChat::run, &chat);
                        chatThread.detach();
        
                    }
                    // Si c'est un message de chat
                    else if (data.contains("sender")) {
                        chat.receiveChatMessages(data);
                    }
                    // Sinon, c'est un menu
                    else {
                        chatMode = false;
                        isPlaying = false;
                        // changer le menuState dans le cas de GUI
                        if(data.contains("state")) {
                            currentMenuState = menuStateManager.deserialize(data["state"]);
                            serverData = data;
                        }
                        //sinon on affiche le menu sur le terminal
                        else {
                            display.displayMenu(data);
                        }
                        
                    }

                    refresh();  // Rafraîchir l'affichage après mise à jour du jeu ou menu
                } catch (json::parse_error& e) {
                    std::cerr << "JSON parse error: " << e.what() << std::endl;
                }

                pos = receivedData.find("\n");
            }
        }
        else if (bytesReceived == 0) {
            // Connection closed
            stopThreads();
            break;
        } else {
            if (stop_threads) {
                break;  // Exit if stop_threads is set
            }
            // Error
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                stopThreads();
            }
        }
    }
}


MenuState Client::getCurrentMenuState() {
    return currentMenuState; 
}
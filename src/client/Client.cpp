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
void Client::setTemporaryMessage(const std::string& msg) {
    std::lock_guard<std::mutex> lock(messageMutex);
    temporaryMessage = msg;
}

std::string Client::getTemporaryMessage() {
    std::lock_guard<std::mutex> lock(messageMutex);
    std::string msg = temporaryMessage;
    temporaryMessage.clear();  // Efface le message après l'avoir lu
    return msg;
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
                    if (data.contains(jsonKeys::GRID)|| data.contains("otherPlayersGrids")) {
                        isPlaying = true;
                        chatMode = false;
                        if(isTerminal && !data.contains("otherPlayersGrids"))
                            display.displayGame(data);
                        else
                            setGameStateFromServer(data);
                    }
                    // Si c'est un message de chat
                    else if (data.contains(jsonKeys::MODE) && data[jsonKeys::MODE] == "chat") {
                        chatMode = true;
                        isPlaying = false;
                        // Lancer le chat dans un thread
                        std::thread chatThread(&ClientChat::run, &chat);
                        chatThread.detach();
        
                    }
                    else if (data.contains(jsonKeys::TEMPORARY_DISPLAY)) {
                        std::string message = data[jsonKeys::TEMPORARY_DISPLAY];
                        setTemporaryMessage(message);  // Stocke le message pour l'affichage temporaire
                    }
                    
                    // Si c'est un message de chat
                    else if (data.contains("sender")) {
                        if(chatMode)
                            chat.receiveChatMessages(data);
                        serverData = data;
                    }
                    else {
                        chatMode = false;
                        isPlaying = false;
                        // changer le menuState dans le cas de GUI
                        if(data.contains("state")) {
                            setGameStateFromServer(data);
                            if(data.contains("message")){
                                std::string message = data["message"];
                                std::cout << "Message reçu : " << message << std::endl;
                                if(message == "avatar"){
                                    int avatarIndex = std::stoi(data["data"][0].get<std::string>());
                                    setAvatarIndex(avatarIndex);
                                }
                                if (data.contains("dataPair") && message == "contacts") {
                                    contacts = data["dataPair"].get<std::vector<std::pair<std::string, int>>>();
                                    std::cout << "Contacts: " << std::endl;
                                    for (const auto& contact : contacts) {
                                        std::cout << "Nom: " << contact.first << ", Avatar: " << contact.second << std::endl;
                                    }
                                }
                                
                            }
                                
                                
                            std::cout << "MenuState: " << data["state"] << std::endl;
                            currentMenuState = menuStateManager.deserialize(data["state"]);
                            serverData = data;
                            
                        } else {
                            if(isTerminal)
                                display.displayMenu(data, inputBuffer);
                            else
                                setGameStateFromServer(data);


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
void Client::setGameStateFromServer(const json& data) {
    std::lock_guard<std::mutex> lock(gameStateMutex);

    if (data.contains(jsonKeys::GRID)) {
        gameState.gridData = data[jsonKeys::GRID];
        gameState.currentPieceData = data[jsonKeys::TETRA_PIECE];
        gameState.nextPieceData = data[jsonKeys::NEXT_PIECE];
        gameState.scoreData = data[jsonKeys::SCORE];
        gameState.isGame = true;
        gameState.message = data[jsonKeys::MESSAGE_CIBLE];

    }
    else if (data.contains("otherPlayersGrids")){
        gameState.miniGrid = data["otherPlayersGrids"];
        gameState.miniUpdate = true;
    }

    else if(data.contains("data") || data.contains("secondData")){
        gameState.friendsLobby = data["data"];
        gameState.pseudos = data["secondData"];
    }
    else{
        gameState.isGame = false;
        gameState.isEnd = true;
        gameState.menu = data;
        
        
    }
    gameState.updated = true;
    

    
}

const GameState Client::getGameState() {
    std::lock_guard<std::mutex> lock(gameStateMutex);
    return gameState;
}

bool Client::isGameStateUpdated() {
    std::lock_guard<std::mutex> lock(gameStateMutex);
    return gameState.updated;
}

void Client::setGameStateUpdated(bool updated) {
    std::lock_guard<std::mutex> lock(gameStateMutex);
    gameState.updated = updated;
}

void Client::setGameStateIsEnd(bool isEnd) {
    std::lock_guard<std::mutex> lock(gameStateMutex);
    gameState.isEnd = isEnd;
}

void Client::sendInputFromSFML(const std::string& input) {
    if (!input.empty()) {
        controller.sendInput(input, clientSocket);
    }
}

#include "Client.hpp"

#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <ncurses.h>
#include <thread>
#include <fstream>
#include <cerrno>

Client::Client(const std::string& serverIP, int port) : serverIP(serverIP), port(port), clientSocket(-1) {}

Client::~Client() {
    stopThreads();
}

void Client::run(const std::string& mode) {
    if (!connect()) {
        std::cerr << "Error: Could not connect to server." << std::endl;
        return;
    }

    json j;
    j["mode"] = mode;
    network.sendData(j.dump() + "\n", clientSocket);

    // Création du thread
    inputThread = std::thread(&Client::handleUserInput, this);
    receiveThread = std::thread(&Client::receiveDisplay, this);

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
    halfdelay(1);  // 100 ms d'attente
    
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
    while (!stop_threads) {
        std::string dataChunk = receiveData();
        if (dataChunk.empty()) continue;
        appendToBuffer(dataChunk);
        processBufferedMessages();
    }
}

std::string Client::receiveData() {
    char buffer[12000];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived > 0) {
        return std::string(buffer, bytesReceived);
    } else if (bytesReceived == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
        stopThreads();
    }
    return "";
}

void Client::appendToBuffer(const std::string& chunk) {
    std::lock_guard<std::mutex> lock(receiveMutex);
    receivedData += chunk;
}

void Client::processBufferedMessages() {
    std::lock_guard<std::mutex> lock(receiveMutex);
    std::size_t pos;
    while ((pos = receivedData.find('\n')) != std::string::npos) {
        std::string jsonStr = receivedData.substr(0, pos);
        receivedData.erase(0, pos + 1);
        handleJsonMessage(jsonStr);
    }
}

void Client::handleJsonMessage(const std::string& jsonStr) {
    try {
        json data = json::parse(jsonStr);

        if (data.contains(jsonKeys::GRID) || data.contains("otherPlayersGrids")) {
            handleGameGrid(data);
        } else if (data.contains(jsonKeys::MODE) && data[jsonKeys::MODE] == "chat") {
            startChatMode(data);
        } else if (data.contains(jsonKeys::TEMPORARY_DISPLAY)) {
            setTemporaryMessage(data[jsonKeys::TEMPORARY_DISPLAY]);
        } else if (data.contains("sender")) {
            handleChatMessage(data);
        } else if (data.is_array()) {
            handleChatHistory(data);
        } else {
            handleOtherMessages(data);
        }
        refresh();
    } catch (json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
}

void Client::handleGameGrid(const json& data) {
    isPlaying = true;
    chatMode = false;
    if (isTerminal && !data.contains("otherPlayersGrids"))
        display.displayGame(data);
    else
        setGameStateFromServer(data);
}

void Client::startChatMode(const json& data) {
    chatMode = true;
    chat.setMyPseudo(data["pseudo"]);
    std::thread chatThread(&ClientChat::run, &chat);
    chatThread.detach();
}

void Client::handleChatMessage(const json& data) {
    if (chatMode)
        chat.receiveChatMessages(data);
    serverData = data;
}

void Client::handleChatHistory(const json& data) {
    for (const auto& msg : data) {
        if (msg.is_object() && msg.contains("message")) {
            chat.addChatMessage(msg);
        }
    }
    chat.displayChatMessages();
}

void Client::handleStatefulData(const json& data) {
    setGameStateFromServer(data);
    serverData = data;
    if (data.contains("message")) {
        std::string message = data["message"];

        if (message == "avatar") {
            int avatarIndex = std::stoi(data["data"][0].get<std::string>());
            setAvatarIndex(avatarIndex);
        }

        if (message == "contacts") {
            if(!data["dataPair"].empty()) setContacts(data["dataPair"].get<std::vector<std::pair<std::string, int>>>());
            else {
                std::vector<std::string> stringData = data["data"].get<std::vector<std::string>>();
                std::vector<std::pair<std::string, int>> convertedData;
                for (const auto& str : stringData) {
                    convertedData.emplace_back(str, -1); // Pas d'avatar
                }
                setContacts(convertedData);
            }
        }

        if (message == jsonKeys::FRIEND_LIST) {
            setAmis(data["data"].get<std::vector<std::string>>());
            std::cout << "Amis:\n";
            for (const auto& ami : amis) {
                std::cout << "Nom: " << ami << '\n';
            }
        }

        if(data.contains("data") && message == jsonKeys::TEAMS_LIST){
            setTeams(data["data"].get<std::vector<std::string>>());
            std::cout << "TEAMS: " << std::endl;
            for (const auto& teamName : teams) {
                std::cout << "Nom: " << teamName << std::endl;
            }
        }

        if (message == "ranking") {
            setRanking(data["secondData"].get<std::map<std::string, std::vector<std::string>>>());
            std::cout << "Classement mis à jour.\n";
        }

        if (message == "player_info") {
            setShow(true);
            setPlayerInfo(data["data"].get<std::vector<std::string>>());
            std::cout << "Infos joueur :\n";
            for (const auto& p : PlayerInfo) {
                std::cout << "Nom: " << p << '\n';
            }
        }
    }

    std::cout << "MenuState: " << data["state"] << '\n';
    currentMenuState = menuStateManager.deserialize(data["state"]);
    serverData = data;
}

void Client::handleOtherMessages(const json& data) {
    chatMode = false;
    isPlaying = false;
    if (data.contains("state")) {
        handleStatefulData(data);
    } else {
        if (isTerminal)
            display.displayMenu(data, inputBuffer);
        else
            setGameStateFromServer(data);
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
        gameState.playerNumberData = data[jsonKeys::PLAYER_NUMBER];
        gameState.isGame = true;
        gameState.message = data[jsonKeys::MESSAGE_CIBLE];

    }
    else if (data.contains("otherPlayersGrids")){
        gameState.miniGrid = data["otherPlayersGrids"];
        gameState.miniUpdate = true;
    }

    else if(data.contains("data") || data.contains("secondData") || data.contains("message")){
        gameState.friendsLobby = data["data"];
        gameState.pseudos = data["secondData"];
        gameState.showCommand = data["message"];
    }
    else{
        gameState.isGame = false;
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

void Client::reintiliseData(){
    std::lock_guard<std::mutex> lock(gameStateMutex);
    gameState.gridData.clear();
    gameState.currentPieceData.clear();
    gameState.nextPieceData.clear();
    gameState.scoreData.clear();
    gameState.playerNumberData.clear();
    gameState.message.clear();
    gameState.miniGrid.clear();
    gameState.miniUpdate = false;
    gameState.isGame = false;
    gameState.friendsLobby.clear();
    gameState.pseudos.clear();
    gameState.showCommand.clear();


}

void Client::sendInputFromSFML(const std::string& input) {
    if (!input.empty()) {
        controller.sendInput(input, clientSocket);
    }
}

const std::vector<std::pair<std::string, int>>&  Client:: getContacts() const {
    return contacts;
}

void Client::setContacts(const std::vector<std::pair<std::string, int>>& newContacts) {
    contacts = newContacts;
}

void Client::setRanking(std::map<std::string, std::vector<std::string>> ranking1) {
    ranking = ranking1;
}
std::map<std::string, std::vector<std::string>> Client::getRanking() const {
    return ranking;
}
void Client::setAmis(const std::vector<std::string>& friends) {
    amis = friends;
}
std::vector<std::string> Client::getAmis() const {
    return amis;
}
void Client::setPlayerInfo(const std::vector<std::string>& playerInfo) {
    PlayerInfo = playerInfo;
}
std::vector<std::string> Client::getPlayerInfo() const {
    return PlayerInfo;
}
void Client::setShow(bool showfenetre) {
    show = showfenetre;
}
bool Client::getShow() const {
    return show;
}

void Client::setTeams(const std::vector<std::string>& teams) {
    this->teams = teams;
}

std::vector<std::string> Client::getTeams() const {
    return teams;
}

void Client::clearServerData() {
    serverData.clear();
}

void Client::setAvatarIndex(int index) {
    avatarIndex = index;
}

int Client::getAvatarIndex() const {
    return avatarIndex;
}
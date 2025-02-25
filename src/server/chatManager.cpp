#include "chatManager.hpp"
#include <algorithm>


void ChatManager::start() {
    chat.start();
}

void ChatManager::stop() {
    chat.stop();
}

void ChatManager::addClient(const std::string& pseudoName, int socket) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients[pseudoName] = socket;
}

void ChatManager::removeClient(const std::string& pseudoName) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(pseudoName);
}

void ChatManager::sendClientRequest(const std::string& pseudoName, const std::string& message) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = clients.find(pseudoName);
    if (it != clients.end()) {
        //chat.sendMessage(it->second, message);
    }
}

void ChatManager::acceptClientRequest(const std::string& pseudoName, int socket) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients[pseudoName] = socket;
}

void ChatManager::handleClient(int clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            auto it = std::find_if(clients.begin(), clients.end(),
                                   [clientSocket](const auto& pair) { return pair.second == clientSocket; });
            if (it != clients.end()) {
                clients.erase(it);
            }
            close(clientSocket);
            return;
        }
        std::string message(buffer, bytesReceived);
        std::string sender = "Client #" + std::to_string(clientSocket);  // Vous pouvez utiliser un pseudoName ici
        broadcastMessage(message, sender);
    }
}

void ChatManager::broadcastMessage(const std::string& message, const std::string& sender) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (const auto& [pseudoName, socket] : clients) {
        std::string fullMessage = sender + ": " + message;
        //chat.sendMessage(socket, fullMessage);
    }
}

std::string ChatManager::getChatMenu() const {
    return "Vous êtes dans le chat. Tapez votre message et appuyez sur Entrée.\n";
}
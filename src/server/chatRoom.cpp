#include "chatRoom.hpp"
#include <algorithm>
#include "Server.hpp"



void chatRoom::addClient(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.insert(pseudo);
}

void chatRoom::removeClient(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(pseudo);  // Corrected line
}

void chatRoom::acceptClientRequest(const std::string& pseudo) {
    addClient(pseudo);
}

void chatRoom::refuseClientRequest(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(requestsMutex);
    receivedReq.erase(pseudo);  // Corrected line
}

void chatRoom::broadcastMessage(const std::string& message, const std::string& sender, Server* server) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    ServerChat chat;  // Initialize the chat object
    for (auto& client : clients) {
        // send message to client
        chat.sendMessage(server->getPseudoSocket()[client], sender, message);
    }   
}
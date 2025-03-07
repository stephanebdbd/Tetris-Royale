#include "chatRoom.hpp"
#include <algorithm>
#include "Server.hpp"


chatRoom::chatRoom(std::string room_name, int admin_id) : roomName(room_name), adminId(admin_id) {}


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

std::string chatRoom::getRoomName() const {
    return roomName;
}

int chatRoom::getAdminId() const {
    return adminId;
}

std::set<std::string> chatRoom::getClients() const {
    return clients;
}

std::set<std::string> chatRoom::getReceivedReq() const {
    return receivedReq;
}

std::set<std::string> chatRoom::getSentReq() const {
    return sentReq;
}
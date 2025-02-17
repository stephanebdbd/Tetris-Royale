#include "chat.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>


ServerChat::~ServerChat() {
    stop();
}


void ServerChat::handleClientChat(int clientSocket) {
    char buffer[1024];
    std::string userId;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0) break;

        ChatMessage msg = ChatMessage::deserialize(buffer);
        std::cout << "[" << msg.senderId << "] : " << msg.message << std::endl;

        // Broadcast selon le canal
        broadcastMessage(msg, msg.channel);
    }

    // Suppression du client à la déconnexion
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(userId);
    close(clientSocket);
}

void ServerChat::broadcastMessage(const ChatMessage& msg, const std::string& channel) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (const auto& [id, socket] : clients) {
        sendMessage(socket, msg.serialize());
    }
}

void ServerChat::sendMessage(int clientSocket, const std::string& message) {
    send(clientSocket, message.c_str(), message.size(), 0);
}

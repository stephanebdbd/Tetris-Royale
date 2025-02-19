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
    uint32_t userId;

    while (ReadStreamMessage(clientSocket, buffer, sizeof(buffer), userId) > 0) {
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
    clients.erase(std::to_string(userId));
    close(clientSocket);
}

void ServerChat::broadcastMessage(const ChatMessage& msg, const std::string& channel) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (const auto& [id, socket] : clients) {
        sendMessage(socket, msg.serialize());
    }
}

void sendMessageToDest(char *Message, int client_fd) {
    uint32_t messageSize = (uint32_t)strlen(Message);               
    WriteStreamMessage(client_fd, Message, messageSize);      
}
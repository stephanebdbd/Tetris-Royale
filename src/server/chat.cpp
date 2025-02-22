#include "chat.hpp"


using json = nlohmann::json;


void ServerChat::processClientChat(int clientSocket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            close(clientSocket);
            return;
        }

        try {
            json msg = json::parse(std::string(buffer, bytes_received));
            std::string receiver = msg["receiver"];
            std::string message = msg["message"];

            std::cout << "[receiver: " << receiver << "] " << message << std::endl;
            broadcastMessage(receiver, message);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

void ServerChat::broadcastMessage(const std::string& message, const std::string& channel) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (const auto& [id, socket] : clients) {
        sendMessage(socket, message);
    }
}

void sendMessageToDest(char *Message, int client_fd) {
    uint32_t messageSize = (uint32_t)strlen(Message);               
    WriteStreamMessage(client_fd, Message, messageSize);      
}
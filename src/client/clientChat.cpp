#include "clientChat.hpp"

using json = nlohmann::json;


ClientChat::ClientChat(int clientSocket) : clientSocket(clientSocket) {}

void ClientChat::sendChatMessage(const std::string& message) {
    json j;
    j["type"] = "chat";
    j["message"] = message;
    network.sendData(j.dump(), clientSocket);
}

void ClientChat::receiveChatMessage() {
    std::string received;
    while (true) {
        char buffer[12000];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            break;
        }
        buffer[bytesReceived] = '\0';
        received += buffer;
    }
    ChatMessage chatMessage(received);
    displayChatMessage(chatMessage);
}

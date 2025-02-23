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

            mvprintw(0, 0, "[receiver: %s] %s\n", receiver.c_str(), message.c_str());
            refresh();
            //            broadcastMessage(receiver, message);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

void ServerChat::broadcastMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (const auto& client : clients) {
        sendMessage(client.first, message);
    }
}

void ServerChat::sendMessage(int clientSocket, const std::string& message) {
    json msg;
    msg["message"] = message;
    std::string msgStr = msg.dump();
    send(clientSocket, msgStr.c_str(), msgStr.size(), 0);
}

std::string ServerChat::getChatMenu() const {
    return "Vous êtes dans le chat. Tapez votre message et appuyez sur Entrée.\n";
}
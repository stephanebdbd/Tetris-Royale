#include "chat.hpp"

using json = nlohmann::json;


void ServerChat::processClientChat(int clientSocket, std::unordered_map<std::string, int>& pseudoSocket) {
    std::cout << "Chat process started for client " << clientSocket << std::endl;
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
            if (msg["receiver"] != "server" && msg["message"] != "exit") {
                std::cout << "Message reçu de " << clientSocket << " : " << msg["message"] << std::endl;
                // std::string sender = pseudoSocket[msg["sender"]];
                int receiver = pseudoSocket[msg["receiver"]];
                std::string message = msg["message"];
                sendMessage(receiver, "?", message);
                std::cout << "Message envoyé à " << msg["receiver"].get<std::string>() << std::endl;
            } else {
                // gere l exit du client
                return;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

void ServerChat::sendMessage(int clientSocket, std::string sender, const std::string& message) {
    json msg;
    msg["sender"] = sender;
    msg["message"] = message;
    std::string msgStr = msg.dump();
    send(clientSocket, msgStr.c_str(), msgStr.size(), 0);
}

std::string ServerChat::getChatMenu() const {
    return "Vous êtes dans le chat. Tapez votre message et appuyez sur Entrée.\n";
}
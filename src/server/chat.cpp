#include "chat.hpp"
#include "Server.hpp"


using json = nlohmann::json;


void ServerChat::processClientChat(int clientSocket, std::unordered_map<std::string, int>& pseudoSocket, bool &runningChat) {
    std::thread chatThread([this, clientSocket, &pseudoSocket, &runningChat]() {
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
                std::cout <<  msg << std::endl;
                if (msg.contains("receiver") && msg.contains("message") && !msg["receiver"].is_null() && !msg["message"].is_null()) {
                    std::cout << "Message reçu de " << clientSocket << " : " << msg["message"] << std::endl;
                    // std::string sender = pseudoSocket[msg["sender"]];
                    int receiver = pseudoSocket[msg["receiver"]];
                    std::string message = msg["message"];
                    sendMessage(receiver, "?", message);
                } else {
                    // gere l exit du client
                    return;
                    
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
        runningChat = false;
        std::cout << "Chat process ended for client " << clientSocket << std::endl;
    });
    chatThread.detach();
}

void ServerChat::sendMessage(int clientSocket, std::string sender, const std::string& message) {
    json msg;
    msg["sender"] = sender;
    msg["message"] = message;
    std::string msgStr = msg.dump();
    send(clientSocket, msgStr.c_str(), msgStr.size(), 0);
    std::cout << msg << std::endl;
}

std::string ServerChat::getChatMenu() const {
    return "Vous êtes dans le chat. Tapez votre message et appuyez sur Entrée.\n";
}
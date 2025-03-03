#include "chat.hpp"
#include "Server.hpp"


using json = nlohmann::json;


void ServerChat::processClientChat(int clientSocket, int clientId, Server &server, MenuState state, std::string menu) {
    std::thread chatThread([this, clientSocket, clientId, &server, state, menu]() {
        std::cout << "Chat process started for client " << clientSocket << std::endl;
        char buffer[1024];

        while (server.getRunningChat(clientId)) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (bytes_received <= 0) {
                close(clientSocket);
                server.setRunningChat(clientId, false);
            }

            try {
                json msg = json::parse(std::string(buffer, bytes_received));
                std::cout <<  msg << std::endl;
                if (msg.contains("receiver") && msg.contains("message") && !msg["receiver"].is_null() && !msg["message"].is_null() && msg["message"] != "exit") {
                    std::cout << "Message reçu de " << clientSocket << " : " << msg["message"] << std::endl;
                    // std::string sender = pseudoSocket[msg["sender"]];
                    int receiver = server.getPseudoSocket()[msg["receiver"]];
                    std::string sender = server.getSocketPseudo()[clientSocket];
                    for (auto const& [pseudo, socket] : server.getPseudoSocket()) {
                        std::cout << pseudo << " : " << socket << std::endl;
                    }
                    std::string message = msg["message"];
                    sendMessage(receiver, sender, message);
                } else {
                    // gere l exit du client
                    std::cout << "Client " << clientSocket << " disconnected." << std::endl;
                    server.setRunningChat(clientId, false);
                    
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
        server.setClientState(clientId, state);
        server.sendMenuToClient(clientSocket, menu);
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
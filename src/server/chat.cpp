#include "chat.hpp"
#include "Server.hpp"
#include <fstream>


bool ServerChat::messagesWaitForDisplay = false;

void ServerChat::processClientChat(int clientSocket, int clientId, Server &server, MenuState state, std::string menu) {
    std::thread chatThread([this, clientSocket, clientId, &server, state, menu]() {
        std::string sender = server.getSocketPseudo()[clientSocket];
        char buffer[1024];
        while (server.getRunningChat(clientSocket)) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (bytes_received <= 0) {
                close(clientSocket);
                server.setRunningChat(clientSocket, false);
            }

            try {
                json msg = json::parse(std::string(buffer, bytes_received));
                std::cout <<  msg << std::endl;
                if (msg.contains("receiver") && msg.contains("message") && !msg["receiver"].is_null() && !msg["message"].is_null() && msg["message"] != "exit") {
                    int receiver = server.getPseudoSocket()[msg["receiver"]];
                    std::string message = msg["message"];
                    if(!server.getRunningChat(receiver)) {
                        saveMessage(msg["receiver"].get<std::string>() + ".json", msg.dump());
                    }else {
                        sendMessage(receiver, sender, message);
                    }
                }else {
                    // gere l exit du client
                    std::cout << "Client " << clientSocket << " disconnected." << std::endl;
                    server.setRunningChat(clientSocket, false);
                    
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
        server.setClientState(clientId, state);
        server.sendMenuToClient(clientSocket, menu);
    });
    chatThread.detach();
}

void ServerChat::sendMessage(int clientSocket, std::string sender, const std::string& message) {
    json msg;
    msg["sender"] = sender;
    msg["message"] = message;
    std::string msgStr = msg.dump() + "\n";
    send(clientSocket, msgStr.c_str(), msgStr.size(), 0);
}


bool ServerChat::initMessageMemory(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.good()) {
        std::ofstream newFile(filename);
        if (newFile.is_open()) {
            newFile.close();
            return true;
        }
        std::cerr << "Erreur lors de la création du fichier messages.json." << std::endl;
        return false;
    }
    return true;
}

bool ServerChat::saveMessage(const std::string& filename, const std::string& message) {
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << message << std::endl;
        file.close();
        messagesWaitForDisplay = true;
        return true;
    }
    return false;
}

bool ServerChat::FlushMemory(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            json message = json::parse(line);
            std::cout << message.dump(4) << std::endl;
        }
        file.close();
        std::ofstream clearFile(filename, std::ios::trunc);
        messagesWaitForDisplay = false;
        return true;
    }
    return false;
}
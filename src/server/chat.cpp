#include "chat.hpp"
#include "Server.hpp"
#include <fstream>
#define CLIENTS "Clients/"


void ServerChat::processClientChat(int clientSocket, int clientId, Server &server, MenuState state, std::string menu) {
    std::thread chatThread([this, clientSocket, clientId, &server, state, menu]() {
        std::cout << "Chat process started for client " << clientSocket << std::endl;
        char buffer[1024];
        std::string sender = server.getSocketPseudo()[clientSocket];
        while (server.getRunningChat(clientSocket)) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (bytes_received <= 0) {
                close(clientSocket);
                server.setRunningChat(clientSocket, false);
            }

            try {
                json msg = json::parse(std::string(buffer, bytes_received));
                if (msg.contains("receiver") && !msg["receiver"].is_null() && msg.contains("message") && !msg["message"].is_null() && msg["message"] != "exit") {
                    msg["sender"] = sender;

                    //si la room existe
                    if(server.getNameChatRoomIndex().find(msg["receiver"]) != server.getNameChatRoomIndex().end()) {
                        //int chatRoomIndex = server.getNameChatRoomIndex()[msg["receiver"]];
                        //server.getChatRooms()[chatRoomIndex].broadcastMessage(msg["message"], sender, server);
                        continue;
                    }
                    //si le receiver est connecté
                    if(server.getPseudoSocket().find(msg["receiver"]) != server.getPseudoSocket().end()) {
                        int receiver = server.getPseudoSocket()[msg["receiver"]];
                        std::string message = msg["message"];
                        sendMessage(receiver, sender, message, server.getRunningChat(receiver));
                        continue;
                    }else {
                        sendMessage(clientSocket, "Server", "User not found.", server.getRunningChat(clientSocket));
                        if(initMessageMemory(CLIENTS + msg["receiver"].get<std::string>() + ".json"))
                            saveMessage(CLIENTS + msg["receiver"].get<std::string>() + ".json", msg.dump()+ "\n");
                        continue;
                    }
                }else {
                    if(msg["message"] == "exit") {
                        // gere l exit du client
                        std::cout << "Client " << clientSocket << " disconnected." << std::endl;
                        server.setRunningChat(clientSocket, false);
                    }else if(msg["message"] == "flush") {
                        // gere le flush de la memoire
                        FlushMemory(CLIENTS + sender + ".json", server);
                    }else {
                        // gere les messages non conformes
                    }
                    
                    
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

void ServerChat::sendMessage(int clientSocket, std::string sender, const std::string& message, bool isOnline) {
    json msg;
    msg["sender"] = sender;
    msg["message"] = message;
    std::string msgStr = msg.dump() + "\n";
    if(isOnline) {
        send(clientSocket, msgStr.c_str(), msgStr.size(), 0);
    }else {
        saveMessage(CLIENTS + msg["receiver"].get<std::string>() + ".json", msg.dump() + "\n");
    }
}


bool ServerChat::initMessageMemory(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.good()) {
        std::ofstream newFile(filename);
        if (newFile.is_open()) {
            json j;
            j["messages"] = json::array();
            newFile << j.dump(4);
            newFile.close();
            return true;
        }
        std::cerr << "Erreur lors de la création du fichier messages.json." << std::endl;
        return false;
    }
    return true;
}

void ServerChat::saveMessage(const std::string& filename, const std::string& message) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        json j;
        file >> j;
        file.close();

        json msg = json::parse(message);
        j["messages"].push_back(msg);

        std::ofstream outFile(filename, std::ios::trunc);
        if (!outFile.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        outFile << j.dump(4);
        outFile.close();

        std::cout << "Message saved in " << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error writing to file: " << e.what() << std::endl;
    }
}


void ServerChat::FlushMemory(const std::string& filename, Server &server) {
    std::ifstream file(filename);
    if (file.is_open()) {
        json j;
        file >> j;
        file.close();

        for (const auto& message : j["messages"]) {
            int receiver = server.getPseudoSocket()[message["receiver"]];
            sendMessage(receiver, message["sender"], message["message"], server.getRunningChat(receiver));
        }

        j["messages"].clear();

        std::ofstream clearFile(filename, std::ios::trunc);
        if (clearFile.is_open()) {
            clearFile << j.dump(4);
            clearFile.close();
        } else {
            std::cerr << "Error opening file: " << filename << std::endl;
        }
    } else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
}
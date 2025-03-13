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
                    //si le receiver est une room
                    if(server.getChatRooms().find(msg["receiver"]) != server.getChatRooms().end()) {
                        server.getChatRooms()[msg["receiver"]]->broadcastMessage(msg["message"], sender, server);
                        continue;
                    }
                    //si le receiver est un client
                    else if(server.getPseudoSocket().find(msg["receiver"]) != server.getPseudoSocket().end()) {
                        //FlushMemory(CLIENTS + msg["receiver"].get<std::string>() + ".json", server);
                        int receiver = server.getPseudoSocket()[msg["receiver"]];
                        std::string message = msg["message"];
                        sendMessage(receiver, sender, message, server.getRunningChat(receiver));
                        continue;
                    }
                    //si le receiver n'existe pas
                    else {
                        sendMessage(clientSocket, "Server", "User not found.", server.getRunningChat(clientSocket));
                        if(initMessageMemory(CLIENTS + msg["receiver"].get<std::string>() + ".json"))
                            saveMessage(CLIENTS + msg["receiver"].get<std::string>() + ".json", msg.dump()+ "\n");
                        continue;
                    }
                }else {
                    if(msg["message"] == "exit") {
                        // gere l exit du client
                        server.setRunningChat(clientSocket, false);
                    }else if(msg["message"] == "flush") {
                        // gere le flush de la memoire
                        FlushMemory(CLIENTS + sender + ".json", server);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error (sending messages): " << e.what() << std::endl;
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
            j["rooms"] = json::array();
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
        json j = openFile(filename);

        json msg = json::parse(message);
        j["messages"].push_back(msg);

        writeFile(filename, j);

        std::cout << "Message saved in " << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error writing to file: " << e.what() << std::endl;
    }
}


void ServerChat::FlushMemory(const std::string& filename, Server &server) {
    try {
        json j = openFile(filename);

        for (auto& message : j["messages"]) {
            std::cout << message << std::endl;
            int receiver = server.getPseudoSocket()[message["receiver"]];
            sendMessage(receiver, message["sender"], message["message"], server.getRunningChat(receiver));
        }

        j["messages"].clear();
        writeFile
        (filename, j);
    } catch (const std::exception& e) {
        std::cerr << "Error writing to file: " << e.what() << std::endl;
    }
}

std::vector<std::string> ServerChat::getMyRooms(const std::string& pseudo) {
    std::vector<std::string> rooms;
    json j = openFile(CLIENTS + pseudo + ".json");
    rooms = j["rooms"];

    return rooms;
}

#include "chat.hpp"
#include "Server.hpp"
#include <fstream>

#define CLIENTS "Clients/"


void ServerChat::processClientChat(int receiverSock, const std::string& sender, const std::string& receiver, const json& msg, bool isRoom) {
    
        try {

            if (msg["message"] != "/exit") {
                
                // Si le receiver est une room
                if (isRoom) {
                    // server.getChatRooms()[receiver]->broadcastMessage(msg["message"], sender, server);
                }
                // Si le receiver est un client
                else {
                    sendMessage(receiverSock, sender, receiver, msg["message"], true);
                }
            }
            else {
                if (msg["message"] == "/exit") {
                    // Gère l'exit du client
                    
                }
            }
        } 
        catch (const std::exception& e) {
            std::cerr << "Error (sending messages): " << e.what() << std::endl;
        }
}


void ServerChat::sendMessage(int clientSocket, std::string sender, std::string receiver, const std::string& message, bool isOnline) {
    json msg;
    msg["sender"] = sender;
    msg["message"] = message;
    std::string msgStr = msg.dump() + "\n";
    if(isOnline) {
        send(clientSocket, msgStr.c_str(), msgStr.size(), 0);
    }else{
        receiver = receiver;
    }
}


json ServerChat::openFile(const std::string& pseudo) {
    std::string filename = CLIENTS + pseudo + ".json";
    std::ifstream file(filename);
    if (!file.good()) {
        std::cerr << "Error opening file " << filename << std::endl;
        return json();
    }
    json j;
    file >> j;
    file.close();
    return j;
}

void ServerChat::writeFile(const std::string& pseudo, const json& j) {
    std::string filename = CLIENTS + pseudo + ".json";
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        return;
    }
    file << j.dump(4);
    file.close();
}


bool ServerChat::initMessageMemory(const std::string& pseudo) {
    std::string filename = CLIENTS + pseudo + ".json";
    std::ifstream file(filename);
    if (!file.good()) {
        std::ofstream newFile(filename);
        if (newFile.is_open()) {
            json j;
            j["messages"] = json::array();
            j["rooms"] = json::array();
            j["roomRequests"] = json::array();
            newFile << j.dump(4);
            newFile.close();
            return true;
        }
        std::cerr << "Erreur lors de la création du fichier messages.json." << std::endl;
        return false;
    }
    return true;
}

void ServerChat::saveMessage(const std::string& pseudo, const std::string& message) {
    try {
        json j = openFile(pseudo);

        json msg = json::parse(message);
        j["messages"].emplace_back(msg);

        writeFile(pseudo, j);

    } catch (const std::exception& e) {
        std::cerr << "Error writing to file: " << e.what() << std::endl;
    }
}

/*
void ServerChat::FlushMemory(const std::string& pseudo, Server &server) {
    try {
        json j = openFile(pseudo);

        for (auto& message : j["messages"]) {
            std::cout << message << std::endl;
            int receiver = server.getPseudoSocket()[message["receiver"]];
            //sendMessage(receiver, message["sender"], pseudo, message["message"], server.getRunningChat(receiver));
        }

        j["messages"].clear();
        writeFile(pseudo, j);
    } catch (const std::exception& e) {
        std::cerr << "Error writing to file: " << e.what() << std::endl;
    }
}*/

std::vector<std::string> ServerChat::getUserData(const std::string& pseudo, const std::string& dataType) {
    std::vector<std::string> data;
    json j = openFile(pseudo);
    data = j[dataType];

    return data;
}

std::vector<std::string> ServerChat::getMyFriends(const std::string& pseudo) {
    return getUserData(pseudo, "friends");
}

std::vector<std::string> ServerChat::getMyFriendRequests(const std::string& pseudo) {
    return getUserData(pseudo, "friendRequests");
}

std::vector<std::string> ServerChat::getMyRooms(const std::string& pseudo) {
    return getUserData(pseudo, "rooms");
}

std::vector<std::string> ServerChat::getMyRoomRequests(const std::string& pseudo) {
    return getUserData(pseudo, "roomRequests");
}

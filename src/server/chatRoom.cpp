#include "chatRoom.hpp"
#include <algorithm>
#include <fstream>
#include "Server.hpp"


chatRoom::chatRoom(std::string room_name, int admin_id) : roomName(room_name), adminId(admin_id) {
    filename = "chatRooms/" + roomName + ".json";
    init_chatRoom();
}


void chatRoom::init_chatRoom() {
    std::ifstream file(filename);
    if (!file.good()) {
        std::ofstream newFile(filename);
        if(newFile.is_open()) {
            json j;
            j["roomName"] = roomName;
            j["adminId"] = adminId;
            j["clients"] = std::set<std::string>();
            j["receivedReq"] = {};
            j["sentReq"] = {};
            j["messages"] = {};
            newFile << j.dump(4);
            newFile.close();
            return;
        }
    }else {
        json j;
        file >> j;
        adminId = j["adminId"];
        for (const auto& client : j["clients"]) {
            clients.insert(client);
        }
        for (const auto& req : j["receivedReq"]) {
            receivedReq.insert(req);
        }
        for (const auto& req : j["sentReq"]) {
            sentReq.insert(req);
        }
        file.close();
    }
}

void chatRoom::addClient(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.insert(pseudo);


    //ajouter dans le fichier de la room
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    json j;
    file >> j;
    j["clients"].push_back(pseudo);
    file.close();

    //ajouter dans le fichier du client
    std::ifstream file2("Clients/" + pseudo + ".json");
    if (!file2.is_open()) {
        std::cerr << "Error opening file: " << "Clients/" + pseudo + ".json" << std::endl;
        return;
    }
    json j2;
    file2 >> j2;
    j2["rooms"].push_back(roomName);
    file2.close();

}

void chatRoom::removeClient(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(pseudo);  // Corrected line

    //enlever du fichier de la room
    std::ifstream file(filename); 
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    json j;
    file >> j;
    j["clients"].erase(std::remove(j["clients"].begin(), j["clients"].end(), pseudo), j["clients"].end());

    //enlever du fichier du client
    std::ifstream file2("Clients/" + pseudo + ".json");
    if (!file2.is_open()) {
        std::cerr << "Error opening file: " << "Clients/" + pseudo + ".json" << std::endl;
        return;
    }
    json j2;
    file2 >> j2;
    j2["rooms"].erase(std::remove(j2["rooms"].begin(), j2["rooms"].end(), roomName), j2["rooms"].end());
    file2.close();

}

void chatRoom::acceptClientRequest(const std::string& pseudo) {
    addClient(pseudo);
}

void chatRoom::refuseClientRequest(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(requestsMutex);
    receivedReq.erase(pseudo);  // Corrected line
    //enlever du fichier
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    json j;
    file >> j;
    j["receivedReq"].erase(std::remove(j["receivedReq"].begin(), j["receivedReq"].end(), pseudo), j["receivedReq"].end());
    file.close();
}

void chatRoom::broadcastMessage(const std::string& message, const std::string& sender, Server& server) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    ServerChat chat;  // Initialize the chat object
    for (auto& client : clients) {
        // send message to client
        int receiverSocket = server.getPseudoSocket()[client];
        chat.sendMessage(server.getPseudoSocket()[client], sender, message, server.getRunningChat(receiverSocket));
    }   
}

std::string chatRoom::getRoomName() const {
    return roomName;
}

int chatRoom::getAdminId() const {
    return adminId;
}

std::set<std::string> chatRoom::getClients() const {
    return clients;
}

std::set<std::string> chatRoom::getReceivedReq() const {
    return receivedReq;
}

std::set<std::string> chatRoom::getSentReq() const {
    return sentReq;
}
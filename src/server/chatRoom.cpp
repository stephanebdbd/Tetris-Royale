#include "chatRoom.hpp"
#include <algorithm>
#include <fstream>

#include "Server.hpp"


chatRoom::chatRoom(std::string room_name, int admin_id) : filename("chatRooms/" + room_name + ".json"), roomName(room_name), adminId(admin_id) {
    init_chatRoom();
}

void chatRoom::init_chatRoom() {
    std::ifstream file(filename);
    if (!file.good()) {
        // the file does not exist and we need to create it
        std::ofstream newFile(filename);
        if(newFile.is_open()) {
            // create the file and write the data
            json j;
            j["roomName"] = roomName;
            j["adminId"] = adminId;
            j["clients"] = {};
            j["receivedReq"] = {};
            j["sentReq"] = {};
            newFile << j.dump(4);
            newFile.close();
            return;
        }
    }
}

void chatRoom::addClient(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    //ajouter dans le fichier de la room
    saveData(filename, "clients", pseudo); 

    //ajouter dans le fichier du client
    saveData(filename, "rooms", roomName);

}

void chatRoom::removeClient(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    //enlever du fichier de la room
    deleteData(filename, "clients", pseudo);

    //enlever du fichier du client
    deleteData(filename, "rooms", roomName);

}

void chatRoom::acceptClientRequest(const std::string& pseudo) {
    addClient(pseudo);
}

void chatRoom::refuseClientRequest(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(requestsMutex);

    //enlever du fichier
    deleteData(filename, "receivedReq", pseudo);
}


void chatRoom::broadcastMessage(const std::string& message, const std::string& sender, Server& server) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    ServerChat chat;
    for (auto& client : loadData(filename, "clients")) {
        //send message to client
        int receiverSocket = server.getPseudoSocket()[client];
        chat.sendMessage(receiverSocket, sender, message, server.getRunningChat(receiverSocket));
    }   
}

std::string chatRoom::getRoomName() const {
    return roomName;
}

int chatRoom::getAdminId() const {
    return adminId;
}

std::set<std::string> chatRoom::getClients() const {
    return loadData(filename, "clients");
}

std::set<std::string> chatRoom::getReceivedReq() const {
    return loadData(filename, "receivedReq");
}

std::set<std::string> chatRoom::getSentReq() const {
    return loadData(filename, "sentReq");
}
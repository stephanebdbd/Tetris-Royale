#include "chatRoom.hpp"
#include <algorithm>
#include <fstream>

#include "Server.hpp"


chatRoom::chatRoom(std::string room_name, std::string admin_pseudo) : filename("ChatRooms/" + room_name + ".json"), roomName(room_name), adminPseudo(admin_pseudo) {
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
            j["adminPseudo"] = {adminPseudo};
            j["clients"] = {};
            j["receivedReq"] = {};
            j["sentReq"] = {};
            newFile << j.dump(4);
            newFile.close();
            addClient(adminPseudo);
            return;
        }
    }
    
}

bool chatRoom::isInKey(const std::string& key , const std::string& pseudo) const {
    try{
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return false;
        }
        json j;
        file >> j;
        file.close();

        return std::find(j[key].begin(), j[key].end(), pseudo) != j[key].end();
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing to file: " << e.what() << std::endl;
    }
    return false;
}

bool chatRoom::isClient(const std::string& pseudo) const {
    return isInKey("clients", pseudo);
}

bool chatRoom::isAdmin(const std::string& pseudo) const {
    return isInKey("adminPseudo", pseudo);
}

void chatRoom::addAdmin(const std::string& pseudo) {
    //saveData(filename, "adminPseudo", pseudo);
    try{
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        json j;
        file >> j;
        file.close();

        if(j["Clients"].find(pseudo) == j["Clients"].end()){
            std::cerr << "Error: " << pseudo << " is not a client of the room" << std::endl;
            return;
        }
        j["adminPseudo"].push_back(pseudo);

        std::ofstream file2(filename);
        file2 << j.dump(4);
        file2.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing to file: " << e.what() << std::endl;
    }
}

void chatRoom::addClient(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    saveData(filename, "clients", pseudo);
    saveData("Clients/" + pseudo + ".json", "rooms", roomName);
}

void chatRoom::removeClient(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    deleteData(filename, "clients", pseudo);
    deleteData("Clients/" + pseudo + ".json", "rooms", roomName);

}

void chatRoom::addReceivedRequest(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(requestsMutex);
    saveData(filename, "receivedReq", pseudo);
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
        if (client != sender) {
            int receiverSocket = server.getPseudoSocket()[client];
            chat.sendMessage(receiverSocket, sender, message, server.getRunningChat(receiverSocket));
        }
    }
}

std::string chatRoom::getRoomName() const {
    return roomName;
}

std::set<std::string> chatRoom::getadminPseudo() const {
    return loadData(filename, "adminPseudo");
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

void chatRoom::saveData(const std::string& filename, const std::string& key, const std::string& value) {

    try{
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        json j;
        file >> j;
        file.close();

        if (j.find(key) == j.end()) {
            j[key] = json::array();
        }

        j[key].push_back(value);

        std::ofstream file2(filename);
        file2 << j.dump(4);
        file2.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing to file: " << e.what() << std::endl;
    }
}

std::set<std::string> chatRoom::loadData(const std::string& filename, const std::string& key) const {
    std::set<std::string> data;
    try{
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return data;
        }
        json j;
        file >> j;
        file.close();

        return std::set<std::string>(j[key].begin(), j[key].end());
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing to file: " << e.what() << std::endl;
    }
    return data;
}

void chatRoom::deleteData(const std::string& filename, const std::string& key, const std::string& value) {

    try{
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        json j;
        file >> j;
        file.close();

        if (j.find(key) == j.end()) {
            std::cerr << "Error: Key " << key << " does not exist in the file" << std::endl;
            return;
        }

        j[key].erase(std::remove(j[key].begin(), j[key].end(), value), j[key].end());

        std::ofstream file2(filename);
        file2 << j.dump(4);
        file2.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing to file: " << e.what() << std::endl;
    }
}
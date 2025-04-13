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
        std::ofstream newFile(filename);
        if(newFile.is_open()) {
            // create the file and write the data
            json j;
            j["roomName"] = roomName;
            j["adminPseudo"] = {adminPseudo};
            j["clients"] = {};
            j["receivedReq"] = json::array();
            j["sentReq"] = json::array();
            newFile << j.dump(4);
            newFile.close();
            addClient(adminPseudo);
            return;
        }
    }
    
}

/*bool chatRoom::isInKey(const std::string& key , const std::string& pseudo) const {
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
    }catch (const std::exception& e) {
        std::cerr << "Error writing to file: " << e.what() << std::endl;
    }
    return false;
}
*/


bool chatRoom::isClient(const std::string& pseudo) const {
    return isInKey(filename,"clients", pseudo);
}

bool chatRoom::isAdmin(const std::string& pseudo) const {
    return isInKey(filename,"adminPseudo", pseudo);
}

bool chatRoom::isInReceivedReq(const std::string& pseudo) const {
    return isInKey(filename,"receivedReq", pseudo);
}

void chatRoom::addAdmin(const std::string& pseudo) {
    saveData(filename, "adminPseudo", pseudo);
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
void chatRoom::removeAdmin(const std::string& pseudo) {
    deleteData(filename, "adminPseudo", pseudo);
    deleteData("Clients/" + pseudo + ".json", "rooms", roomName);
}


void chatRoom::addReceivedRequest(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(requestsMutex);
    saveData(filename, "receivedReq", pseudo);
}

void chatRoom::acceptClientRequest(const std::string& pseudo) {
    std::lock_guard<std::mutex> lock(requestsMutex);
    //ajouter au fichier
    addClient(pseudo);
    deleteData(filename, "receivedReq", pseudo);
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
        std::cout << "Sending message to " << client << std::endl;
        //send message to client
        if (client != sender) {
            int receiverSocket = server.getPseudoSocket()[client];
            std::cout << "Receiver socket: " << message << receiverSocket << std::endl;
            //sendMessage(receiverSocket, sender, client, message, true);
        }
    }
}

std::string chatRoom::getRoomName() const {
    return roomName;
}

std::vector<std::string> chatRoom::getadminPseudo() const {
    return loadData(filename, "adminPseudo");
}

std::vector<std::string> chatRoom::getClients() const {
    return loadData(filename, "clients");
}

std::vector<std::string> chatRoom::getReceivedReq() const {
    return loadData(filename, "receivedReq");
}

std::vector<std::string> chatRoom::getSentReq() const {
    return loadData(filename, "sentReq");
}

void chatRoom::deleteRoomFile() {
    for(auto& client : loadData(filename, "clients")) {
        removeClient(client);
    }
    if (std::filesystem::exists(filename)) {
        std::filesystem::remove(filename);
        std::cout << "File " << filename << " deleted." << std::endl;
    } else {
        std::cerr << "File " << filename << " not found." << std::endl;
    }
}

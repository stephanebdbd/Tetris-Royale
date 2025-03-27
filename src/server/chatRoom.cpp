#include "chatRoom.hpp"
#include <algorithm>
#include <fstream>

#include "Server.hpp"


chatRoom::chatRoom(std::string room_name, std::string prop_pseudo) : filename("ChatRooms/" + room_name + ".json"), roomName(room_name){
    init_chatRoom(prop_pseudo);
}

void chatRoom::init_chatRoom(std::string propPseudo) {
    
    std::ifstream file(filename);
    if (!file.good()) {
        std::ofstream newFile(filename);
        if(newFile.is_open()) {
            // create the file and write the data
            json j;
            j["roomName"] = roomName;
            j["proprietaire"] = propPseudo;
            j["adminPseudo"] = {propPseudo};
            j["clients"] = {};
            j["receivedReq"] = json::array();
            j["sentReq"] = json::array();
            newFile << j.dump(4);
            newFile.close();
            addClient(propPseudo);
            return;
        }
    }
    
}

//verifier si un client est dans la salle
bool chatRoom::isClient(const std::string& pseudo) const {
    return isInKey(filename,"clients", pseudo);
}

//verifier si un client est admin
bool chatRoom::isAdmin(const std::string& pseudo) const {
    return isInKey(filename,"adminPseudo", pseudo);
}

//verifier si un client a deja envoyé une demande
bool chatRoom::isInReceivedReq(const std::string& pseudo) const {
    return isInKey(filename,"receivedReq", pseudo);
}

//verifier si un client est un admin
void chatRoom::addAdmin(const std::string& pseudo) const {
    saveData(filename, "adminPseudo", pseudo);
}

//supprimer un admin
void chatRoom::removeAdmin(const std::string& pseudo) const {
    deleteData(filename, "adminPseudo", pseudo);
}

//ajouter un client
void chatRoom::addClient(const std::string& pseudo) const {
    saveData(filename, "clients", pseudo);
    saveData("Clients/" + pseudo + ".json", "rooms", roomName);
}

//supprimer un client
void chatRoom::removeClient(const std::string& pseudo) const {
    deleteData(filename, "clients", pseudo);
    deleteData("Clients/" + pseudo + ".json", "rooms", roomName);

}

//envoyer une invitation pour rejoindre la salle
void chatRoom::sendRequestToClient(const std::string& pseudo) const {
    saveData(filename, "sentReq", pseudo);
    saveData("Clients/" + pseudo + ".json", "roomRequests", roomName);
}

//ajouter une demande reçue
void chatRoom::addReceivedRequest(const std::string& pseudo) const {
    saveData(filename, "receivedReq", pseudo);
}   

//accepter une demande
void chatRoom::acceptClientRequest(const std::string& pseudo) const {
    //ajouter au fichier
    addClient(pseudo);
    deleteData(filename, "receivedReq", pseudo);
}


//refuser une demande
void chatRoom::refuseClientRequest(const std::string& pseudo) const {
    //enlever du fichier
    deleteData(filename, "receivedReq", pseudo);
}

void chatRoom::invitationResponse(const std::string& pseudo, bool accepted) const {
    if (accepted) {
        addClient(pseudo);
    }
    deleteData(filename, "sentReq", pseudo);
    deleteData("Clients/" + pseudo + ".json", "roomRequests", roomName);
}

//envoyer un message à tous les clients de la salle
void chatRoom::broadcastMessage(const std::string& message, const std::string& sender, Server& server) {
    ServerChat chat;
    for (auto& client : loadData(filename, "clients")) {
        std::cout << "Sending message to " << client << std::endl;
        //send message to client
        if (client != sender) {
            int receiverSocket = server.getPseudoSocket()[client];
            chat.sendMessage(receiverSocket, sender, client, message, server.getRunningChat(receiverSocket));
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

//supprimer la salle
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

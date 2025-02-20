#include "ClientChat.hpp"

using json = nlohmann::json;



void ClientChat::sendChatMessage(const std::string& message) {
    json j;
    j["type"] = "chat";
    j["message"] = message;
    network.sendData(j.dump(), clientSocket);
}

void ClientChat::receiveChatMessage() {
    std::string received;
    while (true) {
        char buffer[12000];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            break;
        }
        buffer[bytesReceived] = '\0';
        received += buffer;
        try {
            json j = json::parse(received);
            if (j.find("type") != j.end() && j["type"] == "chat") {
                displayChatMessage(j["message"]);
                received.clear();
            }
        } catch (const json::parse_error& e) {
            // Handle JSON parsing error
        }
    }
}

void ClientChat::displayChatMessage(const std::string& message){}

bool ClientChat::initMessageMemory() {
    
    std::ifstream file("messages.json");
    if (!file.good()) { // Vérifie si le fichier existe
        std::ofstream newFile("messages.json");
        if (newFile.is_open()) {
            newFile << ""; // Fichier vide (on ajoute ligne par ligne)
            newFile.close();
            return true;
        }
        return false;
    }
}

bool ClientChat::saveMessage(const std::string& message) {

    std::ofstream file("messages.json", std::ios::app); // Mode append
    if (file.is_open()) {
        file << message << std::endl; // Écrire JSON en une seule ligne
        file.close();
        return true;
    }
    return false;

}

bool ClientChat::FlushMemory() {

    std::ifstream file("messages.json");
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) { // Lire ligne par ligne
            json message = json::parse(line);
            std::cout << "📜 Message extrait : " << message.dump(4) << std::endl;
        }
        file.close();

        // Nettoyer le fichier après extraction
        std::ofstream clearFile("messages.json", std::ios::trunc);
        return true;

    }
    return false;

}
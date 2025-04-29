#include "MenuState.hpp"
#include "json.hpp"
#include <string>
#include <iostream>
#include <sys/socket.h>
#include "jsonKeys.hpp"
// Serialize
std::string MenuStateManager::serialize(const MenuState& state) {
    return std::to_string(static_cast<int>(state));
}

// Deserialize
MenuState MenuStateManager::deserialize(const std::string& data) {
    return static_cast<MenuState>(std::stoi(data));
}

// Send MenuState to Client
void MenuStateManager::sendMenuStateToClient(int clientSocket, const MenuState& state, const std::string& message, const std::vector<std::string>& data, const std::vector<std::pair<std::string, int>>& dataPair, const std::map<std::string, std::vector<std::string>>& secondData) {
    json jsonData;
    jsonData["state"] = serialize(state);
    jsonData["message"] = message;
    jsonData["data"] = data;
    jsonData["dataPair"] = dataPair;
    jsonData["secondData"] = secondData;
    std::string serializedState = jsonData.dump() + "\n";
    
    // Add error handling for send
    if (send(clientSocket, serializedState.c_str(), serializedState.size(), 0) == -1) {
        std::cerr << "Failed to send menu state to client" << std::endl;
    }
    std::cout << "MenuState sent to client: " << serializedState;   
}
//send affichage temporaire
void MenuStateManager::sendTemporaryDisplay(int clientSocket, const std::string& message) {
    json jsonData;
    jsonData[jsonKeys::TEMPORARY_DISPLAY] = message;
    std::string serializedState = jsonData.dump() + "\n";
    
    // Add error handling for send
    if (send(clientSocket, serializedState.c_str(), serializedState.size(), 0) == -1) {
        std::cerr << "Failed to send temporary display to client" << std::endl;
    }

}
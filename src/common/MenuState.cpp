#include "MenuState.hpp"
#include "json.hpp"
#include <string>
#include <iostream>
#include <sys/socket.h>

// Serialize
std::string MenuStateManager::serialize(const MenuState& state) {
    return std::to_string(static_cast<int>(state));
}

// Deserialize
MenuState MenuStateManager::deserialize(const std::string& data) {
    return static_cast<MenuState>(std::stoi(data));
}

// Send MenuState to Client
void MenuStateManager::sendMenuStateToClient(int clientSocket, const MenuState& state, const std::string& message, const std::vector<std::string>& data) {
    json jsonData;
    jsonData["state"] = serialize(state);
    jsonData["message"] = message;
    jsonData["data"] = data;
    std::string serializedState = jsonData.dump() + "\n";
    
    // Add error handling for send
    if (send(clientSocket, serializedState.c_str(), serializedState.size(), 0) == -1) {
        std::cerr << "Failed to send menu state to client" << std::endl;
    }
    std::cout << "MenuState sent to client: " << serializedState;   
}
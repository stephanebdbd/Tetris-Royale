#include "MenuState.hpp"
#include "json.hpp"
#include <string>
#include <iostream>
#include <sys/socket.h>
#include "jsonKeys.hpp"

// Sérialiser
std::string MenuStateManager::serialize(const MenuState& state) {
    return std::to_string(static_cast<int>(state));
}

// Désérialiser
MenuState MenuStateManager::deserialize(const std::string& data) {
    return static_cast<MenuState>(std::stoi(data));
}

// Envoyer l'état du menu au client
void MenuStateManager::sendMenuStateToClient(int clientSocket, const MenuState& state, const std::string& message, const std::vector<std::string>& data, const std::vector<std::pair<std::string, int>>& dataPair, const std::map<std::string, std::vector<std::string>>& secondData) {
    json jsonData;
    jsonData["state"] = serialize(state);
    jsonData["message"] = message;
    jsonData["data"] = data;
    jsonData["dataPair"] = dataPair;
    jsonData["secondData"] = secondData;
    std::string serializedState = jsonData.dump() + "\n";
    
    // Ajouter une gestion d'erreur pour l'envoi
    if (send(clientSocket, serializedState.c_str(), serializedState.size(), 0) == -1) {
        std::cerr << "Échec de l'envoi de l'état du menu au client" << std::endl;
    }
}

// Envoyer un affichage temporaire
void MenuStateManager::sendTemporaryDisplay(int clientSocket, const std::string& message) {
    json jsonData;
    jsonData[jsonKeys::TEMPORARY_DISPLAY] = message;
    std::string serializedState = jsonData.dump() + "\n";
    
    // Ajouter une gestion d'erreur pour l'envoi
    if (send(clientSocket, serializedState.c_str(), serializedState.size(), 0) == -1) {
        std::cerr << "Échec de l'envoi de l'affichage temporaire au client" << std::endl;
    }
}

#include "Controller.hpp"

#include "../common/json.hpp"
#include <netinet/in.h>
#include <iostream>

void Controller::sendInput(const std::string& action, int clientSocket) {
    if (clientSocket == -1) return;

    json message;
    message["action"] = action;
    std::string msg = message.dump();

    if (send(clientSocket, msg.c_str(), msg.size(), 0) == -1) {
        std::cerr << "Erreur: Impossible d'envoyer le message." << std::endl;
    }
}
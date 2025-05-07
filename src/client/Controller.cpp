#include "Controller.hpp"

#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"
#include <netinet/in.h>
#include <iostream>

void Controller::sendInput(const std::string& action, int clientSocket) {
    if (clientSocket == -1) return;

    json message;
    message[jsonKeys::ACTION] = action;
    std::string msg = message.dump()+"\n";
    if (send(clientSocket, msg.c_str(), msg.size(), 0) == -1) {
        std::cerr << "Erreur: Impossible d'envoyer le message." << std::endl;
    }
}
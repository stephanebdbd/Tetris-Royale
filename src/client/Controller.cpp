#include "Controller.hpp"


void Controller::sendInput(const std::string& action, int clientSocket) {
    if (clientSocket == -1) return;

    json message;
    message["action"] = action;
    std::string msg = message.dump();

    std::cout << "Envoi au serveur: " << msg << std::endl;

    if (send(clientSocket, msg.c_str(), msg.size(), 0) == -1) {
        std::cerr << "Erreur: Impossible d'envoyer le message." << std::endl;
    }
}
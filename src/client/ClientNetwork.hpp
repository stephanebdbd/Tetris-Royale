#ifndef CLIENTNETWORK_HPP
#define CLIENTNETWORK_HPP

#include "ClientNetwork.hpp"

#include <string>

class ClientNetwork {

    public:
        bool connectToServer(const std::string& serverIP, int port, int& clientSocket);
        void disconnect(int& clientSocket);       
};

#endif
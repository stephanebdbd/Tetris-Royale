#ifndef CLIENTNETWORK_HPP
#define CLIENTNETWORK_HPP


#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include "ClientNetwork.hpp"
#include "../common/json.hpp"

class ClientNetwork {

    public:
        bool connectToServer(const std::string& serverIP, int port, int& clientSocket);
        void disconnect(int& clientSocket);       
};

#endif

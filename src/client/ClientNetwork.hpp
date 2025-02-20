#ifndef CLIENTNETWORK_HPP
#define CLIENTNETWORK_HPP

#include "ClientNetwork.hpp"
#include <sys/socket.h>
#include <unistd.h>

#include <string>

class ClientNetwork {

    public:
        bool connectToServer(const std::string& serverIP, int port, int& clientSocket);
        void disconnect(int& clientSocket);       
        void sendData(const std::string& data, int clientSocket);
        void receivedData(int clientSocket, std::string& received);
};


#endif
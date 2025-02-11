#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class Client {
    std::string serverIP;
    int port;
    int clientSocket;

public:
    Client(const std::string& serverIP, int port);

    bool connectToServer();
    void sendInput(const std::string& action);
    void disconnect();
    void run();
};

#endif

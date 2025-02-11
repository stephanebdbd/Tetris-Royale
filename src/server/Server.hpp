#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <atomic>
#include <thread>

class Server {
    int port;
    int serverSocket;
    std::atomic<int> clientIdCounter;

public:
    Server(int port);

    bool start();
    void acceptClients();
    void handleClient(int clientSocket, int clientId);
    void stop();
};

#endif

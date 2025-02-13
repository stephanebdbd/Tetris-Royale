#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"
#include "Menu.hpp"
#include <netinet/in.h>
#include <atomic>
#include <thread>

class Server {
    int port;
    int serverSocket;
    std::atomic<int> clientIdCounter;
    Game* game;

    public:
        Server(int port, Game* game);
    
        bool start();
        void acceptClients();
        void handleClient(int clientSocket, int clientId);
        void stop();
        void sendMenuToClient(int clientSocket, const std::string& screen);
};

#endif
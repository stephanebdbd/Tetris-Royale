#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"
#include "Grid.hpp"
#include "Menu.hpp"
#include <netinet/in.h>
#include <atomic>
#include <thread>

class Server {
    int port;
    int serverSocket;
    std::unordered_map<int, int> clientMenuChoices;
    bool runningGame = false;
    std::atomic<int> clientIdCounter;
    Game* game;
    Grid grid;

    public:
        Server(int port, Game* game, Grid grid);
    
        bool start();
        void acceptClients();
        void handleClient(int clientSocket, int clientId);
        void stop();
        void sendMenuToClient(int clientSocket, const std::string& screen);
        void sendGameToClient(int clientSocket, const std::string& screen);
        void keyInuptWelcomeMenu(int clientSocket, int clientId, const std::string& action);
        void keyInuptMainMenu(int clientSocket, int clientId, const std::string& action);
        void keyInuptGameMenu(int clientSocket, int clientId, const std::string& action);

        bool getRunningGame() { return runningGame; }
};

#endif
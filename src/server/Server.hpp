#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"
#include "Grid.hpp"
#include "Tetramino.hpp"
#include <atomic>
#include <thread>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <fstream>
#include "../common/json.hpp"

using json = nlohmann::json;


class Server {
    int port;
    int serverSocket;
    std::unordered_map<int, int> clientMenuChoices;
    bool runningGame = false;
    std::atomic<int> clientIdCounter;
    Game* game;
    Grid grid;
    Tetramino currentPiece;

    public:
        Server(int port, Game* game, Grid grid, Tetramino tetramino);
    
        bool start();
        void acceptClients();
        void handleClient(int clientSocket, int clientId);
        void stop();
        void sendMenuToClient(int clientSocket, const std::string& screen);
        void sendGameToClient(int clientSocket, const std::string& screen);
        void keyInuptWelcomeMenu(int clientSocket, int clientId, const std::string& action);
        void keyInuptMainMenu(int clientSocket, int clientId, const std::string& action);
        void keyInuptGameMenu(int clientSocket, int clientId, const std::string& action);
};

#endif
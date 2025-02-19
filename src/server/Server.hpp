#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"
#include "Grid.hpp"
#include "Tetramino.hpp"
#include <atomic>
#include <csignal>
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
    std::unordered_map<int, MenuNode> clientMenuChoices;
    std::atomic<bool> runningGame{false};
    std::atomic<int> clientIdCounter;
    std::unique_ptr<Game> game;
    std::unique_ptr<Grid> grid;
    std::unique_ptr<Tetramino> currentPiece;

    std::unordered_map<std::string, std::string> unicodeToText = {
        {"\u0005", "right"},
        {"\u0004", "left"},
        {"\u0003", "up"},
        {"\u0002", "down"},
        {" ", "drop"}
    };


    public:
        Server(int port, Game* game);
    
        bool start();
        void acceptClients();
        void handleClient(int clientSocket, int clientId);
        void stop();
        void sendMenuToClient(int clientSocket, const std::string& screen);
        void sendGameToClient(int clientSocket);
        void keyInuptWelcomeMenu(int clientSocket, int clientId, const std::string& action);
        void keyInuptMainMenu(int clientSocket, int clientId, const std::string& action);
        void keyInuptGameMenu(int clientSocket, const std::string& action);
        void loopGame(int clientSocket);
        void receiveInputFromClient(int clientSocket, int clientId);
        void handleMenu(int clientSocket, int clientId, const std::string& action);
        std::string convertUnicodeToText(const std::string& unicode);
        void createMenuTree(MenuNode root);
};

#endif
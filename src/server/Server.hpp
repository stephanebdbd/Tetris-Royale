#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"
#include "Grid.hpp"
#include "Tetramino.hpp"
#include <atomic>



class Server {
    int port;
    int serverSocket;
    std::unordered_map<int, int> clientMenuChoices;
    bool runningGame = false;
    std::atomic<int> clientIdCounter;
    Game* game;
    Grid* grid;
    Tetramino* currentPiece;

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
};

#endif
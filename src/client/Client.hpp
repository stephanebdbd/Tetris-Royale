#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../common/json.hpp"

class Client {
    std::string serverIP;
    int port;
    int clientSocket;
    bool runningGame = false;

public:
    Client(const std::string& serverIP, int port);

    bool connectToServer();
    void sendInput(const std::string& action);
    void disconnect();
    void run();
    void receiveAndDisplayMenu();
    void displayMenu(const nlohmann::json& data);
    void displayGrid(const nlohmann::json& data);
    void drawGrid(int width, int height, const nlohmann::json& cells);
};

#endif

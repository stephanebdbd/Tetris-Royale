#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ClientDisplay.hpp"  
#include "Controller.hpp"
#include "ClientNetwork.hpp"
#include "ClientChat.hpp"


class Client {
    ClientDisplay display;  // Chaque client a son propre display
    Controller controller;  // Chaque client a son propre controller
    ClientNetwork network;  // Chaque client a son propre network
    ClientChat chat;        // Chaque client a son propre chat


    std::string serverIP;
    int port;
    int clientSocket;

    public:
        Client(const std::string& serverIP, int port);
        void run();
        void receiveDisplay();
        void handleUserInput();
        void displayMenu(const json& data);

};

#endif

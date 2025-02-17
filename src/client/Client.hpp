#ifndef CLIENTDISPLAY_HPP
#define CLIENTDISPLAY_HPP

#include <iostream>
#include <unistd.h>
#include <ncurses.h>
#include "ClientDisplay.hpp"  
#include "Controller.hpp"
#include "User.hpp
#include "ClientNetwork.hpp"
#include "../common/json.hpp"

class Client {
    ClientDisplay display;  // Chaque client a son propre display
    Controller controller;  // Chaque client a son propre controller
    ClientNetwork network;  // Chaque client a son propre network
    User user; // Chaque client a son propre user
    std::string serverIP;
    int port;
    int clientSocket;

    public:
        Client(const std::string& serverIP, int port);
        void run();
        void receiveDisplay();
};

#endif

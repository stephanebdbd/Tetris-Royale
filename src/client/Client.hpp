#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ClientDisplay.hpp"  
#include "Controller.hpp"
#include "ClientNetwork.hpp"
#include "ClientChat.hpp"
#include "../common/MenuState.hpp"
#include <ostream>


class Client {

private:
    ClientDisplay display;  // Chaque client a son propre display
    Controller controller;  // Chaque client a son propre controller
    ClientNetwork network;  // Chaque client a son propre network
    ClientChat chat;        // Chaque client a son propre chat
    MenuStateManager menuStateManager; // Chaque client a son propre gestionnaire d'état de menu


    std::string serverIP;               // Adresse IP du serveur
    int port;                           // Port du serveur
    int clientSocket;                   // Socket du client
    bool isPlaying = false;             // Indique si le client est en mode jeu
    bool chatMode = false;                  // Indique si le client est en mode chat
    MenuState currentMenuState;            // État du menu actuel
    json serverData;                // Données reçues du serveur
    bool stop_threads = false; // Indique si les threads doit s'arrêter


public:
        Client(const std::string& serverIP, int port);
        void run();
        bool connect();
        void receiveDisplay();
        void handleUserInput();
        void displayMenu(const json& data);
        int getClientSocket() const { return clientSocket; }
        void stopThreads() { stop_threads = true; }
        MenuState getCurrentMenuState();
        json getServerData() const { return serverData; }

};

#endif

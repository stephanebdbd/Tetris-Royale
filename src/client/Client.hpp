#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ClientDisplay.hpp"  
#include "Controller.hpp"
#include "ClientNetwork.hpp"


class Client {
private:
    int sock;
    struct sockaddr_in serverAddr;
    bool isConnected;
    std::shared_ptr<User> user;  // Association avec l'utilisateur

public:
    // Constructeur : prend un shared_ptr<User>, l'IP du serveur, et le port
    Client(std::shared_ptr<User> user, const std::string& serverIP, int port);
    // Processus de connexion : demander les informations à l'utilisateur et envoyer au serveur
    bool login();
    // Méthode pour envoyer un message au serveur avec le pseudonyme de l'utilisateur
    void sendMessage(const std::string& message);

    public:
        Client(const std::string& serverIP, int port);
        void run();
        void receiveDisplay();
        void handleUserInput();
        void displayMenu(const json& data);

};

#endif

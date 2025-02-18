#pragma once
#include <string>
#include <memory>
#include "../server/User.hpp"

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

    // Méthode pour écouter les messages reçus du serveur
    void receiveMessages();

    // Méthode pour commencer l'écoute des messages dans un thread séparé
    void startListening();

    ~Client();
};


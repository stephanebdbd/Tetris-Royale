#include "Client.hpp"
#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>

// Constructeur : initialisation du client et connexion au serveur
Client::Client(std::shared_ptr<User> user, const std::string& serverIP, int port)
    : user(user), isConnected(false) {

    // Création du socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Erreur: Impossible de créer le socket\n";
        exit(1);
    }

    // Configuration de l'adresse du serveur
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    // Connexion au serveur
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Erreur: Impossible de se connecter au serveur\n";
        exit(1);
    }
    isConnected = true;
    std::cout << "✅ Connecté au serveur !\n";
}
bool Client::login() {
    std::string userName, pseudonym, password;

    // Demander les informations
    std::cout << "Nom d'utilisateur: ";
    std::getline(std::cin, userName);
    std::cout << "Pseudonyme: ";
    std::getline(std::cin, pseudonym);
    std::cout << "🔒 Mot de passe: ";
    std::getline(std::cin, password);

    // Créer l'objet User avec les informations fournies
    user = std::make_shared<User>(userName, pseudonym, password);

    // Envoyer les infos d'authentification au serveur
    std::string authMessage = "LOGIN " + userName + " " + pseudonym + " " + password;
    send(sock, authMessage.c_str(), authMessage.size(), 0);

    // Attendre la réponse du serveur
    char buffer[256];
    int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::string response(buffer);
        if (response == "SUCCESS") {
            std::cout << "✅ Connexion réussie !\n";
            return true;
        } else {
            std::cout << "❌ Échec de l'authentification !\n";
            return false;
        }
    }

    return false;
}
// Envoi de message au serveur en préfixant avec le pseudonyme de l'utilisateur
void Client::sendMessage(const std::string& message) {
    std::string fullMessage = user->getPseudonym() + ": " + message;
    send(sock, fullMessage.c_str(), fullMessage.size(), 0);
}

// Réception des messages du serveur
void Client::receiveMessages() {
    char buffer[1024];
    while (isConnected) {
        int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "\n📩 Nouveau message: " << buffer << "\n";
            fflush(stdout);
        }
    }
}

// Démarrer un thread pour écouter les messages entrants
void Client::startListening() {
    std::thread listener(&Client::receiveMessages, this);
    listener.detach();  // Lancer l'écoute en arrière-plan
}

// Destructeur pour fermer la connexion au serveur
Client::~Client() {
    close(sock);
}

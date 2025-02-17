#include "Client.hpp"

#include "../common/json.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <ncurses.h>
#include <thread>

using json = nlohmann::json;

Client::Client(const std::string& serverIP, int port) : serverIP(serverIP), port(port), clientSocket(-1) {}

void Client::run() {
    if (!network.connectToServer(serverIP, port, clientSocket)) {
        std::cerr << "Erreur: Impossible de se connecter au serveur." << std::endl;
        return;
    }
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); // Permet de ne pas bloquer l'affichage en attendant un input

    // Lancer un thread pour écouter les touches et envoyer les inputs
    std::thread inputThread(&Client::handleUserInput, this);
    inputThread.detach(); // Permet au thread de fonctionner indépendamment

    // Boucle principale pour recevoir et afficher le jeu
    while (true) {
        receiveDisplay();
    }

    network.disconnect(clientSocket);
    delwin(stdscr);
    endwin();
}

void Client::handleUserInput() {
    while (true) {
        int ch = getch();
        if (ch != ERR) {  // Si une touche est pressée
            if (ch == 'q') {
                network.disconnect(clientSocket);
                endwin();
                exit(0); 
            }
            std::string action(1, ch);
            controller.sendInput(action, clientSocket);  // Envoyer au serveur
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Pause pour éviter une boucle trop rapide
    }
}

void Client::receiveDisplay() {
    char buffer[15000];
    
    memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        std::cerr << "Erreur : Impossible de recevoir l'affichage du serveur." << std::endl;
        return;
    }
    refresh();
    
    try {
        json data = json::parse(buffer);

        if (data.contains("grid")) {   
            display.displayGame(data);
            //char choice = getch(); trouver un moyen de bouger et pas de freeze en attendant l'input
            //controller.sendInput(std::string(1, choice), clientSocket);
            receiveDisplay();
        }
        else {
            display.displayMenu(data);
        }
    }
    catch (json::parse_error& e) {
        std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
    }
        
}

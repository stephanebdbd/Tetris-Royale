#include "Client.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <ncurses.h>
#include "../common/json.hpp"

using json = nlohmann::json;

Client::Client(const std::string& serverIP, int port) : serverIP(serverIP), port(port), clientSocket(-1) {}

bool Client::connectToServer() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Erreur: Impossible de créer le socket client." << std::endl;
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Erreur: Adresse IP invalide." << std::endl;
        close(clientSocket);
        return false;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Erreur: Connexion au serveur échouée." << std::endl;
        close(clientSocket);
        return false;
    }
    std::cout << "Connecté au serveur." << std::endl;
    return true;
}

void Client::sendInput(const std::string& action) {
    if (clientSocket == -1) return;

    json message;
    message["action"] = action;
    std::string msg = message.dump();

    std::cout << "Envoi au serveur: " << msg << std::endl;

    if (send(clientSocket, msg.c_str(), msg.size(), 0) == -1) {
        std::cerr << "Erreur: Impossible d'envoyer le message." << std::endl;
    }
}

void Client::disconnect() {
    if (clientSocket != -1) {
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
        clientSocket = -1;
    }
}

void Client::run() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    std::string choice;
    while (true) {
        // Affichage de la première page
        clear();
        mvprintw(5, 10, "Bienvenue dans Tetris Royal !");
        mvprintw(7, 10, "1. Se connecter");
        mvprintw(8, 10, "2. Créer un compte");
        mvprintw(9, 10, "3. Quitter");
        mvprintw(11, 10, "Votre choix: ");
        refresh();

        char input[10];
        echo();
        getstr(input);
        noecho();

        choice = std::string(input);

        if (choice == "1") {
            break; // On passe à l'étape suivante
        } else if (choice == "3") {
            disconnect();
            endwin();
            return;
        }
    }

    while (true) {
        // Affichage de la page se connecter
        clear();
        mvprintw(5, 10, "Menu principal");
        mvprintw(7, 10, "1. Jouer");
        mvprintw(8, 10, "2. Amis");
        mvprintw(9, 10, "3. Classements");
        mvprintw(10, 10, "4. Rejoindre");
        mvprintw(11, 10, "5. Retour");
        mvprintw(13, 10, "Votre choix: ");
        refresh();

        char input[10];
        echo();
        getstr(input);
        noecho();

        choice = std::string(input);

        // Pour plus tard, appeler les fonctions run() de chaque page. Exemple: choice == 2 => friendsRun();
        if (choice == "1") {
            break; // On passe à la boucle de jeu
        } else if (choice == "5") {
            return run(); // Retourner au menu principal
        }
    }

    clear();
    mvprintw(5, 10, "GamePlay du jeu... lol");
    refresh();

    keypad(stdscr, TRUE);
    timeout(0);
    nodelay(stdscr, TRUE);

    int ch;
    while (true) {
        ch = getch();
        if (ch == ERR) {
            continue;
        }
        std::string action;

        if (ch == KEY_RIGHT) {
            action = "right";
        } else if (ch == KEY_LEFT) {
            action = "left";
        } else if (ch == KEY_UP) {
            action = "up";
        } else if (ch == KEY_DOWN) {
            action = "down";
        } else if (ch == 'q') {
            action = "quit";
        }

        if (!action.empty()) {
            sendInput(action);
        }

        if (action == "quit") {
            break;
        }
    }

    endwin();
}


int main() {
    Client client("127.0.0.1", 12345);

    if (!client.connectToServer()) {
        return 1;
    }

    client.run();
    client.disconnect();
    return 0;
}

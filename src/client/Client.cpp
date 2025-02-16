#include "Client.hpp"

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

    while (true) {
        receiveDisplay();
        char choice = getch();
        echo();

        if (choice == 'q') {
            break;
        }

        controller.sendInput(std::string(1, choice), clientSocket);
        choice = '\0';
    }
    network.disconnect(clientSocket);
    endwin(); // Restaure le terminal à son état initial
    
}


void Client::receiveDisplay() {
    char buffer[10000];
    
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




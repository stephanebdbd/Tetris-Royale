#include "Server.hpp"

Server::Server(int port, Game* game, Grid grid, Tetramino tetramino) : port(port), serverSocket(-1), game(game), grid(grid), currentPiece(tetramino) {}

bool Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Erreur: Impossible de créer le socket du serveur." << std::endl;
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Erreur: Impossible de binder le socket." << std::endl;
        close(serverSocket);
        return false;
    }

    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Erreur: Impossible d'écouter sur le port." << std::endl;
        close(serverSocket);
        return false;
    }

    std::cout << "Serveur en attente de connexions sur le port " << port << std::endl;
    return true;
}

void Server::acceptClients() {
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientSocket < 0) {
        std::cerr << "Erreur: Échec de l'acceptation du client." << std::endl;
        return;
    }

    int clientId = clientIdCounter.fetch_add(1);  // Attribuer un ID unique et incrémenter le compteur
    std::cout << "Client #" << clientId << " connecté." << std::endl;
    clientMenuChoices[clientId] = 0;  // Chaque client commence avec menuChoice = 0
    
    clear();
    sendMenuToClient(clientSocket, game->getMainMenu0()); 
    refresh();

    // Lancer un thread pour gérer ce client
    std::thread clientThread(&Server::handleClient, this, clientSocket, clientId);
    clientThread.detach();
}

void Server::handleClient(int clientSocket, int clientId) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Client #" << clientId << " déconnecté." << std::endl;
            close(clientSocket);
            break;
        }
        try {
            json receivedData = json::parse(buffer);
            std::string action = receivedData["action"];
            // Affichage côté serveur de l'action reçue
            if (clientMenuChoices[clientId] == 0) {
                keyInuptWelcomeMenu(clientSocket, clientId, action);
            }
            else if (clientMenuChoices[clientId] == 1) {
                keyInuptMainMenu(clientSocket, clientId, action);
            }
            if (clientMenuChoices[clientId] == 2) {
                keyInuptGameMenu(clientSocket, clientId, action);
            }
        } 
        catch (json::parse_error& e) {
            std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
        }
    }
}

void Server::keyInuptWelcomeMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        clientMenuChoices[clientId]++;
        sendMenuToClient(clientSocket, game->getMainMenu1());      
    }
    else if (action == "2") {
        // Créer un compte => à implémenter
    }
    else if (action == "3") {
        // Quitter => à implémenter
    }
}

void Server::keyInuptMainMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        clientMenuChoices[clientId]++;
        runningGame = true;
        sendGameToClient(clientSocket, game->getGrid().gridToJson().dump());
    }
    else if (action == "2") {
        // Amis => à implémenter
    }
    else if (action == "3") {
        // Classements => à implémenter
    }
    else if (action == "4") {
        // Rejoindre => à implémenter
    }
    if (action == "5") { 
        clientMenuChoices[clientId]--;
        sendMenuToClient(clientSocket, game->getMainMenu0());
    }
}

void Server::keyInuptGameMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "right"){
        //
    }
    if (action == "left"){
        //
    }
    if (action == "down"){
        //
    }
}

void Server::stop() {
    if (serverSocket != -1) {
        shutdown(serverSocket, SHUT_RDWR);
        close(serverSocket);
        serverSocket = -1;
    }
}

void Server::sendMenuToClient(int clientSocket, const std::string& screen) {
    send(clientSocket, screen.c_str(), screen.size(), 0);
}

void Server::sendGameToClient(int clientSocket, const std::string& screen) {
    json message;

    // Envoi de la grille
    message["grid"] = grid.gridToJson(); // Ajout de l'envoi de la grille
    std::string msg = message.dump();
    send(clientSocket, msg.c_str(), msg.size(), 0);

    //Envoi de la pièce courante => Je crois faut attendre que le client envoie un message OK j'ai print la grille
    // mtn je peux print le tetramino
    message["tetraPiece"] = currentPiece.tetraminoToJson();
    msg = message.dump();
    send(clientSocket, msg.c_str(), msg.size(), 0);
}

int main() {
    try {
        std::ofstream serverLog("server.log"); // Créer un fichier de log
        // Rediriger std::cout et std::cerr vers le fichier log
        std::cout.rdbuf(serverLog.rdbuf());
        std::cerr.rdbuf(serverLog.rdbuf());

        Game game(10, 20);
        Server server(12345, &game, game.getGrid(), game.getCurrentPiece());
        if (!server.start()) {
            std::cerr << "Erreur: Impossible de démarrer le serveur." << std::endl;
            return 1;
        }

        while (true) {
            server.acceptClients();
        } 

        server.stop();
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }

    return 0;
}
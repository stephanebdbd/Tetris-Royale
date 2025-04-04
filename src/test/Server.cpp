#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "../common/json.hpp"

#define PORT 12345
#define MAX_CLIENTS 10

class Server {
private:
    int serverSocket;
    fd_set master_fds, read_fds;
    std::map<int, std::string> clients; // Associe les sockets aux noms d'utilisateurs

public:
    Server() : serverSocket(-1) {}

    bool start() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            std::cerr << "Erreur: Impossible de créer le socket du serveur.\n";
            return false;
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Erreur: Bind échoué.\n";
            return false;
        }

        if (listen(serverSocket, MAX_CLIENTS) < 0) {
            std::cerr << "Erreur: Échec de l'écoute.\n";
            return false;
        }

        FD_ZERO(&master_fds);
        FD_SET(serverSocket, &master_fds);

        std::cout << "Serveur en écoute sur le port " << PORT << "...\n";
        return true;
    }

    void run() {
        while (true) {
            read_fds = master_fds;

            if (select(FD_SETSIZE, &read_fds, nullptr, nullptr, nullptr) < 0) {
                std::cerr << "Erreur: Échec de select().\n";
                break;
            }

            for (int fd = 0; fd < FD_SETSIZE; fd++) {
                if (FD_ISSET(fd, &read_fds)) {
                    if (fd == serverSocket) {
                        acceptClient();
                    } else {
                        handleClient(fd);
                    }
                }
            }
        }
    }

    void acceptClient() {
        sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);

        if (clientSocket < 0) {
            std::cerr << "Erreur: Échec de l'acceptation du client.\n";
            return;
        }

        FD_SET(clientSocket, &master_fds);
        clients[clientSocket] = "Client" + std::to_string(clientSocket);
        std::cout << "Nouveau client connecté: " << clients[clientSocket] << "\n";
    }

    void handleClient(int clientSocket) {
        char buffer[1024] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            disconnectClient(clientSocket);
            return;
        }

        try {
            json message = json::parse(std::string(buffer, bytesReceived));
            std::string receiver = message["receiver"];
            std::string msgContent = message["message"];
            std::string sender = clients[clientSocket];

            json response = {{"sender", sender}, {"message", msgContent}};

            for (const auto& [fd, name] : clients) {
                if (name == receiver) {
                    send(fd, response.dump().c_str(), response.dump().size(), 0);
                    break;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Erreur JSON: " << e.what() << "\n";
        }
    }

    void disconnectClient(int clientSocket) {
        std::cout << "Client déconnecté: " << clients[clientSocket] << "\n";
        FD_CLR(clientSocket, &master_fds);
        clients.erase(clientSocket);
        close(clientSocket);
    }

    ~Server() {
        for (const auto& [fd, _] : clients) {
            close(fd);
        }
        close(serverSocket);
    }
};

int main() {
    Server server;
    if (server.start()) {
        server.run();
    }
    return 0;
}

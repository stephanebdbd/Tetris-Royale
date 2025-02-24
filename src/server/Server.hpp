#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"
#include "Grid.hpp"
#include "Tetramino.hpp"
#include "Score.hpp"
#include "UserManager.hpp"  // Inclure le gestionnaire d'utilisateurs
#include <atomic>
#include <unordered_map>

class Server {
    int port;
    int serverSocket;
    //0 = welcome, 1 = main, 2 = création compte, x => game.
    std::unordered_map<int, int> currentMenu;
    std::unique_ptr<Game> game;
    std::atomic<int> clientIdCounter;
    

    //chaque client aura sa game
    std::unordered_map<int, std::unique_ptr<Game>> games;

    std::atomic<bool> runningGame{false};
    std::unique_ptr<Grid> grid;
    std::unique_ptr<Tetramino> currentPiece;
    std::unique_ptr<Score> score;

    // Gestionnaire d'utilisateurs déclaré directement ici.
    std::unique_ptr<UserManager> userManager;

    std::unordered_map<std::string, std::string> unicodeToText = {
        {"\u0005", "right"},
        {"\u0004", "left"},
        {"\u0003", "up"},
        {"\u0002", "down"},
        {" ", "drop"}
    };

public:
    Server(int port, Game* game);

    bool start();
    void acceptClients();
    void handleClient(int clientSocket, int clientId);
    void stop();
    void sendMenuToClient(int clientSocket, const std::string& screen);
    void sendGameToClient(int clientSocket, int clientId);
    void keyInuptWelcomeMenu(int clientSocket, int clientId, const std::string& action);
    void keyInuptMainMenu(int clientSocket, int clientId, const std::string& action);
    void keyInuptGameMenu(int clientSocket, int clientId, const std::string& action);
    void loopGame(int clientSocket, int clientId);
    void receiveInputFromClient(int clientSocket, int clientId);
    void handleMenu(int clientSocket, int clientId, const std::string& action);
    void handleRegisterMenu(int clientSocket, int clientId, const nlohmann::json& data);
    std::string convertUnicodeToText(const std::string& unicode);

    bool isValidPseudo(int clientSocket, int clientId, const std::string& action);
};

#endif

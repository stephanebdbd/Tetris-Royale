#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"
#include "GameMode.hpp"
#include "GameMode.hpp"
#include "Grid.hpp"
#include "Tetramino.hpp"
#include "Score.hpp"
#include "UserManager.hpp"
#include "GameRoom.hpp"
#include "Menu.hpp"
#include "GameRoom.hpp"
#include "Menu.hpp"
#include <atomic>
#include <unordered_map>

enum class MenuState {
    Welcome,
        RegisterPseudo,
            RegisterPassword,
        LoginPseudo,
            LoginPassword,
    Main,
        JoinOrCreateGame,
            GameMode,
        classement,
        chat
};

class Server {
    int port;
    int serverSocket;
    //0 = welcome, 1 = main, 2 = création compte, x => game.
    std::unordered_map<int, int> currentMenu;
    std::unordered_map<int, Menu> menus;
    std::atomic<int> clientIdCounter;
    

    //chaque client aura sa game
    std::unordered_map<int, std::string> clientPseudo;    // id -> pseudo
    std::unordered_map<int, MenuState> clientStates;      // id -> menu
    std::unordered_map<std::string, int> pseudoTosocket;  // pseudo -> socket

    std::unique_ptr<UserManager> userManager;

public:
    Server(int port);

    bool start();
    void acceptClients();
    void handleClient(int clientSocket, int clientId);
    void stop();
    void sendMenuToClient(int clientSocket, const std::string& screen);
    void keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputMainMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputRegisterPseudoMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputJoinOrCreateGameMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputChatMenu(int clientSocket, int clientId, const std::string& action);
    void sendChatModeToClient(int clientSocket);
    void receiveInputFromClient(int clientSocket, int clientId);
    void handleMenu(int clientSocket, int clientId, const std::string& action);
    std::string convertUnicodeToText(const std::string& unicode);



};

#endif 

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"
#include "GameMode.hpp"
#include "Grid.hpp"
#include "Tetramino.hpp"
#include "Score.hpp"
#include "GameRoom.hpp"
#include "chat.hpp"
#include "UserManager.hpp"  // Inclure le gestionnaire d'utilisateurs
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
        Game, 
            JoinOrCreateGame,
                CreateGame,
                JoinGame,
                    Play,
                    GameOver,
        classement,
        chat
};

class Server {
    int port;
    int serverSocket;
    //0 = welcome, 1 = main, 2 = création compte, x => game.
    std::atomic<int> clientIdCounter;
    std::unordered_map<int, int> clientGameRoomId;
    std::vector<std::shared_ptr<GameRoom>> gameRooms;
    std::unique_ptr<ServerChat> chat;
    
    int gameRoomIdCounter=0;
    
    
    //chaque client aura sa game
    std::unordered_map<int, std::string> clientPseudo;    // id -> pseudo
    std::unordered_map<int, MenuState> clientStates;      // id -> menu
    std::unordered_map<std::string, int> pseudoTosocket;  // pseudo -> socket
    
    std::unique_ptr<UserManager> userManager;
    
    Menu menu;


public:
    Server(int port);

    bool start();
    void acceptClients();
    void handleClient(int clientSocket, int clientId);
    void stop();
    void loopGame(int clientSocket, int clientId);
    void sendMenuToClient(int clientSocket, const std::string& screen);
    void keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputMainMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputRegisterPseudoMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputJoinOrCreateGameMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputGameModeMenu(int clientSocket, int clientId, GameModeName gameMode=GameModeName::Endless);
    //void keyInputChatMenu(int clientSocket, int clientId, const std::string& action);
    void sendChatModeToClient(int clientSocket);
    void receiveInputFromClient(int clientSocket, int clientId);
    void deleteGameRoom(int roomId);
    void sendGameToClient(int clientSocket, int clientId);
    void sendInputToGameRoom(int clientId, const std::string& action);
    void shiftGameRooms(int index);
    void keyInputRankingMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputGameOverMenu(int clientSocket, int clientId, const std::string& action);
    void handleMenu(int clientSocket, int clientId, const std::string& action);
    void clearMenu(int clientSocket, const std::string& functionName);
    void sendGameToPlayer(int clientSocket, int clientId);
    ~Server() {endwin();}
};

#endif 

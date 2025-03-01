#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"
#include "Grid.hpp"
#include "Tetramino.hpp"
#include "Score.hpp"
#include "chat.hpp"
#include "UserManager.hpp"  // Inclure le gestionnaire d'utilisateurs
#include <atomic>
#include <unordered_map>
#include "FriendList.hpp"

enum class MenuState {
    Welcome,
        RegisterPseudo,
            RegisterPassword,
        LoginPseudo,
            LoginPassword,
    Main,
        Game, 
            JoinOrCreateGame,
                GameMode,
                GameOver,
        classement,
        chat,
            createRoom,
            joinRoom,
            listeRooms,
            privateChat,
        Friends,
            AddFriend,
            RemoveFriend,
            FriendList,
            Request
};

class Server {
    int port;
    int serverSocket;
    //0 = welcome, 1 = main, 2 = création compte, x => game.
    std::unordered_map<int, int> currentMenu;
    std::unique_ptr<Game> game;
    std::unique_ptr<ServerChat> chat;
    std::unique_ptr<FriendList> friendList;
    std::atomic<int> clientIdCounter;

    //chaque client aura sa game
    std::unordered_map<int, std::unique_ptr<Game>> games; // id -> game
    std::unordered_map<int, std::string> clientPseudo;    // id -> pseudo
    std::unordered_map<int, MenuState> clientStates;      // id -> menu
    std::unordered_map<std::string, int> pseudoTosocket;  // pseudo -> socket


    std::unordered_map<int, bool> runningGames;
    std::unordered_map<int, bool> runningChats;

    std::unique_ptr<Grid> grid;
    std::unique_ptr<Tetramino> currentPiece;
    std::unique_ptr<Score> score;

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
    void keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputMainMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputGameMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputRegisterPseudoMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputRegisterPseudoMenuFailed(int clientSocket, int clientId, const std::string& action);
    void keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputJoinOrCreateGameMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputChatMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputModeGameMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputRankingMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputGameOverMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputFriendsMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputAddFriendMenu(int clientSocket, int clientId, const std::string& action);
    void loopGame(int clientSocket, int clientId);
    void receiveInputFromClient(int clientSocket, int clientId);
    void handleMenu(int clientSocket, int clientId, const std::string& action);
    std::string convertUnicodeToText(const std::string& unicode);
    void setRunningGame(int clientId, bool value) { runningGames[clientId] = value; }
    void setRunningChat(int clientId, bool value) { runningChats[clientId] = value; }
    void setClientState(int clientId, MenuState state) { clientStates[clientId] = state; }
    bool getRunningChat(int clientId) { return runningChats[clientId]; }
    std::unordered_map<std::string, int> getPseudoSocket() { return pseudoTosocket; }


};

#endif 

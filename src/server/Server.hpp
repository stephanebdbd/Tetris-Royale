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
#include "FriendList.hpp"
#include "chatRoom.hpp"
#include "FriendList.hpp"
#include "chatRoom.hpp"

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
                    ChooseGameMode,
                        Endless,
                        Duel,
                        Classic,
                        Royal_Competition,
                            Settings,
                            Help,
                JoinGame,
                    GameRequestList,
                        Play,
                        GameOver,
        classement,
        chat,
            CreateRoom,
            JoinRoom,
            ManageRooms,
                ManageRoom,
                    ListRoomMembres,
                    AddMembre,
                    AddAdmin,
                    RoomRequestList,
                    QuitRoom,
                    ConfirmDeleteRoom,
                    ConfirmQuitRoom,
                    
            PrivateChat,
        Friends,
            AddFriend,
            FriendList,
            FriendRequestList,
};

class Server {
    int port;
    int serverSocket;
    //0 = welcome, 1 = main, 2 = création compte, x => game.
    std::atomic<int> clientIdCounter;
    std::unordered_map<int, int> clientGameRoomId;
    std::vector<std::shared_ptr<GameRoom>> gameRooms;  
    std::unordered_map<std::string, std::shared_ptr<chatRoom>> chatRooms;
    std::shared_ptr<ServerChat> chat;
    std::shared_ptr<FriendList> friendList;
    
    int gameRoomIdCounter=0;
    
    
    //chaque client aura sa game
    std::unordered_map<int, std::string> clientPseudo;    // id -> pseudo
    std::unordered_map<int, MenuState> clientStates;      // id -> menu
    std::unordered_map<std::string, int> pseudoTosocket;  // pseudo -> socket
    std::unordered_map<int, std::string> sockToPseudo;    // socket -> pseudo
    std::unordered_map<int, bool> runningChats; 
    std::unordered_map<int, std::string> roomToManage;    // id -> room
    std::mutex clientPseudoMutex;        // socket -> bool(chat en cours)
   

    std::unique_ptr<UserManager> userManager;
    
    Menu menu;
    void returnToMenu(int clientSocket, int clientId, MenuState state, const std::string& message = "", int sleepTime = 3);

public:
    Server(int port);

    bool start();
    void acceptClients();
    void handleClient(int clientSocket, int clientId);
    void stop();
    void loopGame(int clientSocket, int clientId);

    void sendMenuToClient(int clientSocket, const std::string& screen);
    //welcome & main
    void keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputMainMenu(int clientSocket, int clientId, const std::string& action);
    //register & login
    void keyInputRegisterPseudoMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action);
    //game
    void keyInputJoinOrCreateGameMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputGameModeMenu(int clientSocket, int clientId, GameModeName gameMode=GameModeName::Endless);
    //chat
    void keyInputChatMenu(int clientSocket, int clientId, const std::string& action);
    void sendChatModeToClient(int clientSocket);
    void keyInputCreateChatRoom(int clientSocket, int clientId, const std::string& action);
    void keyInputJoinChatRoom(int clientSocket, int clientId, const std::string& action);
    void keyInputManageMyRooms(int clientSocket, int clientId, const std::string& action);
    void keyInputManageRoom(int clientSocket, int clientId, const std::string& action);
    void keyInputListMembres(int clientSocket, int clientId, const std::string& action);
    void keyInputAddMembre(int clientSocket, int clientId, const std::string& action);
    void keyInputAddAdmin(int clientSocket, int clientId, const std::string& action);
    void keyInputRequestList(int clientSocket, int clientId, const std::string& action);
    void keyInputQuitRoom(int clientSocket, int clientId, const std::string& action);
    void loadChatRooms();
    void keyInputConfirmDeleteRoom(int clientSocket, int clientId, const std::string& action);

    //friends
    void keyInputFriendsMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputAddFriendMenu(int clientSocket, int clientId, const std::string& action);
    

    void receiveInputFromClient(int clientSocket, int clientId);
    void deleteGameRoom(int roomId);
    void extractDataBetweenSlashes(const std::string& toFind, const std::string& action, std::string& status, std::string& receiver);
    void sendInputToGameRoom(int clientId, const std::string& action, std::shared_ptr<GameRoom> gameRoom);
    void shiftGameRooms(int index);
    void keyInputRankingMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputGameOverMenu(int clientSocket, int clientId, const std::string& action);
    void handleMenu(int clientSocket, int clientId, const std::string& action);
    std::string convertUnicodeToText(const std::string& unicode);
    void setRunningChat(int clientSocket, bool value);
    void setClientState(int clientId, MenuState state);
    bool getRunningChat(int clientSocket);
    std::unordered_map<std::string, int> getPseudoSocket() { return pseudoTosocket; }
    std::unordered_map<int, std::string> getSocketPseudo() { return sockToPseudo; }
    std::unordered_map<std::string, std::shared_ptr<chatRoom>> getChatRooms() { return chatRooms; }
    void keyInputManageFriendRequests(int clientSocket, int clientId, const std::string& action);
    void keyInputManageFriendlist(int clientSocket, int clientId, const std::string& action);
    void sendGameToPlayer(int clientSocket, std::shared_ptr<Game> game, Score& score);
    std::string trim(const std::string& s);
    void keyInputChooseGameModeMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLobbySettingsMenu(int clientSocket, int clientId, const std::string& action, std::shared_ptr<GameRoom> gameRoom=nullptr);
    void keyInputHelpMenu(int clientSocket, int clientId, const std::string& action);
    std::string getMode(int clientId);
    int getMaxPlayers(int clientId);
    int getAmountOfPlayers(int clientId);
    void letPlayersPlay(const std::vector<int>& players);
    void ownerStartsGame(std::shared_ptr<GameRoom> gameRoom);

    void keyInputSendGameRequestMenu(int clientSocket, int clientId, std::string receiver, std::string status);
    void keyInputChoiceGameRoom(int clientSocket, int clientId, const std::string& action);

    void startGame(int clientSocket, int clientId);

};

#endif 

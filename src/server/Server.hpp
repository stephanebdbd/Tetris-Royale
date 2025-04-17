#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"                 // Inclure la classe Game
#include "GameMode.hpp"             // Inclure la classe GameMode
#include "Grid.hpp"                 // Inclure la classe Grid
#include "Tetramino.hpp"            // Inclure la classe Tetramino
#include "Score.hpp"                // Inclure la classe Score
#include "GameRoom.hpp"             // Inclure la classe GameRoom
#include "./data/UserManager.hpp"   // Inclure le gestionnaire d'utilisateurs
#include "./data/chatRoom.hpp"      // Inclure la base de données
#include "./data/chat.hpp"          // Inclure la base de données
#include "Menu.hpp"
#include <atomic>
#include <unordered_map>
#include <mutex>


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
                JoinGame,
                    GameRequestList,
                        Play,
                        GameOver,
        classement,

        Rooms,
            CreateRoom,
            JoinRoom,
            RoomsRequest,
            ChatRooms,
                ChatRoom,
                
            ManageRooms,
                ManageRoom,
                    ManageMyRooms,
                        ListRoomMembres,
                        AddMembre,
                        AddAdmin,
                        RoomRequestList,
                        QuitRoom,
                            ConfirmDeleteRoom,
                            ConfirmQuitRoom,
        Friends,
            ChooseContact,
                PrivateChat,
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
    std::shared_ptr<DataBase> database; // DataBase instance
    DataManager dataManager; //std::unordered_map<std::string, std::vector<std::string>> users_notifications;
    ChatRoom chatRoomsManage; // ChatRoom instance
    Chat chat; // Chat instance

    //ChatRoom chatRoomsManage;
    //std::shared_ptr<ServerChat> chat;
    //std::shared_ptr<FriendList> friendList;
    
    int gameRoomIdCounter=0;
    
    
    //chaque client aura sa game
    std::unordered_map<std::string, std::shared_ptr<std::vector<int>>> roomMap;
    std::mutex roomMutex;

    std::unordered_map<int, std::string> clientPseudo;            // id -> pseudo
    std::unordered_map<int, MenuState> clientStates;              // id -> menu
    std::unordered_map<std::string, int> pseudoTosocket;          // pseudo -> socket
    std::unordered_map<int, std::string> sockToPseudo;            // socket -> pseudo
    std::unordered_map<int, bool> runningChats;                   // socket -> bool(chat en cours)
    std::unordered_map<int, std::string> roomToManage;            // id -> room
    std::unordered_map<int, std::string> receiverOfMessages;       //id -> destinataire
    std::mutex clientPseudoMutex;                                 // mutex pour pseudo
    std::mutex clientStatesMutex;                                 // mutex pour menu
    std::mutex clientsChatMutex;                                       //mutex pour chat

    //std::unique_ptr<UserManager> userManager;
    
    Menu menu;
    void returnToMenu(int clientSocket, int clientId, MenuState state, const std::string& message = "", int sleepTime = 3);

public:
    Server(int port);

    bool start();
    void acceptClients();
    void handleClient(int clientSocket, int clientId);
    void cleanupClient(int clientSocket, int clientId);
    void stop();
    void loopGame(int clientSocket, int clientId);
    std::shared_ptr<std::vector<int>> joinRoom(const std::string& roomName, int clientSocket) ;

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
    void keyInputChatRoomsChooseMenu(int clientSocket, int clientId, const std::string& action) ;

    void keyInputChatFriendsMenu(int clientSocket, int clientId, const std::string& action);
    //void keyInputPrivateChat(int clientSocket, int clientId, const std::string& action);
    void sendChatModeToClient(int clientSocket);
    void keyInputCreateChatRoom(int clientSocket, int clientId, const std::string& action);
    void keyInputJoinChatRoom(int clientSocket, int clientId, const std::string& action);
    void keyInputRoomsRequest(int clientSocket, int clientId, const std::string& action);
    void keyInputManageMyRooms(int clientSocket, int clientId, const std::string& action);
    void keyInputManageRoom(int clientSocket, int clientId, const std::string& action);
    void keyInputListMembres(int clientSocket, int clientId, const std::string& action);
    void keyInputAddMembre(int clientSocket, int clientId, const std::string& action);
    void keyInputAddAdmin(int clientSocket, int clientId, const std::string& action);
    void keyInputRequestList(int clientSocket, int clientId, const std::string& action);
    void keyInputQuitRoom(int clientSocket, int clientId, const std::string& action);
    void keyInputConfirmDeleteRoom(int clientSocket, int clientId, const std::string& action);
    void keyInputChatRoomsMenu(int clientSocket, int clientId, const std::string& action) ;


    //friends
    void keyInputFriendsMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputAddFriendMenu(int clientSocket, int clientId, const std::string& action);


    void receiveInputFromClient(int clientSocket, int clientId);
    void deleteGameRoom(int roomId);
    void sendInputToGameRoom(int clientId, const std::string& action, std::shared_ptr<GameRoom> gameRoom);
    void shiftGameRooms(int index);
    void keyInputRankingMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputGameOverMenu(int clientSocket, int clientId, const std::string& action);
    void handleMenu(int clientSocket, int clientId, const std::string& action, bool refreshMenu=false);
    std::string convertUnicodeToText(const std::string& unicode);
    bool getRunningChat(int clientSocket);
    std::unordered_map<std::string, int> getPseudoSocket() { return pseudoTosocket; }
    std::unordered_map<int, std::string> getSocketPseudo() { return sockToPseudo; }
    //std::unordered_map<std::string, std::shared_ptr<chatRoom>> getChatRooms() { return chatRooms; }
    void keyInputManageFriendRequests(int clientSocket, int clientId, const std::string& action);
    void keyInputManageFriendlist(int clientSocket, int clientId, const std::string& action);
    void sendGameToPlayer(int clientSocket, std::shared_ptr<Game> game, Score& score);
    std::string trim(const std::string& s);
    void keyInputChooseGameModeMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLobbySettingsMenu(int clientSocket, int clientId, const std::string& action, std::shared_ptr<GameRoom> gameRoom=nullptr);
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

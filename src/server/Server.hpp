#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"
#include "GameMode.hpp"
#include "Grid.hpp"
#include "Tetramino.hpp"
#include "Score.hpp"
#include "GameRoom.hpp"
#include "Menu.hpp"
#include "data/chat.hpp"
#include "data/UserManager.hpp"
#include "data/chatRoom.hpp"
#include "../common/MenuState.hpp"
#include <atomic>
#include <unordered_map>
#include <mutex>



class Server {
    int port;
    int serverSocket;
    //0 = welcome, 1 = main, 2 = création compte, x => game.
    std::atomic<int> clientIdCounter;
    std::atomic<int> gameRoomIdCounter;
    std::unordered_map<int, int> clientGameRoomId;
    std::unordered_map<int, int> clientIdToSocket; 
    std::unordered_map<int, std::shared_ptr<GameRoom>> gameRooms;  
    std::shared_ptr<DataBase> database; // DataBase instance
    DataManager userManager; //std::unordered_map<std::string, std::vector<std::string>> users_notifications;
    ChatRoom chatRoomsManage; // ChatRoom instance
    Chat chat; // Chat instance
    Menu menu;
    std::shared_ptr<MenuStateManager> menuStateManager;
    std::unordered_map<int, int> clientAvatar; // Associe un clientId à un avatarIndex
    

    //chaque client aura sa game
    std::unordered_map<int, std::string> clientPseudo;    // id -> pseudo
    std::unordered_map<int, MenuState> clientStates;      // id -> menu
    std::unordered_map<std::string, int> pseudoTosocket;  // pseudo -> socket
    std::unordered_map<int, std::string> sockToPseudo;    // socket -> pseudo
    std::unordered_map<int, std::string> roomToManage;    // id -> room
    std::unordered_map<int, std::string> receiverOfMessages;    // id -> receiver
    std::unordered_map<int, bool> clientMode; //id -> Mode(terminale (false), Gui(true))
    std::mutex clientPseudoMutex;        // socket -> bool(chat en cours)
    std::mutex gameRoomsMutex;          // mutex pour la gestion des rooms de jeu
   

    std::mutex clientMutex;

public:
    Server(int port);

    bool start();
    void acceptClients();
    void handleClient(int clientSocket, int clientId);
    void stop();
    void loopGame(int ownerId);
    void keyInputAddFriendMenuGUI(int clientSocket,int clientId,const std::string& action);

    //menu
    void sendMenuToClient(int clientSocket, const std::string& screen);
    void returnToMenu(int clientSocket, int clientId, MenuState state, const std::string& message = "", int sleepTime = 3);
    void handleMenu(int clientSocket, int clientId, const std::string& action, bool refreshMenu=false);
 
    //welcome & main
    void keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputMainMenu(int clientSocket, int clientId, const std::string& action);
    //register & login
    void keyInputRegisterPseudoMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action);

    //chat
    void handleChat(int clientSocket, int clientId, json& receivedData);
    void keyInputPrivateChat(int clientSocket, int clientId, const std::string& action);
    void keyInputChatTeam(int clientSocket, int clientId, const std::string& action);

    //Teams
    void keyInputTeamsMenu(int clientSocket, int clientId, const std::string& action);
    void sendChatModeToClient(int clientSocket);
    void keyInputCreateChatRoom(int clientSocket, int clientId, const std::string& action);
    void keyInputJoinChatRoom(int clientSocket, int clientId, const std::string& action);
    void keyInputTeamsInvitation(int clientSocket, int clientId, const std::string& action);
    void keyInputManageTeams(int clientSocket, int clientId, const std::string& action);
    void keyInputManageTeam(int clientSocket, int clientId, const std::string& action);
    void keyInputListMembres(int clientSocket, int clientId, const std::string& action);
    void keyInputAddMembre(int clientSocket, int clientId, const std::string& action);
    void keyInputAddAdmin(int clientSocket, int clientId, const std::string& action);
    void keyInputRequestList(int clientSocket, int clientId, const std::string& action);
    void keyInputQuitRoom(int clientSocket, int clientId, const std::string& action);
    void keyInputConfirmDeleteRoom(int clientSocket, int clientId, const std::string& action);


    //friends
    void keyInputFriendsMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputAddFriendMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputManageFriendRequests(int clientSocket, int clientId, const std::string& action);
    void keyInputManageFriendlist(int clientSocket, int clientId, const std::string& action);


    //game
    void keyInputJoinOrCreateGameMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputCreateGameRoom(int clientId, GameModeName gameMode);
    bool gamePreparation(int ownerId, std::shared_ptr<GameRoom> gameRoom);
    void deleteGameRoom(int roomId, const std::vector<int> players);
    void extractDataBetweenSlashes(const std::string& toFind, const std::string& action, std::string& status, std::string& receiver);
    void sendInputToGameRoom(int clientId, const std::string& action);
    void keyInputRankingMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputGameOverMenu(int clientSocket, int clientId, const std::string& action);
    void sendGameToPlayer(int clientId, int clientSocket, std::shared_ptr<GameRoom> gameRoom);
    std::string trim(const std::string& s);
    void keyInputChooseGameModeMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputLobbySettingsMenu(int clientSocket, int clientId, const std::string& action);
    void keyInputHelpMenu(int clientSocket, int clientId, const std::string& action);
    std::string getMode(int clientId);
    int getMaxPlayers(int clientId);
    int getAmountOfPlayers(int clientId);
    void letPlayersPlay(const std::vector<int>& players);
    void disconnectPlayer(int clientId);
    void keyInputSendGameRequestMenu(int clientSocket, int clientId, std::string receiver, std::string status);
    void keyInputChoiceGameRoom(int clientSocket, int clientId, const std::string& action);


    //GUI
    void handleGUIActions(int clientSocket, int clientId, const json& action);
    void sendMiniGameToPlayer(int clientId, int clientSocket, std::shared_ptr<GameRoom> gameRoom);
    void sendInputToGameRoom2(int clientId, const std::string& action);
    void letObserversObserve(const std::vector<int>& observers);


};

#endif 
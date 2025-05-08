#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ClientDisplay.hpp"  
#include "Controller.hpp"
#include "ClientNetwork.hpp"
#include "ClientChat.hpp"
#include "../common/MenuState.hpp"
#include <ostream>

#include "../common/GameState.hpp"
#include <mutex>
#include <thread>
#include <atomic>
#include <string>
#include <functional>


class Client {
    ClientDisplay display;  // Chaque client a son propre display
    Controller controller;  // Chaque client a son propre controller
    ClientNetwork network;  // Chaque client a son propre network
    ClientChat chat;        // Chaque client a son propre chat
    MenuStateManager menuStateManager; // Chaque client a son propre gestionnaire d'état de menu


    std::string serverIP;               // Adresse IP du serveur
    int port;                           // Port du serveur
    int clientSocket;                   // Socket du client
    std::string inputBuffer;                  // Tampon d'entrée pour les données du client
    bool isPlaying = false;             // Indique si le client est en mode jeu
    bool chatMode = false;                  // Indique si le client est en mode chat
    MenuState currentMenuState;            // État du menu actuel
    GameState gameState;
    std::mutex gameStateMutex;

    json serverData;                // Données reçues du serveur
    std::atomic<bool> stop_threads{false};
    std::thread inputThread;
    std::thread receiveThread;
    std::string receivedData;
    std::mutex receiveMutex;
    std::string temporaryMessage;
    std::mutex messageMutex;
    int avatarIndex = -1; // Index de l'avatar du joueur actuel (-1 par défaut si non défini)
    bool show = false;
    bool isTerminal;
    std::vector<std::pair<std::string, int>> contacts;
    std::map<std::string, std::vector<std::string>> ranking;
    std::vector<std::string> amis;
    std::vector<std::string> PlayerInfo;
    std::vector<std::string> teams;

    public:
        Client(const std::string& serverIP, int port);
        ~Client();
        void run(const std::string& mode);
        bool connect();


        void receiveDisplay();
        std::string receiveData();
        void appendToBuffer(const std::string& chunk);
        void processBufferedMessages();

        void handleJsonMessage(const std::string& jsonStr);

        void handleGameGrid(const json& data);
        void startChatMode(const json& data);
        void handleChatMessage(const json& data);
        void handleChatHistory(const json& data);
        void handleOtherMessages(const json& data);
        void handleStatefulData(const json& data);

        
        void handleUserInput();
        void displayMenu(const json& data);
        int getClientSocket() const { return clientSocket; }
        void stopThreads() { stop_threads = true; }
        MenuState getCurrentMenuState();
        void setCurrentMenuState(MenuState menuState){ currentMenuState = menuState; }
        json getServerData() const { return serverData; }
        void setTemporaryMessage(const std::string& msg);
        std::string getTemporaryMessage();

        const GameState getGameState();
        void setGameStateFromServer(const json& data);
        bool isGameStateUpdated();
        void setGameStateUpdated(bool updated);


        void sendInputFromSFML(const std::string& input);
        void setGameStateIsEnd(bool isEnd);
       
        void clearServerData();
        void setAvatarIndex(int index);

        int getAvatarIndex() const;
        const std::vector<std::pair<std::string, int>>& getContacts() const ;
        void setContacts(const std::vector<std::pair<std::string, int>>& newContacts);
        void setIsTerminal(bool isTerminal) { this->isTerminal = isTerminal; }
        void reintiliseData();
        void setRanking(std::map<std::string, std::vector<std::string>> ranking1);
        std::map<std::string, std::vector<std::string>> getRanking() const;
        void setAmis(const std::vector<std::string>& friends);
        std::vector<std::string> getAmis() const;
        void setPlayerInfo(const std::vector<std::string>& playerInfo);
        std::vector<std::string> getPlayerInfo() const;
        void setShow(bool showfenetre);
        bool getShow() const;
        std::vector<std::string> getTeams() const;
        void setTeams(const std::vector<std::string>& teams);
};

#endif
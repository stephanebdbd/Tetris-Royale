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

private:
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

    //DisplayCallback displayCallback;
    json serverData;                // Données reçues du serveur
    std::atomic<bool> stop_threads{false};
    std::thread inputThread;
    std::thread receiveThread;
    std::string receivedData;
    std::mutex receiveMutex;
    std::string temporaryMessage;
    std::mutex messageMutex;
    int avatarIndex = -1; // Index de l'avatar du joueur actuel (-1 par défaut si non défini)
    std::vector<std::pair<std::string, int>> contacts;
    bool isTerminal;
    std::map<std::string, std::vector<std::string>> ranking;
    std::vector<std::string> amis;
    std::vector<std::string> PlayerInfo;
    bool show = false;





public:
        Client(const std::string& serverIP, int port);
        ~Client();
        void run(const std::string& mode);
        bool connect();
        void receiveDisplay();
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
       
        void clearServerData() { serverData.clear(); }
        void setAvatarIndex(int index) {
            avatarIndex = index;
        }

        int getAvatarIndex() const {
            return avatarIndex;
        }
        const std::vector<std::pair<std::string, int>>& getContacts() const ;
        void setContacts(const std::vector<std::pair<std::string, int>>& newContacts);
        void setIsTerminal(bool isTerminal) { this->isTerminal = isTerminal; }
        void reintiliseData();
        void setRanking(std::map<std::string, std::vector<std::string>> ranking1) {
            ranking = ranking1;
        }
        std::map<std::string, std::vector<std::string>> getRanking() const {
            return ranking;
        }
        void setAmis(const std::vector<std::string>& friends) {
            amis = friends;
        }
        std::vector<std::string> getAmis() const {
            return amis;
        }
        void setPlayerInfo(const std::vector<std::string>& playerInfo) {
            PlayerInfo = playerInfo;
        }
        std::vector<std::string> getPlayerInfo() const {
            return PlayerInfo;
        }
        void setShow(bool showfenetre) {
            show = showfenetre;
        }
        bool getShow() const {
            return show;
        }
};

#endif
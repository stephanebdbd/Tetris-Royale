#ifndef CLIENTCHAT_HPP
#define CLIENTCHAT_HPP

#include "ClientNetwork.hpp"
#include "../common/json.hpp"
#include <ncurses.h>
#include <thread>


class ClientChat{
    int clientSocket;                    // Le socket du client
    ClientNetwork network;               // Le network du client
    std::string MyPseudo;               // Le pseudo du client
    static bool messagesWaitForDisplay;  // Booléen pour savoir si le message attend d'être affiché
    std::vector<json> chatMessages;   // Les messages
    int y = 1;                            // Position du chat
    WINDOW * inputWin, *displayWin;       // L'input et affichage de la fenêtre

    public:
        ClientChat() = default;
        ~ClientChat() = default;

        void run();
        void sendChatMessages();
        void receiveChatMessages(const json& msg);
        void displayChatMessages();
        void addChatMessage(const json& msg);
        void setClientSocket(int clientSocket);
        void setMyPseudo(const std::string& pseudo);
};

#endif
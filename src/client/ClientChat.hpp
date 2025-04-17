#ifndef CLIENTCHAT_HPP
#define CLIENTCHAT_HPP

#include "ClientNetwork.hpp"
#include "../common/json.hpp"
#include <ncurses.h>
#include <thread>


class ClientChat{
    private:
        int clientSocket;                    ///< The client socket.
        ClientNetwork network;               ///< The client network.
        std::string MyPseudo;               ///< The client's pseudo.
        static bool messagesWaitForDisplay;  ///< A boolean indicating if the message is waiting for display.
        std::vector<json> chatMessages;   ///< A vector of chat messages.
        int y = 1;                            ///< The y position of the chat message.
        WINDOW * inputWin, *displayWin;       ///< The input and display window.

    public:
        ClientChat() = default;
        ~ClientChat() = default;

        void run();
        void sendChatMessages();
        void receiveChatMessages(const json& msg);
        void displayMessage(const std::string& sender, const std::string& message);
        void displayChatMessages();
        void addChatMessage(const json& msg);
        void setClientSocket(int clientSocket);
        void setMyPseudo(const std::string& pseudo);
};

#endif // CLIENTCHAT_HPP
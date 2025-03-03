#ifndef CLIENTCHAT_HPP
#define CLIENTCHAT_HPP

#include "ClientNetwork.hpp"
#include "../common/json.hpp"
#include <ncurses.h>
#include <thread>

/**
 * @class ClientChat
 * @brief A class representing a client chat.
 */
class ClientChat{
    private:
        int clientSocket;                    ///< The client socket.
        ClientNetwork network;               ///< The client network.
        bool isChatting;                     ///< A boolean indicating if the client is chatting.
        static bool messagesWaitForDisplay;  ///< A boolean indicating if the message is waiting for display.
        int y = 1;                            ///< The y position of the chat message.
        WINDOW * inputWin, *displayWin;       ///< The input and display window.

    public:

        ClientChat() = default;

        void run();
        void sendChatMessages();
        void receiveChatMessages(const json& msg);
        void displayChatMessage(std::string sender, const std::string& message);

        static bool initMessageMemory();
        static bool saveMessage(const std::string& message);
        static bool FlushMemory();

        void setClientSocket(int clientSocket);
        void setIsChatting(bool isChatting);
};

#endif // CLIENTCHAT_HPP
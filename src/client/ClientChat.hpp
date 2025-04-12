#ifndef CLIENTCHAT_HPP
#define CLIENTCHAT_HPP

#include "ClientNetwork.hpp"
#include "../common/json.hpp"
#include <ncurses.h>
#include <thread>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
/**
 * @class ClientChat
 * @brief A class representing a client chat.
 */
class ClientChat{
    private:
        int clientSocket;                    ///< The client socket.
        ClientNetwork network;               ///< The client network.
        static bool messagesWaitForDisplay;  ///< A boolean indicating if the message is waiting for display.
        int y = 1;                            ///< The y position of the chat message.
        WINDOW * inputWin, *displayWin;       ///< The input and display window.
        std::string pseudo_name;                  ///< The pseudo of the client.
    public:

        ClientChat() = default;

        void run(std::string pseudo);
        void sendChatMessages();
        void receiveChatMessages(const json& msg);
        void displayChatMessage(const std::string& sender, const std::string& message);


        void setClientSocket(int clientSocket);
};

#endif // CLIENTCHAT_HPP
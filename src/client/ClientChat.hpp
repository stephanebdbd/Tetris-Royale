#ifndef ClientChat_HPP
#define ClientChat_HPP

#include "Client.hpp"
#include "../common/json.hpp"
#include "../include.hpp"

/**
 * @class ClientChat
 * @brief A class representing a client chat.
 */
class ClientChat{
    private:
        int clientSocket;         ///< The client socket.
        ClientNetwork network;    ///< The client network.


    public:
        /**
         * @brief Constructor for ClientChat.
         * @param clientSocket The client socket.
         */
        ClientChat() = default;

        /**
         * @brief Sends a chat message.
         * @param message The message to send.
         */
        void sendChatMessage(const std::string& message);

        /**
         * @brief Receives a chat message.
         */
        void receiveChatMessage();

        /**
         * @brief Displays a chat message.
         * @param message The message to display.
         */
        void displayChatMessage(const std::string& message);

        /**
         * @brief Initializes the message memory.
         */
        static bool initMessageMemory();

        /**
         * @brief Saves a message to the message memory.
         * @param message The message to save.
         */
        static bool saveMessage(const std::string& message);

        /**
         * @brief Flushes the message memory.
         */
        static bool FlushMemory();

        /**
         * @brief sets the client socket.
         * @param clientSocket The client socket.
         */
        void setClientSocket(int clientSocket);
};

#endif
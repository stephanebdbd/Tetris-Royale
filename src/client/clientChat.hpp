#ifndef ClientChat_HPP
#define ClientChat_HPP#ifndef ClientChat_HPP
#define ClientChat_HPP

#include "Client.hpp"
#include <string>
#include <iostream>

/**
 * @class ClientChat
 * @brief A class representing a client chat.
 * @inherits Client
 */
class ClientChat : public Client {
    private:
        int clientSocket; ///< The client socket.
        std::string messageMemory; ///< The memory to store chat messages.

    public:
        /**
         * @brief Constructor for ClientChat.
         * @param clientSocket The client socket.
         */
        ClientChat(int clientSocket);

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
        void initMessageMemory();

        /**
         * @brief Saves a message to the message memory.
         * @param message The message to save.
         */
        void saveMessage(const std::string& message);

        /**
         * @brief Flushes the message memory.
         */
        void FlushMemory();
};

#endif
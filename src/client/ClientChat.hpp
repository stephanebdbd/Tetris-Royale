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
        bool isPlaying = false;              ///< A boolean indicating if the client is playing.
        static bool messagesWaitForDisplay; ///< A boolean indicating if the message is waiting for display.
        int y =0;                            ///< The y position of the chat message.

    public:
        /**
         * @brief Constructor for ClientChat.
         * @param clientSocket The client socket.
         */
        ClientChat() = default;

        /**
         *  @brief Launches the chat.
         */
        void run();

        /**
         * @brief Sends a chat message.
         * @param message The message to send.
         */
        void sendChatMessages(WINDOW *input_win);

        /**
         * @brief Receives a chat message.
         */
        void receiveChatMessages();

        /**
         * @brief Displays a chat message.
         * @param message The message to display.
         */
        void displayChatMessage(std::string sender, const std::string& message);

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

        /**
         * @brief sets the isPlaying boolean.
         * @param isPlaying The boolean to set.
         */
        void setIsPlaying(bool isPlaying);
};

#endif // CLIENTCHAT_HPP
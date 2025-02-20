#ifndef CLIENTCHAT_HPP
#define CLIENTCHAT_HPP

#include "client.hpp"
#include "../common/chatMessage.hpp"


class ClientChat : public Client {
    private:
        int clientSocket;
    public:
        ClientChat(int clientSocket);

        void sendChatMessage(const std::string& message);
        void receiveChatMessage();
        void displayChatMessage(std::string message);
        void initMessageMemory();
        void saveMessage(std::string message);
        void FlushMemory();

}

#endif // CLIENTCHAT_HPP
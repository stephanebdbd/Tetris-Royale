#ifndef CHATMANAGER_HPP
#define CHATMANAGER_HPP

#include "chat.hpp"
#include "User.hpp"
#include "../include.hpp"


class chatManager{
private:

    ServerChat chat;
    std::vector<User> usersRequest;
    
public:

    chatManager(ServerChat chat);
    void start();
    void stop();
    void addClient(std::string pseudoName, int socket);
    void removeClient(std::string pseudoName);
    void sendClientRequest(std::string pseudoName, std::string message);
    void acceptClientRequest(std::string pseudoName, int socket);

}

#endif //CHATMANAGER_HPP
#pragma once

#include "Client.hpp"
#include "ClientDisplay.hpp"  
#include "Controller.hpp"
#include "ClientNetwork.hpp"
#include "ClientChat.hpp"

class SFMLGame {
    Client& client;

    public:
        SFMLGame(Client& client);
        void run();
};

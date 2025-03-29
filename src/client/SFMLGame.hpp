#pragma once

#include "Client.hpp"

class SFMLGame {
public:
    SFMLGame(Client& client);
    void run();
private:
    Client& client;
};

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <netinet/in.h>
#include <iostream>
#include "../common/json.hpp"

using json = nlohmann::json;

class Controller {

    public:
        void sendInput(const std::string& action, int clientSocket);
};

#endif

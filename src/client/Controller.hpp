#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <string>
#include "../common/json.hpp"
#include <netinet/in.h>
#include <iostream>


class Controller {

    public:
        void sendInput(const std::string& action, int clientSocket);
};

#endif

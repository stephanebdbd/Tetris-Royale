#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <string>

class Controller {

    public:
        void sendInput(const std::string& action, int clientSocket);
};

#endif
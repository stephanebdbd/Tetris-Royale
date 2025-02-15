#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <nlohmann/json.hpp>

class ClientDisplay {

    public:
        void displayMenu(const nlohmann::json& data);
        void displayTetramino(const nlohmann::json& data);
        void displayGrid(const nlohmann::json& data);


        void drawGrid(int width, int height, const nlohmann::json& cells);
};

#endif

#include "ChatMessage.hpp"
#include <sstream>
#include <iomanip>

std::string ChatMessage::serialize() const {
    std::ostringstream oss;
    oss << senderId << "|" << channel << "|" << message;
    return oss.str();
}

ChatMessage ChatMessage::deserialize(const std::string& data) {
    std::istringstream iss(data);
    ChatMessage msg;
    std::getline(iss, msg.senderId, '|');
    std::getline(iss, msg.channel, '|');
    std::getline(iss, msg.message, '|');
    msg.timestamp = std::chrono::system_clock::now();
    return msg;
}

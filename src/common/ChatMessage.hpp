#pragma once
#include <string>
#include <chrono>

struct ChatMessage {
    std::string senderId;
    std::string message;
    std::string channel; // "global", "team", "private"
    std::chrono::system_clock::time_point timestamp;

    std::string serialize() const;
    static ChatMessage deserialize(const std::string& data);
};

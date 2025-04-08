#include <algorithm> 
#include "chat.hpp"
#include <iostream>
#include <sqlite3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <regex>
#include <sodium.h>


void Chat::processClientChat(int clientSocket, int clientId, std::string sender) {
        std::cout << "Chat process started for client " << clientSocket << std::endl;
        char buffer[1024];
        bool lumessage = false;  // Ajout de la variable lumessage pour contrôler l'historique
        clientId = clientId; // Utiliser clientId pour identifier le client

        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (bytes_received <= 0) {
                close(clientSocket);
                break;
            }

            try {
                json msg = json::parse(std::string(buffer, bytes_received));
                std::string chatTarget = msg["receiver"];

                if (msg.contains("receiver") && !chatTarget.empty() && msg.contains("message") && !msg["message"].is_null() && msg["message"] != "exit") {
                    msg["sender"] = sender;

                    // Charger l'historique des messages si ce n'est pas déjà fait
                    if (!lumessage) {
                        std::string previousMessages = getMsgBetweenUsers(sender, chatTarget);
                        json historiquejs = json::parse(previousMessages);
    
                        // Envoie chaque message précédent au client
                        //for (const auto& row : historiquejs) {
                            //sendMessage(clientSocket, row["sender"], row["message"], server.getRunningChat(clientSocket));
                        //}
                        lumessage = true;  // Une fois l'historique envoyé, on met lumessage à true
                    }

                    // Processus d'envoi et de réception des messages
                    std::string receiver = msg["receiver"];

                } else {
                    if (msg["message"] == "exit") {
                        break;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error (sending messages): " << e.what() << std::endl;
            }
        }
}



bool Chat::saveMessage(const std::string &sender, const std::string &receiver, const std::string &msg){
    // Get the string time the message was send (now)
    std::string time = getTime();

    // Insert the message in the db
    std::string clean_msg = std::regex_replace(msg, std::regex("'"), "''");    
    std::string columns = "sender, receiver, msg, msg_date_time";
    std::string values = "'" + sender + "', '" + receiver + "', '" + clean_msg + "', '" + time +  "'";
    QueryResult result = db->insertEntry("Messages", columns, values);
    return result.isOk();
}
std::string Chat::getMsgBetweenUsers(const std::string &id_user, const std::string &id_friend) {
    std::string columns = "sender, receiver, msg, msg_date_time";
    std::string condition = "(sender = '" + id_user + "' AND receiver = '" + id_friend + "')" \
                            " OR (sender = '" + id_friend + "' AND receiver = '" + id_user + "')";
    std::string order_by = " ORDER BY msg_date_time";   // Order by the msg_date_time column
    QueryResult result = db->selectFromTable("Messages", columns, condition + order_by);

    json messagesJson = json::array();

    for (const auto& row : result.getData()) {
        if (row.size() >= 4) {
            json message;
            message["sender"] = row[0];
            message["receiver"] = row[1];
            message["message"] = row[2];
            message["timestamp"] = row[3];
            messagesJson.push_back(message);
        }
    }

    return messagesJson.dump();  // Convert JSON array to string
}

QueryResult Chat::getAllUserMsg(const std::string &id_user){
    std::string columns = "sender, receiver, msg";
    std::string condition = "(sender = '" + id_user + "' OR receiver = '" + id_user + "')";
    std::string order_by = " ORDER BY (CASE WHEN sender = '" + id_user + "' THEN receiver ELSE sender END)";
    QueryResult result = db->selectFromTable("Messages", columns, condition + order_by);
    return result;
}

std::string Chat::getTime(){
    std::time_t now = std::time(nullptr);
    std::tm *tm_time = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", tm_time);
    return std::string(buffer);
}


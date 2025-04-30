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


bool Chat::processClientChat(int senderSocket, const std::string& sender, const std::map<std::string, int>& receiver, json& msg){
    (void)senderSocket; // Suppression de l'avertissement sur l'argument non utilisé
    try {
        
        if (msg["message"] != "/exit") {
            msg["sender"] = sender;
            sendMessage(msg, receiver);
            return true;
        }else{
            std::cout << "Le client a quitté la conversation." << std::endl;
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error (sending  messages): " << e.what() << std::endl;
    }
    return false;   
}


void Chat::sendMessage(json& msg, const std::map<std::string, int>& receiver) {
    //save the message in the db
    if(receiver.size() == 1){
        auto receiverOfMessage = receiver.begin()->first;
        // Enregistrement du message dans la base de données
        if (!saveMessage(msg["sender"], receiverOfMessage, msg["message"]))
            std::cerr << "Failed to save message: " << msg["sender"] << "  : " << msg["message"] << std::endl;
    }

    for (const auto& [receiverName, receiverSocket] : receiver) {
        // Envoi du message au socket du destinataire
        std::string jsonStr = msg.dump() + "\n"; // Convertir le message en chaîne JSON
        if(send(receiverSocket, jsonStr.c_str(), jsonStr.size(), 0) == -1) {
            std::cerr << "Erreur d'envoi du message au socket " << receiverSocket << std::endl;
        }
    }
}

void Chat::sendOldMessages(int senderSocket, const std::string& sender, const std::string& receiver) {
    std::string previousMessages = getMsgBetweenUsers(sender, receiver);
    
    if (!previousMessages.empty() && previousMessages != "[]") {
        send(senderSocket, previousMessages.c_str(), previousMessages.size(), 0);

    } else {
        std::cout << "Aucun message précédent à envoyer." << std::endl;
    }
}


bool Chat::saveMessage(const std::string &sender, const std::string &receiver, const std::string &msg){
    // Get the string time the message was send (now)
    std::string time = db->getTime();

    // Insert the message in the db
    std::string columns = "sender, receiver, msg, msg_date_time";
    std::string values = "'" + sender + "', '" + receiver + "', '" + msg + "', '" + time +  "'";
    QueryResult result = db->insertEntry("Messages", columns, values);
    return result.isOk();
}


std::string Chat::getMsgBetweenUsers(const std::string &id_user, const std::string &id_friend) {
    std::string columns = "sender, receiver, msg, msg_date_time";
    std::string condition = "(sender = '" + id_user + "' AND receiver = '" + id_friend + "')" \
                            " OR (sender = '" + id_friend + "' AND receiver = '" + id_user + "')";
    std::string order_by = " ORDER BY msg_date_time";
    QueryResult result = db->selectFromTable("Messages", columns, condition + order_by);

    json messagesJson = json::array();  // Initialisation du tableau JSON

    for (const auto& row : result.getData()) {
        if (row.size() >= 4) {
            json message;
            message["sender"] = row[0];
            message["receiver"] = row[1];
            message["message"] = row[2];
            message["msg_date_time"] = row[3];
            messagesJson.push_back(message);
        } else {
            std::cerr << "Ligne ignorée : pas assez de colonnes (attendu >= 4)" << std::endl;
        }
    }
    return messagesJson.dump()+"\n";  // renvoie une string du tableau JSON (vide ou non)
}

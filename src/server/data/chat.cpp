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


bool Chat::processClientChat(int senderSocket, const std::string& sender, const std::map<std::string, int>& receiver, json& msg) {
    (void)senderSocket;
    try {
        // Always override the sender with the authenticated name
        msg["sender"] = sender;  // This ensures "You" becomes "aa" or whatever the real username is
        
        if (msg["message"] != "/exit") {
            sendMessage(msg, receiver);
            return true;
        } else {
            std::cout << "Le client a quitté la conversation." << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error (sending messages): " << e.what() << std::endl;
    }
    return false;
}

void Chat::sendMessage(json& msg, const std::map<std::string, int>& receiver) {
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

QueryResult Chat::getUserId(const std::string &username) const{
    std::string condition = "username = '" + username + "'";
    QueryResult result = db->selectFromTable("Users", "id_user", condition);
    return result; // Retourne l'objet result avec les données si l'utilisateur existe
}


bool Chat::saveMessage(const std::string &sender, const std::string &receiver, const std::string &msg) {
    std::string time = db->getTime();
    
    const auto& senderReceiverId = getSenderReceiverId(sender, receiver);
    if(senderReceiverId.first.empty() || senderReceiverId.second.empty()) return false;
    std::string senderId = senderReceiverId.first;
    std::string receiverId = senderReceiverId.second;
    
    // Use parameterized queries if possible, or at least escape strings
    std::string escapedMsg;
    for (char c : msg) {
        if (c == '\'') escapedMsg += '\'';
        escapedMsg += c;
    }
    
    std::string columns = "id_sender, id_receiver, msg, msg_date_time";
    std::string values = "'" + senderId + "', '" + receiverId + "', '" + escapedMsg + "', '" + time + "'";
    QueryResult result = db->insertEntry("PrivateMessages", columns, values);
    
    if (!result.isOk()) {
        std::cerr << "DB Error saving message: " << result.getError() << std::endl;
    }
    return result.isOk();
}

std::pair<std::string, std::string> Chat::getSenderReceiverId(const std::string& sender, const std::string& receiver){
    std::pair<std::string, std::string> result;
    // Vérifier si les utilisateur existe
    QueryResult senderResult = getUserId(sender);
    QueryResult receiverResult = getUserId(receiver);
    if (!senderResult.isOk() || senderResult.data.empty() || !receiverResult.isOk() || receiverResult.data.empty()) {
        std::cerr << "Erreur : Utilisateur  introuvable.\n";
        return result;
    }

    result.first = senderResult.getFirst(); // Récupération de l'ID utilisateur
    result.second = receiverResult.getFirst(); // Récupération de l'ID utilisateur
    return result;
}

std::string Chat::getUserName(const std::string& userId) const{
    QueryResult result =  db->selectFromTable("Users", "username", "id_user = " + userId);
    if(!result.isOk())
        return "";
    return result.getFirst();
}

std::string Chat::getMsgBetweenUsers(const std::string &user, const std::string& Friend ) {
    const auto& senderReceiverId = getSenderReceiverId(user, Friend);
    if(senderReceiverId.first.empty() || senderReceiverId.second.empty()) return "";
    std::string userId = senderReceiverId.first;
    std::string friendId = senderReceiverId.second;
    std::cout << senderReceiverId.first << "   "  << senderReceiverId.second << std::endl;
    std::string columns = "id_sender, id_receiver, msg, msg_date_time";
    std::string condition = "(id_sender = '" + userId + "' AND id_receiver = '" + friendId + "')" \
                            " OR (id_sender = '" + friendId + "' AND id_receiver = '" + userId + "')";
    std::string order_by = " ORDER BY msg_date_time";
    QueryResult result = db->selectFromTable("PrivateMessages", columns, condition + order_by);

    json messagesJson = json::array();  // Initialisation du tableau JSON

    for (const auto& row : result.getData()) {
        if (row.size() >= 4) {
            json message;
            message["sender"] = getUserName(row[0]);
            message["receiver"] = getUserName(row[1]);
            message["message"] = row[2];
            message["msg_date_time"] = row[3];
            messagesJson.push_back(message);
        } else {
            std::cerr << "Ligne ignorée : pas assez de colonnes (attendu >= 4)" << std::endl;
        }
    }
    std::cout << "old messages :  " << messagesJson.dump()+"\n";
    return messagesJson.dump()+"\n";  // renvoie une string du tableau JSON (vide ou non)
}

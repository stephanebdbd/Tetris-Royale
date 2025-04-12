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

void Chat::sendHistory(int receiversocket, const json& historique) {
    std::string jsonStr = historique.dump() + "\n";  // Historique = tableau JSON
    send(receiversocket, jsonStr.c_str(), jsonStr.size(), 0);
}
/*void Chat::processRoomChat(int senderSocket, const std::string& sender, const std::string& roomName, std::shared_ptr<std::vector<int>> roomSockets) {
    std::cout << "Room chat started in room: " << roomName << std::endl;
    
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(senderSocket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            std::cerr << "Client disconnected from room or error occurred." << std::endl;
            close(senderSocket);
            break;
        }

        try {
            json msg = json::parse(std::string(buffer, bytes_received));

            if (msg.is_object() && msg.contains("message")) {
                if (msg["message"] == "exit") {
                    std::cout << sender << " a quitté la room " << roomName << std::endl;
                    break;
                }

                msg["sender"] = sender;

                std::string serializedMsg = msg.dump()+ "\n";

                // Envoyer le message à tous les membres sauf l'expéditeur
                for (int socket : *roomSockets) {
                    if (socket != senderSocket) {
                        send(socket, serializedMsg.c_str(), serializedMsg.size(), 0);
                    }
                }
            } else {
                std::cerr << "JSON invalide reçu dans room chat." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Erreur (room chat) : " << e.what() << std::endl;
        }
    }
}
*/
void Chat::processClientChat(int receiversocket, int sendersocket,  const std::string& sender, const std::string& receiver) {
    std::cout << "Chat process started for client " << receiversocket << std::endl;
    char buffer[1024];
    bool lumessage = false; // Variable pour savoir si l'historique a été envoyé
    if (!lumessage) {
        std::string previousMessages = getMsgBetweenUsers(sender, receiver);
    
        if (!previousMessages.empty() && previousMessages != "[]") {
            send(receiversocket, previousMessages.c_str(), previousMessages.size(), 0);
            send(sendersocket, previousMessages.c_str(), previousMessages.size(), 0);

        } else {
            std::cout << "Aucun message précédent à envoyer." << std::endl;
        }
    
        lumessage = true;
    }   
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sendersocket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            close(sendersocket);
            break;
        }

        try {
            json msg = json::parse(std::string(buffer, bytes_received));
        
            if (msg.is_object() && msg.contains("message")) {
                if (msg["message"] != "exit") {
                    msg["sender"] = sender;
                    msg["receiver"] = receiver;
                    sendMessage(receiversocket,msg);
                } else {
                    break;
                }
            } else {
                std::cerr << "Reçu JSON invalide ou pas un objet avec 'message'" << std::endl;
            }
        
        } catch (const std::exception& e) {
            std::cerr << "Error (sending  messages): " << e.what() << std::endl;
        }
        
    }
}
void Chat::sendMessage(int receiversocket, const json& msg) {

    if (saveMessage(msg["sender"], msg["receiver"], msg["message"])) {
        std::cout << "Message saved: " << msg["message"] << std::endl;
    } else {
        std::cerr << "Failed to save message: " << msg["message"] << std::endl;
    }
    std::string jsonStr = msg.dump() + "\n";

    std::cout << "Sending message: " << jsonStr << std::endl;
    send(receiversocket, jsonStr.c_str(), jsonStr.size(), 0);


}




bool Chat::saveMessage(const std::string &sender, const std::string &receiver, const std::string &msg){
    // Get the string time the message was send (now)
    std::string time = db->getTime();

    // Insert the message in the db
    std::string columns = "sender, receiver, msg, msg_date_time";
    std::string values = "'" + sender + "', '" + receiver + "', '" + msg + "', '" + time +  "'";
    QueryResult result = db->insertEntry("Messages", columns, values);
    std::cout<<result.getError()<<std::endl;
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

    if (messagesJson.empty()) {
        std::cout << "Aucun message trouvé entre " << id_user << " et " << id_friend << std::endl;
    }
    std::cout << messagesJson.dump()+"\n" << std::endl;

    return messagesJson.dump()+"\n";  // renvoie une string du tableau JSON (vide ou non)
}


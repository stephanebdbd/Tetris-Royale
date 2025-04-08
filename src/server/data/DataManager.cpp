#include "DataManager.hpp"
#include "database.hpp"
#include "security.hpp"
#include <iostream>

QueryResult DataManager::getUsername(const std::string &id_user) {
    std::string condition = "id_user = '" + id_user + "'";
    QueryResult result = db->selectFromTable("Users", "username", condition);
    return result;
}
bool DataManager::authenticateUser(const std::string& username, const std::string& password) {
    // Vérifier si l'utilisateur existe
    QueryResult userResult = getUserId(username);
    if (!userResult.isOk() || userResult.data.empty()) {
        std::cerr << "Erreur : Utilisateur " << username << " introuvable.\n";
        return false;
    }

    std::string userId = userResult.getFirst(); // Récupération de l'ID utilisateur

    // Vérifier le mot de passe
    if (!checkPwd(userId, password)) {
        std::cerr << "Erreur : Mot de passe incorrect pour l'utilisateur " << username << ".\n";
        return false;
    }

    return true;
}

QueryResult DataManager::getUserId(const std::string &username) {
    std::string condition = "username = '" + username + "'";
    QueryResult result = db->selectFromTable("Users", "id_user", condition);
    return result; // Retourne l'objet result avec les données si l'utilisateur existe
}

bool DataManager::checkPwd(const std::string &id_user, const std::string &pwd) {
    std::string columns = "hash_pwd, salt";
    std::string condition = "id_user = '" + id_user + "'";
    QueryResult result = db->selectFromTable("Users", columns, condition);

    if (!result.isOk()|| result.data.empty()) {
        return false; // L'ID utilisateur n'existe pas
    }

    std::string hash_pwd = result.data[0][0];
    std::string salt = result.data[0][1];
    std::string salt_pwd = pwd + salt;

    return Security::verifyPwd(hash_pwd, salt_pwd); // Vérification du mot de passe
}

bool DataManager::loginUser(const std::string &username, const std::string &pwd) {
    QueryResult result = getUserId(username);
    if (result.isOk()) {
        return checkPwd(result.getFirst(), pwd);
    }
    return false;
}

bool DataManager::registerUser(const std::string &username, const std::string &pwd) {
    std::string salt = Security::genSalt();
    std::string hashpwd = Security::hashPwd(pwd + salt);
    std::string columns = "username, hash_pwd, salt";
    std::string values = "'" + username + "', '" + hashpwd + "', '" + salt + "'";
    QueryResult result = db->insertEntry("Users", columns, values);
    return result.isOk();
}


bool DataManager::sendFriendRequest(const std::string& sender, const std::string& receiver) {
    // Récupérer les ID des utilisateurs
    QueryResult senderId = getUserId(sender);
    QueryResult receiverId = getUserId(receiver);

    // Vérification si les utilisateurs existent
    if (!senderId.isOk()|| senderId.data.empty()) {
        return false; 
    }

    if (!receiverId.isOk()|| receiverId.data.empty()) {
        return false; // Utilisateur "sender" inexistant
    }

    // Vérification si une relation existe déjà
    std::string condition = "(id_sender = '" + senderId.getFirst() + "' AND id_receiver = '" + receiverId.getFirst() + "') "
                            "OR (id_sender = '" + receiverId.getFirst() + "' AND id_receiver = '" + senderId.getFirst() + "')";
    
    QueryResult checkExisting = db->selectFromTable("Friendships", "status", condition);

    if (!checkExisting.data.empty()) {
        return false; // Une relation existe déjà entre ces deux utilisateurs
    }

    // Insertion de la nouvelle demande d'ami
    std::string columns = "id_sender, id_receiver, status";
    std::string values = "'" + senderId.getFirst() + "', '" + receiverId.getFirst() + "', 'pending'";

    QueryResult result = db->insertEntry("Friendships", columns, values);

    return result.isOk(); // Retourne vrai si l'insertion a réussi, sinon faux
}


bool DataManager::acceptFriendRequest(const std::string& receiver, const std::string& sender) {
    // Récupérer les IDs des utilisateurs
    QueryResult senderId = getUserId(sender);
    QueryResult receiverId = getUserId(receiver);

    if (senderId.data.empty() || receiverId.data.empty()) {
        return false; // L'un des utilisateurs n'existe pas
    }

    // Construire la condition pour vérifier l'existence d'une demande d'ami en attente
    std::string condition = "id_sender = '" + senderId.getFirst() + "' AND id_receiver = '" + receiverId.getFirst() + "' AND status = 'pending'";

    // Vérifier si la demande d'ami existe
    QueryResult checkRequest = db->selectFromTable("Friendships", "status", condition);

    if (checkRequest.data.empty()) {
        return false; // Aucune demande d'ami en attente
    }

    // Mettre à jour le statut de la demande d'ami
    std::string updateValues = "status = 'accepted'";
    QueryResult result = db->updateEntry("Friendships", updateValues, condition);

    return result.isOk(); // Retourner true si l'opération est réussie, sinon false
}

bool DataManager::rejectFriendRequest(const std::string& receiver, const std::string& sender) {
    // Vérifier si les utilisateurs existent
    QueryResult senderId = getUserId(sender);
    QueryResult receiverId = getUserId(receiver);

    if (senderId.data.empty() || receiverId.data.empty()) {
        return false; // L'un des utilisateurs n'existe pas
    }

    // Vérifier si une demande d'ami en attente existe
    std::string condition = "id_sender = '" + senderId.getFirst() + "' AND id_receiver = '" + receiverId.getFirst() + "' AND status = 'pending'";
    QueryResult checkRequest = db->selectFromTable("Friendships", "status", condition);

    if (checkRequest.data.empty()) {
        return false; // Aucune demande d'ami en attente
    }

    // Supprimer complètement la demande d'ami
    return deleteFriend(sender, receiver);
}


bool DataManager::areFriends(const std::string& user1, const std::string& user2) {
    // Récupération des IDs des utilisateurs
    QueryResult user1Result = getUserId(user1);
    QueryResult user2Result = getUserId(user2);

    // Vérifier si les utilisateurs existent
    if (user1Result.data.empty() || user2Result.data.empty()) {
        return false;  // L'un des utilisateurs (ou les deux) n'existe pas
    }

    std::string user1Id = user1Result.getFirst();
    std::string user2Id = user2Result.getFirst();

    // Condition pour vérifier s'ils sont amis
    std::string condition =
        "(id_sender = '" + user1Id + "' AND id_receiver = '" + user2Id + "') "
        "OR (id_sender = '" + user2Id + "' AND id_receiver = '" + user1Id + "') "
        "AND status = 'accepted'";

    // Vérification dans la base de données
    QueryResult checkRequest = db->selectFromTable("Friendships", "COUNT(*)", condition);

    // Vérifier si le résultat contient un nombre supérieur à 0
    return (!checkRequest.data.empty() && checkRequest.getFirst() != "0");
}

bool DataManager::userExists(const std::string& username) const {
    // Construire la condition SQL
    std::string condition = "username = '" + username + "'";

    // Exécuter la requête pour vérifier si l'utilisateur existe
    QueryResult result = db->selectFromTable("Users", "COUNT(*)", condition);

    // Vérifier si le COUNT(*) est différent de "0", ce qui signifie que l'utilisateur existe
    return result.getFirst() != "0";
}

bool DataManager::userNotExists(const std::string& username) const {
    return !userExists(username);  // Inverse de userExists
}

std::vector<std::string> DataManager::getList(const std::string& user, const std::string& status) {
    std::vector<std::string> requestList;

    // Récupérer l'ID de l'utilisateur
    QueryResult userIdResult = getUserId(user);
    if (userIdResult.data.empty()) {
        return requestList; // Retourne une liste vide si l'utilisateur n'existe pas
    }
    std::string userId = userIdResult.getFirst();

    // Construire la condition pour récupérer les relations avec le bon statut
    std::string condition = 
        "(id_sender = '" + userId + "' OR id_receiver = '" + userId + "') "
        "AND status = '" + status + "'";

    // Récupérer les IDs des amis ou demandes d'amis selon le statut
    QueryResult result = db->selectFromTable("Friendships", "id_sender, id_receiver", condition);

    if (result.data.empty()) {
        return requestList; // Retourne une liste vide si aucun ami/demande trouvée
    }

    // Parcourir les résultats et récupérer les amis ou demandeurs
    for (const auto& row : result.data) {
        std::string id1 = row[0];
        std::string id2 = row[1];

        // Déterminer qui est l'ami ou l'expéditeur de la demande
        std::string friendId = (id1 == userId) ? id2 : id1;

        // Convertir l'ID en pseudo
        QueryResult friendUsername = db->selectFromTable("Users", "username", "id_user = '" + friendId + "'");
        if (!friendUsername.data.empty()) {
            requestList.push_back(friendUsername.getFirst());
        }
    }

    return requestList;
}
bool DataManager::deleteFriend(const std::string& user1, const std::string& user2) {
    // Récupération des IDs des utilisateurs
    QueryResult user1Result = getUserId(user1);
    QueryResult user2Result = getUserId(user2);

    // Vérifier si les utilisateurs existent
    if (user1Result.data.empty() || user2Result.data.empty()) {
        return false;  // L'un des utilisateurs (ou les deux) n'existe pas
    }

    std::string user1Id = user1Result.getFirst();
    std::string user2Id = user2Result.getFirst();

    // Condition pour supprimer la relation d'amitié
    std::string condition =
        "(id_sender = '" + user1Id + "' AND id_receiver = '" + user2Id + "') "
        "OR (id_sender = '" + user2Id + "' AND id_receiver = '" + user1Id + "')";

    // Suppression de l'entrée dans la base de données
    QueryResult result = db->deleteEntry("Friendships", condition);

    return result.isOk();
}



// Obtenir la liste des amis confirmés
std::vector<std::string> DataManager::getFriendList(const std::string& user) {
    return getList(user, "accepted");
}

// Obtenir la liste des demandes d'amis en attente
std::vector<std::string> DataManager::getRequestList(const std::string& user) {
    return getList(user, "pending");
}

bool DataManager::sendMsg(const std::string &sender, const std::string &receiver, const std::string &msg){
    // Get the string time the message was send (now)
    std::string time = getTime();

    // Insert the message in the db
    std::string clean_msg = std::regex_replace(msg, std::regex("'"), "''");     // Escape single quotes in the message
    std::string columns = "sender, receiver, msg, msg_date_time";
    std::string values = "'" + sender + "', '" + receiver + "', '" + clean_msg + "', '" + time +  "'";
    QueryResult result = db->insertEntry("Messages", columns, values);
    return result.isOk();
}


QueryResult DataManager::getMsgBetweenUsers(const std::string &id_user, const std::string &id_friend){
    std::string columns = "sender, receiver, msg";
    std::string condition = "(sender = '" + id_user + "' AND receiver = '" + id_friend + "')"\
                            " OR (sender = '" + id_friend + "' AND receiver = '" + id_user + "')";
    std::string order_by = " ORDER BY msg_date_time";   // Order by the msg_date_time column
    QueryResult result = db->selectFromTable("Messages", columns, condition + order_by);
    return result;
}


QueryResult DataManager::getAllUserMsg(const std::string &id_user){
    std::string columns = "sender, receiver, msg";
    std::string condition = "(sender = '" + id_user + "' OR receiver = '" + id_user + "')";
    std::string order_by = " ORDER BY (CASE WHEN sender = '" + id_user + "' THEN receiver ELSE sender END)";
    QueryResult result = db->selectFromTable("Messages", columns, condition + order_by);
    return result;
}

std::string DataManager::getTime(){
    std::time_t now = std::time(nullptr);
    std::tm *tm_time = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", tm_time);
    return std::string(buffer);
}


QueryResult DataManager::updateUserName(const std::string &id_user, const std::string &pwd, const std::string &new_username){
    QueryResult result;
    // Check if the provided password is correct
    if (checkPwd(id_user, pwd)){
        // Update the username for the specified user
        std::string set_clause = "username = '" + new_username + "'";
        std::string condition = "id_user = '" + id_user + "'";
        result = db->updateEntry("Users", set_clause, condition);
    }
    return result;
}


QueryResult DataManager::updatePwd(const std::string &id_user, const std::string &pwd, const std::string &new_pwd) {
    QueryResult result;
    // Check if the password is correct
    if (checkPwd(id_user, pwd)) {
        // Update the password for the given user
        std::string salt = Security::genSalt();
        std::string nhpwd = Security::hashPwd(new_pwd+salt);
        std::string set_clause = "hash_pwd = '" + nhpwd + "', salt = '" + salt + "'";
        std::string condition = "id_user = '" + id_user + "'";
        result = db->updateEntry("Users", set_clause, condition);
    }
    return result;
}





QueryResult DataManager::deleteAccount(const std::string &id_user, const std::string &pwd) {
    QueryResult result;
    // Check if the provided password is correct
    if (checkPwd(id_user, pwd)) {
        // Delete the user's account
        std::string condition = "id_user = '" + id_user + "'";
        result = db->deleteEntry("Users", condition);
    }
    return result;
}


QueryResult DataManager::getUserFriends(const std::string &id_user){
    std::string condition = "id_user_r = '" + id_user + "'";
    QueryResult result = db->selectFromTable("Relations", "id_friend", condition);
    return result;
}


bool DataManager::addGameState(const std::string &id_player1, const std::string &id_player2,const std::string &id_session, const std::string &game_state){
    std::string time = getTime();
    std::string columns = "id_player1, id_player2, id_session, game_state, state_date_time";
    std::string values = "'" + id_player1 + "', '" + id_player2 + "', '" + id_session + "', '" + game_state + "', '" + time + "'";
    QueryResult result = db->insertEntry("GameStates", columns, values);
    if(result.isOk()){return true;}else{std::cout << result.getError() << std::endl;}
    return false;
}

QueryResult DataManager::getGameStates(const std::string &id_session){
    std::string columns = "game_state";
    std::string condition = "(id_session = '" + id_session + "')";
    std::string order_by = " ORDER BY state_date_time";
    QueryResult result = db->selectFromTable("GameStates", columns, condition + order_by);
    return result;
}

QueryResult DataManager::getSessionId(const std::string &id_player){
    std::string columns = "DISTINCT id_session, id_player1, id_player2";
    std::string condition = "(id_player1 = '" + id_player + "' OR id_player2 = '" + id_player + "')";    
    std::string order_by = " ORDER BY state_date_time";
    QueryResult result = db->selectFromTable("GameStates", columns, condition + order_by);
    return result;
}

QueryResult DataManager::deleteGameStates(const std::string &id_session){
    std::string condition = "id_session = '" + id_session + "'";
    QueryResult result = db->deleteEntry("GameStates", condition);
    return result;
}

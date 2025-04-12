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
    // Empêcher l'envoi de demande à soi-même
    if (sender == receiver) return false;

    // Récupération des IDs
    QueryResult senderIdResult = getUserId(sender);
    QueryResult receiverIdResult = getUserId(receiver);

    if (!senderIdResult.isOk() || senderIdResult.data.empty() ||
        !receiverIdResult.isOk() || receiverIdResult.data.empty()) {
        return false; // Un des utilisateurs n'existe pas
    }

    std::string senderId = senderIdResult.getFirst();
    std::string receiverId = receiverIdResult.getFirst();

    // Vérifie si une relation existe déjà (dans les deux sens)
    std::string condition = 
        "(id_sender = '" + senderId + "' AND id_receiver = '" + receiverId + "') OR "
        "(id_sender = '" + receiverId + "' AND id_receiver = '" + senderId + "')";

    QueryResult existing = db->selectFromTable("Friendships", "status", condition);

    if (!existing.data.empty()) {
        return false; // Déjà amis ou demande déjà envoyée/reçue
    }

    // Envoi de la demande d'ami
    std::string columns = "id_sender, id_receiver, status";
    std::string values = "'" + senderId + "', '" + receiverId + "', 'pending'";

    QueryResult insertResult = db->insertEntry("Friendships", columns, values);

    return insertResult.isOk();
}


bool DataManager::acceptFriendRequest(const std::string& receiver, const std::string& sender) {
    // Récupérer les IDs des utilisateurs
    QueryResult senderIdResult = getUserId(sender);
    QueryResult receiverIdResult = getUserId(receiver);

    if (senderIdResult.data.empty() || receiverIdResult.data.empty()) {
        return false;
    }

    std::string senderId = senderIdResult.getFirst();
    std::string receiverId = receiverIdResult.getFirst();

    // Vérifie l'existence d'une demande d'ami en attente
    std::string condition = 
        "id_sender = '" + senderId + "' AND id_receiver = '" + receiverId + "' AND status = 'pending'";

    QueryResult checkRequest = db->selectFromTable("Friendships", "status", condition);
    if (checkRequest.data.empty()) {
        return false;
    }

    // Mettre à jour le statut à 'accepted'
    QueryResult update = db->updateEntry("Friendships", "status = 'accepted'", condition);
    return update.isOk();
}

bool DataManager::rejectFriendRequest(const std::string& receiver, const std::string& sender) {
    // Récupérer les IDs des utilisateurs
    QueryResult senderIdResult = getUserId(sender);
    QueryResult receiverIdResult = getUserId(receiver);

    if (senderIdResult.data.empty() || receiverIdResult.data.empty()) {
        return false;
    }

    std::string senderId = senderIdResult.getFirst();
    std::string receiverId = receiverIdResult.getFirst();

    // Vérifie l'existence de la demande
    std::string condition = 
        "id_sender = '" + senderId + "' AND id_receiver = '" + receiverId + "' AND status = 'pending'";

    QueryResult checkRequest = db->selectFromTable("Friendships", "status", condition);
    if (checkRequest.data.empty()) {
        return false;
    }

    // Supprime la demande
    return db->deleteEntry("Friendships", condition).isOk();
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
void printList(const std::vector<std::string>& list) {
    if (list.empty()) {
        std::cout << "La liste est vide." << std::endl;
    } else {
        for (const auto& username : list) {
            std::cout << username << std::endl;
        }
    }
}

std::vector<std::string> DataManager::getList(const std::string& user, const std::string& status) {
    std::vector<std::string> list;

    QueryResult userIdResult = getUserId(user);
    if (userIdResult.data.empty()) return list;

    std::string userId = userIdResult.getFirst();

    std::string sql =
        "SELECT u.username "
        "FROM Friendships f "
        "JOIN Users u ON ("
            "(u.id_user = f.id_sender AND f.id_receiver = '" + userId + "') OR "
            "(u.id_user = f.id_receiver AND f.id_sender = '" + userId + "')"
        ") "
        "WHERE f.status = '" + status + "';";

    QueryResult result = db->executeQuery(sql);

    for (const auto& row : result.getData()) {
        if (!row.empty()) list.push_back(row[0]);  // username
    }
    printList(list);
    return list;
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
    std::cout << " yaw yaw yaw yaw  aw" << std::endl;
    return getList(user, "pending");
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
    std::string time = db->getTime();
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

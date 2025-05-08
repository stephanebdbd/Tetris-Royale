#include "UserManager.hpp"
#include "Database.hpp"
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

int DataManager::getUserAvatarId(const std::string &username){
    std::string condition = "username = '" + username + "'";
    QueryResult result = db->selectFromTable("Users", "id_avatar", condition);
    if(!result.getData().empty())
        return std::stoi(result.getFirst());
    return -1;
}

bool DataManager::checkPwd(const std::string &id_user, const std::string &pwd) {
    std::string columns = "passwrd";
    std::string condition = "id_user = '" + id_user + "'";
    QueryResult result = db->selectFromTable("Users", columns, condition);

    if (!result.isOk()|| result.data.empty()) {
        return false; // L'ID utilisateur n'existe pas
    }

    std::string password = result.data[0][0];
    return password == pwd;
}

bool DataManager::loginUser(const std::string &username, const std::string &pwd) {
    QueryResult result = getUserId(username);
    if (result.isOk()) {
        return checkPwd(result.getFirst(), pwd);
    }
    return false;
}

bool DataManager::registerUser(const std::string &username, const std::string &pwd, const int& avatrId) {
  
    std::string columns = "username, passwrd, id_avatar";
    std::string values = "'" + username + "', '" + pwd + "', '" + std::to_string(avatrId) + "'";
    QueryResult result = db->insertEntry("Users", columns, values);
    return result.isOk();
}


bool DataManager::sendFriendRequest(const std::string& sender, const std::string& receiver) {
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
        "(id_sender = '" + senderId + "' AND id_receiver = '" + receiverId + "' AND status = 'pending')";
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

QueryResult DataManager::getDataFromTable(
    const std::string& table,
    const std::string& columns,
    const std::string& joinTables,
    const std::string& joinConditions,
    const std::string& whereConditions)
{
    std::string sql = "SELECT " + columns + " FROM " + table;
    
    if (!joinTables.empty()) {
        sql += " JOIN " + joinTables;
        if (!joinConditions.empty()) {
            sql += " ON " + joinConditions;
        }
    }
    
    if (!whereConditions.empty()) {
        sql += " WHERE " + whereConditions;
    }
    
    sql += ";";
    
    return db->executeQuery(sql);
}

std::vector<std::string> DataManager::getUserListByStatus(const std::string& user, const std::string& status) {
    std::vector<std::string> list;
    
    // Obtenir l'ID de l'utilisateur
    QueryResult userIdResult = getUserId(user);
    if (userIdResult.data.empty()) {
        return list;
    }
    
    std::string userId = userIdResult.getFirst();
    
    // Construire la condition de jointure
    std::string joinCondition = "((u.id_user = f.id_sender AND f.id_receiver = '" + userId + "')";
    if (status == "accepted") {
        joinCondition += "OR (u.id_user = f.id_receiver AND f.id_sender = '" + userId + "')";
    }
    joinCondition += ")";
    // Exécuter la requête
    QueryResult result = getDataFromTable(
        "Friendships f",     // table
        "DISTINCT u.username", // columns (DISTINCT pour éviter les doublons)
        "Users u",           // joinTables
        joinCondition,       // joinConditions
        "f.status = '" + status + "'" // whereConditions
    );
    
    // Remplir la liste des résultats
    for (const auto& row : result.getData()) {
        if (!row.empty()) {
            list.push_back(row[0]); // username
        }
    }
    
    return list;
}

// Obtenir la liste des amis confirmés
std::vector<std::string> DataManager::getFriendList(const std::string& user) {
    return getUserListByStatus(user, "accepted");
}

// Obtenir la liste des demandes d'amis en attente
std::vector<std::string> DataManager::getRequestList(const std::string& user) {
    return getUserListByStatus(user, "pending");
}

QueryResult DataManager::updateUserName(const std::string &id_user, const std::string &pwd, const std::string &new_username){
    QueryResult result;
    // Check si le mot de passe est correct
    if (checkPwd(id_user, pwd)){
        // Mettre à le jour le pseudo pour l'utilisateur
        std::string set_clause = "username = '" + new_username + "'";
        std::string condition = "id_user = '" + id_user + "'";
        result = db->updateEntry("Users", set_clause, condition);
    }
    return result;
}


QueryResult DataManager::updatePwd(const std::string &id_user, const std::string &pwd, const std::string &new_pwd) {
    QueryResult result;
    // Check si le mot de passe est correct
    if (checkPwd(id_user, pwd)) {
        // Mettre à jour le mdp pour l'utilisateur

        std::string set_clause = "passwrd = '" + new_pwd + "'";
        std::string condition = "id_user = '" + id_user + "'";
        result = db->updateEntry("Users", set_clause, condition);
    }
    return result;
}

bool DataManager::sendInvitationToFriend(const int& gameRoom, const std::string& sender, const std::string& player, const std::string& invitation_type){
    QueryResult senderIdResult = getUserId(sender);
    QueryResult playerIdResult = getUserId(player);
    if(!playerIdResult.isOk()) return false;
    std::string senderId = senderIdResult.getFirst();
    std::string playerId = playerIdResult.getFirst();
    //verfier si une invitation a été déja envoyer a ce joueur
    std::string condition = "(id_game = '" + std::to_string(gameRoom) + "' AND id_player = '"+ playerId + "' And invitation_type = '" + invitation_type + "')";
    QueryResult existing = db->selectFromTable("Games", "status", condition);
    if(!existing.data.empty()) return false;
    // Envoi de l'invitaion au joueur
    std::string columns = "id_game, id_sender, id_player, invitation_type, status";
    std::string values = "'" + std::to_string(gameRoom) + "', '" + senderId + "', '" + playerId + "', '" + invitation_type + "', 'pending'";
    QueryResult insertResult = db->insertEntry("Games", columns, values);
    std::cout << "game inviation sent to " << player << std::endl;
    return insertResult.isOk();
}

bool DataManager::acceptGameInvitation(const int& gameRoom, const std::string& player){
    QueryResult playerIdResult = getUserId(player);
    if (playerIdResult.data.empty()) return false;

    std::string playerId = playerIdResult.getFirst();
    std::string condition = "id_game = '" + std::to_string(gameRoom) + "' AND id_player = '" + playerId + "' AND status = 'pending'";
    QueryResult result = db->deleteEntry("Games", condition);
    return result.isOk();
}

std::vector<std::vector<std::string>> DataManager::getListGameRequest(const std::string& user){
    std::vector<std::vector<std::string>> gamesInviation;
    QueryResult userIdResult = getUserId(user);
    if (userIdResult.data.empty()) return gamesInviation;
    std::string userId = userIdResult.getFirst();
    QueryResult result = getDataFromTable(
        "Games g",
        "u.username, g.invitation_type, g.id_game",
        "Users u",
        "u.id_user = g.id_sender",
        "g.id_player = " + userId + " AND g.status = 'pending'"
    );
    for (const auto& row : result.getData()) {
        if (!row.empty()) gamesInviation.push_back(row);  // pseudo
    }
    return gamesInviation;
}


QueryResult DataManager::updateHighScore(const std::string& username, const int& bestScore){
    QueryResult result;
    // Màj du meilleur score
    std::string set_clause = "best_score = '" + std::to_string(bestScore) + "'";
    std::string condition = "username = '" + username + "'";
    result = db->updateEntry("Users", set_clause, condition);
    return result;
}


std::map<std::string, std::vector<std::string>> DataManager::getRanking() const {
    std::map<std::string, std::vector<std::string>> ranking; // username -> [score, avatarName]
    QueryResult result = db->executeQuery(
        "SELECT username, best_score, id_avatar FROM Users ORDER BY best_score DESC;"
    );

    for (const auto& row : result.getData()) {
        if (!row.empty()) {
            std::string username = row[0];
            std::string bestScore = row[1]; 
            std::string avatarId = row[2]; 

            // Ajouter les données dans la map
            ranking[username] = {bestScore, avatarId};
        }
    }
    return ranking;
}
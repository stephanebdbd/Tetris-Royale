#include "chatRoom.hpp"
#include <algorithm> 

bool ChatRoom::createRoom(const std::string &room_name, const std::string &admin_pseudo) {
    // Récupérer l'id de l'admin à partir du pseudo
    std::string condition = "username = '" + admin_pseudo + "'";
    QueryResult idAdmin = db->selectFromTable("Users", "id_user", condition);
    if (!idAdmin.isOk() || idAdmin.getFirst().empty()) {
        std::cout << "Error: " << idAdmin.getError() << std::endl;
        return false;
    }

    // Créer la salle dans la table ChatRooms
    std::string columns = "room_name";
    std::string values = "'" + room_name + "'";
    QueryResult result = db->insertEntry("ChatRooms", columns, values);
    if (!result.isOk()) {
        std::cout << "Error creating room: " << result.getError() << std::endl;
        return false;
    }
    std::string roomId = db->selectFromTable("ChatRooms", "id_room", "room_name = '" + room_name + "'").getFirst();

    // Ajouter l'admin dans la table ChatRoomMembers
    columns = "id_room, id_user, status";
    values = roomId + ", '" + idAdmin.getFirst() + "', 'admin'";
    return db->insertEntry("ChatRoomMembers", columns, values).isOk();
}

bool ChatRoom::isAdmin(const std::string& pseudo, const std::string& room_name) const {
    // Récupérer l'ID de la salle en utilisant son nom
    std::string condition = "room_name = '" + room_name + "'";
    std::string roomId = db->selectFromTable("ChatRooms", "id_room", condition).getFirst();

    // Vérifier si la salle existe
    if (roomId.empty()) {
        std::cout << "Erreur : La salle '" << room_name << "' n'existe pas." << std::endl;
        return false;
    }

    // Construire la requête pour vérifier si l'utilisateur est admin dans la salle
    condition = "cr.room_name = '" + room_name + "' AND u.username = '" + pseudo + "' AND crm.status = 'admin'";
    std::string sql = 
        "SELECT crm.id_user "
        "FROM ChatRoomMembers crm "
        "JOIN Users u ON crm.id_user = u.id_user "
        "JOIN ChatRooms cr ON crm.id_room = cr.id_room "
        "WHERE " + condition;

    // Exécuter la requête
    QueryResult result = db->executeQuery(sql);

    // Si la requête réussit et qu'un résultat est trouvé, l'utilisateur est admin
    if (result.isOk()) {
        return !result.getFirst().empty();
    } else {
        std::cout << "Erreur : Impossible de récupérer les informations d'administration pour la salle '" << room_name << "'." << std::endl;
        return false;
    }
}


bool ChatRoom::isClient(const std::string& pseudo, const std::string& room_name) const {
    std::vector<std::string> members = getMembers(room_name);
    return std::find(members.begin(), members.end(), pseudo) != members.end();
}


bool ChatRoom::isPendingReq(const std::string& pseudo, const std::string& room_name) const {
    // Récupérer l'ID de l'utilisateur à partir du pseudo
    std::string condition = "username = '" + pseudo + "'";
    QueryResult idUserResult = db->selectFromTable("Users", "id_user", condition);
    
    if (!idUserResult.isOk() || idUserResult.getFirst().empty()) {
        std::cout << "Erreur : Utilisateur introuvable." << std::endl;
        return false;
    }
    std::string idUser = idUserResult.getFirst();

    // Récupérer l'ID de la salle à partir du nom
    condition = "room_name = '" + room_name + "'";
    QueryResult idRoomResult = db->selectFromTable("ChatRooms", "id_room", condition);
    
    if (!idRoomResult.isOk() || idRoomResult.getFirst().empty()) {
        std::cout << "Erreur : Salle de discussion introuvable." << std::endl;
        return false;
    }
    std::string idRoom = idRoomResult.getFirst();

    // Vérifier si l'utilisateur a une demande en attente dans cette salle
    condition = "id_room = " + idRoom + " AND id_user = " + idUser + " AND status = 'pending'";
    QueryResult pendingCheck = db->selectFromTable("ChatRoomMembers", "status", condition);
    
    return pendingCheck.isOk() && !pendingCheck.getFirst().empty();    
}


bool ChatRoom::addAdmin(const std::string& client_pseudo, const std::string& room_name) {
    // Récupérer l'id de l'utilisateur cible
    std::string condition = "username = '" + client_pseudo + "'";
    QueryResult idUserResult = db->selectFromTable("Users", "id_user", condition);
    
    if (!idUserResult.isOk() || idUserResult.getFirst().empty()) {
        std::cout << "Erreur : Utilisateur cible introuvable." << std::endl;
        return false;
    }
    std::string idUser = idUserResult.getFirst();

    // Récupérer l'id de la salle
    condition = "room_name = '" + room_name + "'";
    QueryResult idRoomResult = db->selectFromTable("ChatRooms", "id_room", condition);
    
    if (!idRoomResult.isOk() || idRoomResult.getFirst().empty()) {
        std::cout << "Erreur : Salle de discussion introuvable." << std::endl;
        return false;
    }
    std::string idRoom = idRoomResult.getFirst();

    // Vérifier si le membre est déjà dans la salle avec un statut accepté
    condition = "id_room = " + idRoom + " AND id_user = " + idUser + " AND status = 'member'";
    QueryResult memberCheck = db->selectFromTable("ChatRoomMembers", "status", condition);

    if (!memberCheck.isOk() || memberCheck.getFirst().empty()) {
        std::cout << "Erreur : L'utilisateur n'est pas membre accepté de cette salle." << std::endl;
        return false;
    }
    

    // Mettre à jour son statut en 'admin'
    std::string updateQuery = "status = 'admin'";
    return db->updateEntry("ChatRoomMembers", updateQuery, condition).isOk();
}


bool ChatRoom::removeAdmin(const std::string& client_pseudo, const std::string& room_name) {
    // Récupérer l'id de l'utilisateur cible
    std::string condition = "username = '" + client_pseudo + "'";
    QueryResult idUserResult = db->selectFromTable("Users", "id_user", condition);
    
    if (!idUserResult.isOk() || idUserResult.getFirst().empty()) {
        std::cout << "Erreur : Utilisateur cible introuvable." << std::endl;
        return false;
    }
    std::string idUser = idUserResult.getFirst();

    // Récupérer l'id de la salle
    condition = "room_name = '" + room_name + "'";
    QueryResult idRoomResult = db->selectFromTable("ChatRooms", "id_room", condition);
    
    if (!idRoomResult.isOk() || idRoomResult.getFirst().empty()) {
        std::cout << "Erreur : Salle de discussion introuvable." << std::endl;
        return false;
    }
    std::string idRoom = idRoomResult.getFirst();

    // Vérifier si l'utilisateur est admin dans cette salle
    condition = "id_room = " + idRoom + " AND id_user = " + idUser + " AND status = 'admin'";
    QueryResult checkAdmin = db->selectFromTable("ChatRoomMembers", "status", condition);
    
    if (!checkAdmin.isOk() || checkAdmin.getFirst().empty()) {
        std::cout << "Erreur : L'utilisateur n'est pas un administrateur dans cette salle." << std::endl;
        return false;
    }

    // Mettre à jour son statut pour "membre"
    std::string updateQuery = "status = 'member'";
    return db->updateEntry("ChatRoomMembers", updateQuery, condition).isOk();
}

bool ChatRoom::addClient(const std::string& client_pseudo, const std::string& room_name) {

    // Récupérer l'id de l'utilisateur cible
    std::string condition = "username = '" + client_pseudo + "'";
    QueryResult idUserResult = db->selectFromTable("Users", "id_user", condition);
    if (!idUserResult.isOk() || idUserResult.getFirst().empty()) {
        std::cout << "Erreur : Utilisateur cible introuvable." << std::endl;
        return false;
    }
    std::string idUser = idUserResult.getFirst();

    // Récupérer l'id de la salle
    condition = "room_name = '" + room_name + "'";
    QueryResult idRoomResult = db->selectFromTable("ChatRooms", "id_room", condition);
    if (!idRoomResult.isOk() || idRoomResult.getFirst().empty()) {
        std::cout << "Erreur : Salle de discussion introuvable." << std::endl;
        return false;
    }
    std::string idRoom = idRoomResult.getFirst();

    // Vérifier si l'utilisateur est déjà membre (ou en attente) de cette salle
    condition = "id_room = " + idRoom + " AND id_user = " + idUser + " AND (status = 'pending')";
    QueryResult memberCheck = db->selectFromTable("ChatRoomMembers", "status", condition);
    if (memberCheck.isOk() && !memberCheck.getFirst().empty()) {
        std::cout << "Erreur : L'utilisateur est déjà membre ou en attente dans cette salle." << std::endl;
        return false;
    }

    // Ajouter l'utilisateur comme membre avec le statut 'member'
    std::string columns = "id_room, id_user, status";
    std::string values = idRoom + ", '" + idUser + "', 'member'";
    return db->insertEntry("ChatRoomMembers", columns, values).isOk();
}

bool ChatRoom::removeClient(const std::string& client_pseudo, const std::string& room_name) {

    // Récupérer l'id de l'utilisateur cible
    std::string condition = "username = '" + client_pseudo + "'";
    QueryResult idUserResult = db->selectFromTable("Users", "id_user", condition);
    if (!idUserResult.isOk() || idUserResult.getFirst().empty()) {
        std::cout << "Erreur : Utilisateur cible introuvable." << std::endl;
        return false;
    }
    std::string idUser = idUserResult.getFirst();

    // Récupérer l'id de la salle
    condition = "room_name = '" + room_name + "'";
    QueryResult idRoomResult = db->selectFromTable("ChatRooms", "id_room", condition);
    if (!idRoomResult.isOk() || idRoomResult.getFirst().empty()) {
        std::cout << "Erreur : Salle de discussion introuvable." << std::endl;
        return false;
    }
    std::string idRoom = idRoomResult.getFirst();

    // Vérifier si l'utilisateur est un client dans cette salle
    condition = "id_room = " + idRoom + " AND id_user = " + idUser + " AND status = 'member'";
    QueryResult checkClient = db->selectFromTable("ChatRoomMembers", "status", condition);
    if (!checkClient.isOk() || checkClient.getFirst().empty()) {
        std::cout << "Erreur : L'utilisateur n'est pas un client dans cette salle." << std::endl;
        return false;
    }

    // Supprimer l'utilisateur de la table ChatRoomMembers
    bool success = db->deleteEntry("ChatRoomMembers", condition).isOk();
    
    // Si l'opération de suppression réussit
    if (success) {
        std::cout << "L'utilisateur '" << client_pseudo << "' a été retiré de la salle '" << room_name << "'." << std::endl;
    } else {
        std::cout << "Erreur lors de la suppression de l'utilisateur." << std::endl;
    }

    return success;
}

void ChatRoom::acceptClientRequest(const std::string& client_pseudo, const std::string& room_name) {
    // Récupérer l'id de l'utilisateur
    std::string condition = "username = '" + client_pseudo + "'";
    QueryResult idUserResult = db->selectFromTable("Users", "id_user", condition);
    if (!idUserResult.isOk() || idUserResult.getFirst().empty()) {
        std::cout << "Erreur : Utilisateur cible introuvable." << std::endl;
        return;
    }
    std::string idUser = idUserResult.getFirst();

    // Récupérer l'id de la salle
    condition = "room_name = '" + room_name + "'";
    QueryResult idRoomResult = db->selectFromTable("ChatRooms", "id_room", condition);
    if (!idRoomResult.isOk() || idRoomResult.getFirst().empty()) {
        std::cout << "Erreur : Salle de discussion introuvable." << std::endl;
        return;
    }
    std::string idRoom = idRoomResult.getFirst();

    // Vérifier si l'utilisateur a une demande en attente
    condition = "id_room = " + idRoom + " AND id_user = " + idUser + " AND status = 'pending'";
    QueryResult pendingCheck = db->selectFromTable("ChatRoomMembers", "status", condition);
    if (!pendingCheck.isOk() || pendingCheck.getFirst().empty()) {
        std::cout << "Erreur : L'utilisateur n'a pas de demande en attente dans cette salle." << std::endl;
        return;
    }

    // Mettre à jour le statut de l'utilisateur à "member"
    std::string updateQuery = "status = 'member'";
    if (!db->updateEntry("ChatRoomMembers", updateQuery, condition).isOk()) {
        std::cout << "Erreur : Impossible d'accepter la demande." << std::endl;
    } else {
        std::cout << "Demande acceptée avec succès. L'utilisateur est maintenant membre." << std::endl;
    }
}

void ChatRoom::refuseClientRequest(const std::string& client_pseudo, const std::string& room_name) {
    // Récupérer l'id de l'utilisateur
    std::string condition = "username = '" + client_pseudo + "'";
    QueryResult idUserResult = db->selectFromTable("Users", "id_user", condition);
    if (!idUserResult.isOk() || idUserResult.getFirst().empty()) {
        std::cout << "Erreur : Utilisateur cible introuvable." << std::endl;
        return;
    }
    std::string idUser = idUserResult.getFirst();

    // Récupérer l'id de la salle
    condition = "room_name = '" + room_name + "'";
    QueryResult idRoomResult = db->selectFromTable("ChatRooms", "id_room", condition);
    if (!idRoomResult.isOk() || idRoomResult.getFirst().empty()) {
        std::cout << "Erreur : Salle de discussion introuvable." << std::endl;
        return;
    }
    std::string idRoom = idRoomResult.getFirst();

    // Vérifier si l'utilisateur a une demande en attente
    condition = "id_room = " + idRoom + " AND id_user = " + idUser + " AND status = 'pending'";
    QueryResult pendingCheck = db->selectFromTable("ChatRoomMembers", "status", condition);
    if (!pendingCheck.isOk() || pendingCheck.getFirst().empty()) {
        std::cout << "Erreur : L'utilisateur n'a pas de demande en attente dans cette salle." << std::endl;
        return;
    }

    // Supprimer l'utilisateur de la table ChatRoomMembers
    if (!db->deleteEntry("ChatRoomMembers", condition).isOk()) {
        std::cout << "Erreur : Échec du refus de la demande." << std::endl;
    } else {
        std::cout << "Demande refusée avec succès. L'utilisateur a été retiré de la file d'attente." << std::endl;
    }
}

bool ChatRoom::checkroomExist(const std::string& room_name) const {
    // Vérifier si la salle existe
    std::string condition = "room_name = '" + room_name + "'";
    QueryResult result = db->selectFromTable("ChatRooms", "id_room", condition);
    return result.isOk() && !result.getFirst().empty();
}

void ChatRoom::joinRoom(const std::string& pseudo, const std::string& room_name) {
    // Récupérer l'ID de l'utilisateur
    std::string condition = "username = '" + pseudo + "'";
    QueryResult idUserResult = db->selectFromTable("Users", "id_user", condition);
    if (!idUserResult.isOk() || idUserResult.getFirst().empty()) {
        std::cout << "Erreur : Utilisateur introuvable." << std::endl;
        return;
    }
    std::string idUser = idUserResult.getFirst();

    // Récupérer l'ID de la salle
    condition = "room_name = '" + room_name + "'";
    QueryResult idRoomResult = db->selectFromTable("ChatRooms", "id_room", condition);
    if (!idRoomResult.isOk() || idRoomResult.getFirst().empty()) {
        std::cout << "Erreur : Salle introuvable." << std::endl;
        return;
    }
    std::string idRoom = idRoomResult.getFirst();

    // Vérifier s'il y a déjà une demande ou une appartenance
    condition = "id_room = " + idRoom + " AND id_user = " + idUser;
    QueryResult memberCheck = db->selectFromTable("ChatRoomMembers", "status", condition);
    if (memberCheck.isOk() && !memberCheck.getFirst().empty()) {
        std::cout << "Vous avez déjà fait une demande ou vous êtes déjà membre de cette salle." << std::endl;
        return;
    }

    // Insérer dans ChatRoomMembers avec le statut "pending"
    std::string columns = "id_room, id_user, status";
    std::string values = idRoom + ", " + idUser + ", 'pending'";
    if (db->insertEntry("ChatRoomMembers", columns, values).isOk()) {
        std::cout << "Votre demande pour rejoindre la salle '" << room_name << "' est en attente de validation." << std::endl;
    } else {
        std::cout << "Erreur lors de l'envoi de la demande d'adhésion." << std::endl;
    }
}

std::vector<std::string> ChatRoom::getMembers(const std::string& room_name) const {
    std::vector<std::string> members;

    std::string query =
        "SELECT Users.username "
        "FROM Users "
        "INNER JOIN ChatRoomMembers ON Users.id_user = ChatRoomMembers.id_user "
        "INNER JOIN ChatRooms ON ChatRoomMembers.id_room = ChatRooms.id_room "
        "WHERE ChatRooms.room_name = '" + room_name + "' "
        "AND (ChatRoomMembers.status = 'member' OR ChatRoomMembers.status = 'admin');";

    QueryResult result = db->executeQuery(query);
    try {
        QueryResult result = db->executeQuery(query);

        if (result.isOk()) {
            members = result.getVector(0);
        } else {
            std::cout << "Erreur : Impossible de récupérer les membres de la salle." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Erreur : " << e.what() << std::endl;
    }

    return members;

}

std::vector<std::string> ChatRoom::getAdmins(const std::string& room_name) {
    std::vector<std::string> admins;

    std::string query =
        "SELECT Users.username "
        "FROM Users "
        "INNER JOIN ChatRoomMembers ON Users.id_user = ChatRoomMembers.id_user "
        "INNER JOIN ChatRooms ON ChatRoomMembers.id_room = ChatRooms.id_room "
        "WHERE ChatRooms.room_name = '" + room_name + "' "
        "AND ChatRoomMembers.status = 'admin';";

    QueryResult result = db->executeQuery(query);

    if (result.isOk()) {
        // La première colonne contient les 'usernames' des administrateurs
        admins = result.getVector(0);
    } else {
        std::cout << "Erreur : Impossible de récupérer les admins de la salle." << std::endl;
    }

    return admins;
}



std::vector<std::string> ChatRoom::getClientPending(const std::string& room_name) {
    std::vector<std::string> pendingClients;

    std::string query =
        "SELECT Users.username "
        "FROM Users "
        "INNER JOIN ChatRoomMembers ON Users.id_user = ChatRoomMembers.id_user "
        "INNER JOIN ChatRooms ON ChatRoomMembers.id_room = ChatRooms.id_room "
        "WHERE ChatRooms.room_name = '" + room_name + "' "
        "AND ChatRoomMembers.status = 'pending';";

    QueryResult result = db->executeQuery(query);

    if (result.isOk()) {
        pendingClients = result.getVector(0);
    } else {
        std::cout << "Erreur : Impossible de récupérer les membres en attente pour la salle." << std::endl;
    }

    return pendingClients;
}


bool ChatRoom::deleteChatRoom(const std::string& room_name) {
    // Étape 1 : Récupérer l’ID de la salle
    std::string condition = "room_name = '" + room_name + "'";
    QueryResult idResult = db->selectFromTable("ChatRooms", "id_room", condition);

    if (!idResult.isOk() || idResult.getFirst().empty()) {
        std::cout << "Erreur : salle '" << room_name << "' introuvable." << std::endl;
        return false;
    }

    std::string idRoom = idResult.getFirst();

    // Étape 2 : Supprimer les membres de la salle
    QueryResult delMembersResult = db->deleteEntry("ChatRoomMembers", "id_room = " + idRoom);
    if (!delMembersResult.isOk()) {
        std::cout << "Erreur : échec lors de la suppression des membres de la salle." << std::endl;
        return false;
    }

    // Étape 3 : Supprimer la salle elle-même
    QueryResult delRoomResult = db->deleteEntry("ChatRooms", "id_room = " + idRoom);
    if (!delRoomResult.isOk()) {
        std::cout << "Erreur : échec lors de la suppression de la salle." << std::endl;
        return false;
    }

    std::cout << "Salle '" << room_name << "' supprimée avec succès." << std::endl;
    return true;
}
bool ChatRoom::quitRoom(const std::string& room_name, const std::string& username) {
    // 1. Récupérer id_room depuis room_name
    QueryResult roomResult = db->selectFromTable("ChatRooms", "id_room", "room_name = '" + room_name + "'");
    if (!roomResult.isOk() || roomResult.getFirst().empty()) {
        std::cout << "Erreur : salle '" << room_name << "' introuvable." << std::endl;
        return false;
    }
    std::string id_room = roomResult.getFirst();

    // 2. Récupérer id_user depuis username
    QueryResult userResult = db->selectFromTable("Users", "id_user", "username = '" + username + "'");
    if (!userResult.isOk() || userResult.getFirst().empty()) {
        std::cout << "Erreur : utilisateur '" << username << "' introuvable." << std::endl;
        return false;
    }
    std::string id_user = userResult.getFirst();

    // 3. Vérifier si l'utilisateur est dans la salle
    std::string condition = "id_user = " + id_user + " AND id_room = " + id_room;
    QueryResult memberResult = db->selectFromTable("ChatRoomMembers", "*", condition);
    if (!memberResult.isOk() || memberResult.getVector(0).empty()) {
        std::cout << "Erreur : l'utilisateur n'est pas membre de cette salle." << std::endl;
        return false;
    }

    // 4. Supprimer l’entrée dans ChatRoomMembers
    QueryResult deleteResult = db->deleteEntry("ChatRoomMembers", condition);
    if (!deleteResult.isOk()) {
        std::cout << "Erreur : impossible de quitter la salle." << std::endl;
        return false;
    }

    std::cout << "'" << username << "' a quitté la salle '" << room_name << "'." << std::endl;

    // 5. Vérifier s'il reste des membres, sinon supprimer la salle
    QueryResult remaining = db->selectFromTable("ChatRoomMembers", "id_user", "id_room = " + id_room);
    if (remaining.isOk() && remaining.getVector(0).empty()) {
        std::cout << "Plus aucun membre dans la salle. Suppression automatique..." << std::endl;
        deleteChatRoom(room_name);  // tu peux appeler ta propre méthode ici
    }

    return true;
}
std::vector<std::string> ChatRoom::getChatRooms() const {
    std::vector<std::string> chatRooms;

    std::string query = "SELECT room_name FROM ChatRooms";
    QueryResult result = db->executeQuery(query);

    if (result.isOk()) {
        chatRooms = result.getVector(0);
        if (chatRooms.empty()) {
            std::cout << "Aucune salle de discussion trouvée." << std::endl;
        }
    } else {
        std::cout << "Erreur : Impossible de récupérer les salles de discussion. Détails : " << result.getError() << std::endl;
    }

    return chatRooms;
}
std::vector<std::string> ChatRoom::getChatRoomsForUser(const std::string& username) const {
    std::vector<std::string> chatRooms;

    std::string query =
        "SELECT ChatRooms.room_name "
        "FROM ChatRooms "
        "JOIN ChatRoomMembers ON ChatRooms.id_room = ChatRoomMembers.id_room "
        "JOIN Users ON ChatRoomMembers.id_user = Users.id_user "
        "WHERE Users.username = '" + username + "' "
        "AND (ChatRoomMembers.status = 'member' OR ChatRoomMembers.status = 'admin');";

    QueryResult result = db->executeQuery(query);

    if (result.isOk()) {
        chatRooms = result.getVector(0);
        if (chatRooms.empty()) {
            std::cout << "Aucune salle où l'utilisateur \"" << username << "\" est membre ou admin." << std::endl;
        }
    } else {
        std::cout << "Erreur : Impossible de récupérer les salles. Détails : " << result.getError() << std::endl;
    }

    return chatRooms;
}


bool ChatRoom::saveMessageToRoom(const std::string& room_name, const std::string& pseudUser, const std::string& message) {
    // Récupérer l'id de la salle
    std::string condition = "room_name = '" + room_name + "'";
    QueryResult roomResult = db->selectFromTable("ChatRooms", "id_room", condition);

    if (!roomResult.isOk() || roomResult.getFirst().empty()) {
        std::cout << "Erreur : salle introuvable." << std::endl;
        return false;
    }

    std::string id_room = roomResult.getFirst();
    std::string timestamp = db->getTime();

    std::string columns = "id_room, pseudUser, message, msg_date_time";
    std::string values = id_room + ", '" + pseudUser + "', '" + message + "', '" + timestamp + "'";

    QueryResult insertResult = db->insertEntry("ChatMessages", columns, values);
    return insertResult.isOk();
}
std::string ChatRoom::getMessagesFromRoom(const std::string& room_name) {
    // Récupérer l'id de la salle
    std::string condition = "room_name = '" + room_name + "'";
    QueryResult roomResult = db->selectFromTable("ChatRooms", "id_room", condition);

    if (!roomResult.isOk() || roomResult.getFirst().empty()) {
        std::cout << "Erreur : salle introuvable." << std::endl;
        return "";
    }

    std::string id_room = roomResult.getFirst();
    condition = "id_room = " + id_room;
    std::string order = " ORDER BY msg_date_time ";
    QueryResult result = db->selectFromTable("ChatMessages", "pseudUser, message, msg_date_time", condition + order);

    json messagesJson = json::array();
    for (const auto& row : result.getData()) {
        if (row.size() >= 3) {
            json msg;
            msg["sender"] = row[0];
            msg["message"] = row[1];
            msg["msg_date_time"] = row[2];
            msg["receiver"] = "Room";
            messagesJson.push_back(msg);
        }
    }

    return messagesJson.dump() + "\n";
}
void ChatRoom::processRoomChat(int senderSocket, const std::string& sender, const std::string& roomName, std::shared_ptr<std::vector<int>> roomSockets) {
    std::cout << "Room chat started in room: " << roomName << std::endl;
    bool historiqueEnvoye = false;
    if (!historiqueEnvoye) {
        std::string historique = getMessagesFromRoom(roomName);
        
        if (!historique.empty() && historique != "[]\n") {
            std::cout << "Historique de la room " << roomName << " : " << historique << std::endl;
            std::cout << "----> Envoi de l'historique à " << sender << std::endl;
            send(senderSocket, historique.c_str(), historique.size(), 0);
        } else {
            std::cout << "Aucun historique à envoyer pour la room " << roomName << std::endl;
        }

        historiqueEnvoye = true;
    }
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
                msg["receiver"] = roomName;  // Important pour saveMessage

                if (saveMessageToRoom(sender, roomName, msg["message"])) {
                    std::cout << "Message de " << sender << " sauvegardé dans la room " << roomName << std::endl;
                } else {
                    std::cerr << "Erreur lors de la sauvegarde du message dans la room " << roomName << std::endl;
                }

                std::string jsonStr = msg.dump() + "\n";

                // Envoyer à tous sauf l’expéditeur
                for (int socket : *roomSockets) {
                    if (socket != senderSocket) {
                        send(socket, jsonStr.c_str(), jsonStr.size(), 0);
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


/*std::vector<std::string> ChatRoom::getMembers(const std::string& room_name) {
    std::vector<std::string> clients;

    std::string query = 
        "SELECT username "
        "FROM Users "
        "JOIN ChatRoomMembers ON Users.id_user = ChatRoomMembers.id_user "
        "JOIN ChatRooms ON ChatRoomMembers.id_room = ChatRooms.id_room "
        "WHERE ChatRooms.room_name = '" + room_name + "'";

    QueryResult result = db->executeQuery(query);

    if (!result.isOk() || result.getVector().empty()) {
        std::cout << "Erreur : Aucun client trouvé dans cette salle." << std::endl;
        return clients;
    }

    clients = result.getVector();
    return clients;
}
*/
/*std::vector<std::string> ChatRoom::getAdminPseudo(const std::string& room_name) {
    std::vector<std::string> adminPseudos;

    // Récupérer les ID des admins de la salle
    std::string condition = "room_name = '" + room_name + "'";
    QueryResult roomResult = db->selectFromTable("ChatRooms", "id_admin", condition);
    
    if (!roomResult.isOk() || roomResult.getFirst().empty()) {
        return {"Erreur : Salle introuvable ou sans admin."};
    }

    // Pour chaque ID d'admin, récupérer son pseudo
    for (const std::string& idAdmin : roomResult.getVector()) {
        std::string userCondition = "id_user = " + idAdmin;
        QueryResult adminResult = db->selectFromTable("Users", "username", userCondition);
        
        if (adminResult.isOk() && !adminResult.getFirst().empty()) {
            adminPseudos.push_back(adminResult.getFirst());
        }
    }

    if (adminPseudos.empty()) {
        return {"Erreur : Aucun administrateur trouvé."};
    }

    return adminPseudos;
}
*/


/*
bool ChatRoom::isAdmin(const std::string& pseudo, const std::string& room_name) const {
    std::string condition = "cr.room_name = '" + room_name + "' AND u.username = '" + pseudo + "'";
    std::string sql = 
        "SELECT crm.id_user, crm.is_admin, u.username "
        "FROM ChatRoomMembers crm "
        "JOIN Users u ON crm.id_user = u.id_user "
        "JOIN ChatRooms cr ON crm.id_room = cr.id_room "
        "WHERE " + condition;
    
    QueryResult result = db->executeQuery(sql);
    
    if (result.isOk() && !result.getFirst().empty()) {
        // L'utilisateur existe dans la salle
        std::string isAdmin = result.getFirst();  // récupère le rôle
        if (isAdmin == "admin") {
            std::cout << pseudo << " is an admin." << std::endl;
            return true;  // Si c'est un admin, ce n'est pas un client
        }
        return false;  // Si ce n'est pas un admin, c'est un client
    }
    
    return false;  // Si aucun résultat, l'utilisateur n'est pas dans cette salle
}
bool ChatRoom::isClient(const std::string& pseudo, const std::string& room_name ) const {
    return !isAdmin(pseudo, room_name); // Inverse la logique
}
*/
/*bool ChatRoom::isPendingReq(const std::string& pseudo, const std::string& room_name) const {
    // Récupérer l'ID de la salle
    std::string condition = "room_name = '" + room_name + "'";
    std::string roomId = db->selectFromTable("ChatRooms", "id_room", condition).getFirst();

    // Vérifier si l'utilisateur a une requête en attente
    condition = "cr.room_name = '" + room_name + "' AND u.username = '" + pseudo + "' AND crm.status = 'pending'";
    std::string sql = 
        "SELECT crm.status "
        "FROM ChatRoomMembers crm "
        "JOIN Users u ON crm.id_user = u.id_user "
        "JOIN ChatRooms cr ON crm.id_room = cr.id_room "
        "WHERE " + condition;

    QueryResult result = db->executeQuery(sql);
    return result.isOk() && !result.getFirst().empty();
}
*/
/*
bool ChatRoom::addAdmin(const std::string& admin_pseudo, const std::string& client_pseudo, const std::string& room_name) {
    // Vérifier si le demandeur est un admin
    if (!isAdmin(admin_pseudo, room_name)) {
        std::cout << "Erreur : Seul un administrateur peut ajouter un autre admin." << std::endl;
        return false;
    }

    // Récupérer l'id de l'utilisateur cible
    std::string condition = "username = '" + client_pseudo + "'";
    QueryResult idUserResult = db->selectFromTable("Users", "id_user", condition);
    if (!idUserResult.isOk() || idUserResult.getFirst().empty()) {
        std::cout << "Erreur : Utilisateur cible introuvable." << std::endl;
        return false;
    }
    std::string idUser = idUserResult.getFirst();

    // Récupérer l'id de la salle
    condition = "room_name = '" + room_name + "'";
    QueryResult idRoomResult = db->selectFromTable("ChatRooms", "id_room", condition);
    if (!idRoomResult.isOk() || idRoomResult.getFirst().empty()) {
        std::cout << "Erreur : Salle de discussion introuvable." << std::endl;
        return false;
    }
    std::string idRoom = idRoomResult.getFirst();

    // Vérifier si le membre est déjà dans la salle
    condition = "id_room = " + idRoom + " AND id_user = " + idUser;
    QueryResult memberCheck = db->selectFromTable("ChatRoomMembers", "status", condition);
    if (!memberCheck.isOk() || memberCheck.getFirst().empty()) {
        std::cout << "Erreur : L'utilisateur n'est pas membre de cette salle." << std::endl;
        return false;
    }

    // Mettre à jour son statut en 'admin'
    std::string updateQuery = "status = 'admin'";
    return db->updateEntry("ChatRoomMembers", updateQuery, condition).isOk();
}

*/

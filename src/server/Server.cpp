#include "Server.hpp"
#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"
#include <ncurses.h>
#include <unistd.h>
#include <csignal>
#include <netinet/in.h>
#include <fstream>
#include <iostream>
#include <algorithm>


Server::Server(int port) 
    :   
    port(port), 
    serverSocket(-1), 
    clientIdCounter(0), 
    database(std::make_shared<DataBase>()), 
    userManager(database),
    chatRoomsManage(database), // Initialize chatRoom with the database instance
    chat(database) // Passez la base de données à Chat
    {}


bool Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Erreur: Impossible de créer le socket du serveur." << std::endl;
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Erreur: Impossible de binder le socket." << std::endl;
        close(serverSocket);
        return false;
    }

    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Erreur: Impossible d'écouter sur le port." << std::endl;
        close(serverSocket);
        return false;
    }

    std::cout << "Serveur en attente de connexions sur le port " << port << std::endl;
    return true;
}

void Server::disconnectPlayer(int clientId){
    close(clientIdToSocket[clientId]); // Fermer la connexion
    clientGameRoomId.erase(clientId); // Supprimer l'ID de la salle de jeu   
    clientIdToSocket.erase(clientId); // Supprimer le socket du client
    clientStates.erase(clientId); // Supprimer l'état des menus du client
}

void Server::stop() {
    if (serverSocket != -1) {
        shutdown(serverSocket, SHUT_RDWR);
        close(serverSocket);
        serverSocket = -1;
    }
}

void Server::sendMenuToClient(int clientSocket, const std::string& screen) {
    send(clientSocket, screen.c_str(), screen.size(), 0);
}

void Server::returnToMenu(int clientSocket, int clientId, MenuState state, const std::string& message, int sleepTime){
    if (!message.empty()) {
        sendMenuToClient(clientSocket, menu.displayMessage(message));
        sleep(sleepTime);
    }
    clientStates[clientId] = state;
    handleMenu(clientSocket, clientId, "", true); // Refresh the menu
}

void Server::acceptClients() {
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientSocket < 0) {
        std::cerr << "Erreur: Échec de l'acceptation du client." << std::endl;
        return;
    }

    int clientId = clientIdCounter.fetch_add(1);  // Attribuer un ID unique et incrémenter le compteur
    std::cout << "Client #" << clientId << " connecté." << std::endl;
    clientIdToSocket[clientId] = clientSocket; 
    clientStates[clientId] = MenuState::Welcome;

    clear();
    sendMenuToClient(clientSocket, menu.getMainMenu0()); 
    menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans le menu d'accueil.");
    refresh();

    // Lancer un thread pour gérer ce client
    std::thread clientThread(&Server::handleClient, this, clientSocket, clientId);
    clientThread.detach();
}


void Server::handleClient(int clientSocket, int clientId) {
    char buffer[1024];
    std::string partialMessage; // Stocker les messages partiels
    constexpr int TARGET_FPS = 7;
    constexpr std::chrono::milliseconds FRAME_TIME(1000 / TARGET_FPS);

    auto lastRefreshTime = std::chrono::steady_clock::now();
    auto nextRefreshTime = lastRefreshTime + FRAME_TIME;

    try {
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);

            if (bytesReceived > 0) {
                partialMessage += std::string(buffer, bytesReceived); // Ajouter les données reçues

                // Traiter les messages complets (séparés par '\n')
                std::size_t pos;
                while ((pos = partialMessage.find('\n')) != std::string::npos) {
                    std::string message = partialMessage.substr(0, pos); // Extraire un message complet
                    partialMessage.erase(0, pos + 1); // Supprimer le message traité

                    try {
                        if(message.empty()) continue;
                        json receivedData = json::parse(message);
                        
                        if(receivedData.contains("mode")){
                            clientMode[clientId] = receivedData["mode"] == "gui" ? true  : false;
                        }

                        if (receivedData.contains(jsonKeys::MESSAGE)) {
                            handleChat(clientSocket, clientId, receivedData);
                            continue;
                        }
                        if (!receivedData.contains(jsonKeys::ACTION) || !receivedData[jsonKeys::ACTION].is_string()) {
                            std::cerr << "Erreur: 'action' manquant ou invalide dans le JSON reçu." << std::endl;
                            continue;
                        }

                        std::string action = receivedData[jsonKeys::ACTION];
                        handleGUIActions(clientSocket, clientId, receivedData);
                        handleMenu(clientSocket, clientId, action);
                    } catch (const json::parse_error& e) {
                        std::cerr << "Erreur de parsing JSON : " << e.what() << std::endl;
                    }
                }
            } else if (bytesReceived == 0) {
                // Client disconnected
                disconnectPlayer(clientId);
                return;
            } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                // Real error occurred
                std::cerr << "Receive error: " << strerror(errno) << std::endl;
                break;
            }

            // Gestion du rafraîchissement à 12 FPS
            if(!clientMode[clientId] || clientStates[clientId] == MenuState::JoinGame){
                auto currentTime = std::chrono::steady_clock::now();
                if (currentTime >= nextRefreshTime) {
                    handleMenu(clientSocket, clientId, "", true); // Refresh only
                    lastRefreshTime = currentTime;
                    nextRefreshTime = lastRefreshTime + FRAME_TIME;

                    // Ajustement dynamique si on prend du retard
                    if (currentTime > nextRefreshTime) {
                        nextRefreshTime = currentTime + FRAME_TIME;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception dans handleClient pour le client #" << clientId << ": " << e.what() << std::endl;
        disconnectPlayer(clientId);
    }
}


void Server::handleGUIActions(int clientSocket, int clientId, const json& action) {
    if(action.contains(jsonKeys::ACTION) && action[jsonKeys::ACTION].is_string()) {
        std::string actionType = action[jsonKeys::ACTION];
        
        if(actionType == jsonKeys::LOGIN) {
            //gerer le login
            if(userManager.userNotExists(action[jsonKeys::USERNAME])){
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Le pseudo n'existe pas.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "Le pseudo n'existe pas.");

            }else{
                if (userManager.authenticateUser(action[jsonKeys::USERNAME], action[jsonKeys::PASSWORD])) { // Si le mot de passe est correct
                    clientStates[clientId] = MenuState::Main;
                    menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans le menu principal.");
                    clientPseudo[clientId] = action[jsonKeys::USERNAME];
                    pseudoTosocket[clientPseudo[clientId]] = clientSocket;
                    sockToPseudo[clientSocket] = clientPseudo[clientId];
                    int avatarIndex = userManager.getUserAvatarId(action[jsonKeys::USERNAME]);
                    menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "avatar", {std::to_string(avatarIndex)});
                    json response = {
                        {"action", "login_success"},
                        {"username", action[jsonKeys::USERNAME]},
                        {"avatar", avatarIndex}
                    };
                    send(clientSocket, response.dump().c_str(), response.dump().size(), 0);
                    
                }else{
                    menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Le mot de passe est incorrect.");
                    menuStateManager->sendTemporaryDisplay(clientSocket, "Le mot de passe est incorrect.");
                }
            }
            return;
        }
        else if(actionType == jsonKeys::REGISTER) {
            //gerer l'enregistrement
            if(userManager.userNotExists(action[jsonKeys::USERNAME])){
                int avatarIndex = action.contains("avatar") ? static_cast<int>(action["avatar"]) : -1;
                userManager.registerUser(action[jsonKeys::USERNAME], action[jsonKeys::PASSWORD], avatarIndex);
                clientStates[clientId] = MenuState::Main;
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Le pseudo n'existe pas.");
                clientPseudo[clientId] = action[jsonKeys::USERNAME];
                sockToPseudo[clientSocket] = action[jsonKeys::USERNAME];
                pseudoTosocket[action[jsonKeys::USERNAME]] = clientSocket;
            }else{
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Le pseudo existe déjà.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "Le pseudo existe déjà.");
            }
            return;
        }
        else if(actionType == jsonKeys::WELCOME) {
            //gerer le menu d'accueil
            clientStates[clientId] = MenuState::Welcome;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans le menu d'accueil.");
            return;
        }
        else if(actionType == jsonKeys::MAIN) {
            //gerer le menu principal
            clientStates[clientId] = MenuState::Main;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId]);
            return;
        }
        else if(actionType == jsonKeys::CREATE_TEAM_MENU) {
            //gerer la creation d'equipe
            clientStates[clientId] = MenuState::CreatTeamMenu;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "createTeam");
            return;
        }
        else if(actionType == jsonKeys::CREATE_TEAM) {
            std::string teamName = action[jsonKeys::TEAM_NAME];
            if(chatRoomsManage.checkroomExist(teamName)){
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "la room  existe déjà.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "La room existe déjà.");
                return;

            }
            else {
                chatRoomsManage.createTeam(teamName, sockToPseudo[clientSocket]);
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "la room a été créée avec succès.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "La room a été créée avec succès.");
                return;
            }
            
        }
        else if(actionType == jsonKeys::JOIN_TEAM_MENU) {
            //gerer la creation d'equipe
            clientStates[clientId] = MenuState::JoinTeam;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "joinTeam");
            return;
        }
        else if(actionType == jsonKeys::JOIN_TEAM) {
            std::string teamName = action[jsonKeys::TEAM_NAME];
            if(!chatRoomsManage.checkroomExist(teamName)){
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "la room n existe pas.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "La room n'existe pas.");
                return;
            }
            else {
                chatRoomsManage.joinTeam(sockToPseudo[clientSocket], teamName);
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "la room a été rejointe avec succès.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "La room a été rejointe avec succès.");
                return;
            }
        }
        else if(actionType == jsonKeys::MANAGE_TEAMS_MENU) {
            //gerer la gestion d'equipe
            clientStates[clientId] = MenuState::ManageTeam;
            auto teams = chatRoomsManage.getChatRoomsForUser(clientPseudo[clientId]);
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "manageTeam",teams);
            return;
        }
        else if(actionType == jsonKeys::MANAGE_TEAM) {
            //gerer les membres d'equipe
            std::string teamName = action[jsonKeys::TEAM_NAME];
            clientStates[clientId] = MenuState::ManageTeam;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "manageTeam");
            menuStateManager->sendTemporaryDisplay(clientSocket, "Bienvenue dans le menu de gestion de l'équipe.");
            return;
        }
        else if(actionType == jsonKeys::DELETE_TEAM) {
            //gerer la suppression d'equipe
            std::string teamName = action[jsonKeys::TEAM_NAME];
            if(!chatRoomsManage.checkroomExist(teamName)){
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "la room n existe pas.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "La room n'existe pas.");
                return;
            }
            else {
                chatRoomsManage.deleteChatRoom(teamName);
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "la room a été supprimée avec succès.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "La room a été supprimée avec succès.");
                return;
            }
        }
        else if(actionType == jsonKeys::QUIT_ROOM) {
            //gerer la sortie d'equipe
            std::string teamName = action[jsonKeys::TEAM_NAME];
            if(!chatRoomsManage.checkroomExist(teamName)){
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "la room n existe pas.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "La room n'existe pas.");
                return;
            }
            else {
                chatRoomsManage.quitRoom(teamName, sockToPseudo[clientSocket]);
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "la room a été quittée avec succès.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "La room a été quittée avec succès.");
                return;
            }
        }
        else if(actionType == jsonKeys::LIST_MEMBERS){
            //gerer la liste des membres d'equipe
            std::string teamName = action[jsonKeys::TEAM_NAME];
            auto members = chatRoomsManage.getMembers(teamName);
            clientStates[clientId] = MenuState::ListTeamMembres;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], jsonKeys::LIST_MEMBERS, members);
            return;
        }
        else if (actionType == jsonKeys::ADD_MEMBER) {
            //gerer l'ajout de membre d'equipe
            std::string teamName = action[jsonKeys::TEAM_NAME];
            std::string memberName = action[jsonKeys::MEMBER_NAME];
            if(!chatRoomsManage.checkroomExist(teamName)){
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "la room n existe pas.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "La room n'existe pas.");
                return;
            }
            else {
                chatRoomsManage.addClient(memberName, teamName);
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "le membre a été ajouté avec succès.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "Le membre a été ajouté avec succès.");
                return;
            }
        }
        else if(actionType == jsonKeys::REMOVE_MEMBER) {
            //gerer la suppression de membre d'equipe
            std::string teamName = action[jsonKeys::TEAM_NAME];
            std::string memberName = action[jsonKeys::MEMBER_NAME];
            if(!chatRoomsManage.checkroomExist(teamName)){
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "la room n existe pas.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "La room n'existe pas.");
                return;
            }
            else {
                chatRoomsManage.removeClient(memberName, teamName);
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "le membre a été supprimé avec succès.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "Le membre a été supprimé avec succès.");
                return;
            }
        }
        else if(actionType == jsonKeys::ADD_ADMIN) {
            //gerer l'ajout d'admin d'equipe
            std::string teamName = action[jsonKeys::TEAM_NAME];
            std::string memberName = action[jsonKeys::MEMBER_NAME];
            if(!chatRoomsManage.checkroomExist(teamName)){
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "la room n existe pas.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "La room n'existe pas.");
                return;
            }
            else {
                chatRoomsManage.addAdmin(memberName, teamName);
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "le membre a été ajouté en tant qu'admin avec succès.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "Le membre a été ajouté en tant qu'admin avec succès.");
                return;
            }
        }
        else if(actionType == jsonKeys::REMOVE_ADMIN) {
            //gerer la suppression d'admin d'equipe
            std::string teamName = action[jsonKeys::TEAM_NAME];
            std::string memberName = action[jsonKeys::MEMBER_NAME];
            if(!chatRoomsManage.checkroomExist(teamName)){
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "la room n existe pas.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "La room n'existe pas.");
                return;
            }
            else {
                chatRoomsManage.removeAdmin(memberName, teamName);
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "le membre a été supprimé en tant qu'admin avec succès.");
                menuStateManager->sendTemporaryDisplay(clientSocket, "Le membre a été supprimé en tant qu'admin avec succès.");
                return;
            }
        }
        else if(actionType == "game") {
            //gerer le jeu
            clientStates[clientId] = MenuState::Game;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans le jeu.");
            return;
        }
        else if(actionType == jsonKeys::RANKING) {
            //gerer le classement
            clientStates[clientId] = MenuState::classement;
            auto ranking = userManager.getRanking();
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "ranking", {},{},ranking);
            return;
        }
        else if(actionType == jsonKeys::SETTINGS) {
            //gerer les parametres
            clientStates[clientId] = MenuState::Settings;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans les paramètres.");
            return;
        }
        else if(actionType == jsonKeys::CHAT_PRIVATE) {
            //gerer le chat
            clientStates[clientId] = MenuState::chat;
            auto friends = userManager.getFriendList(clientPseudo[clientId]);
            std::vector<std::pair<std::string,int>> contactStrings;
            for (const auto& friendName : friends) {
                int avatarIndex = userManager.getUserAvatarId(friendName);
                auto contact = std::make_pair(friendName, avatarIndex); 
                contactStrings.push_back(contact);
            }
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "contacts",{},contactStrings);
            return;
        }
        else if(actionType == jsonKeys::OPEN_CHAT){
            //gerer l'ouverture du chat
            receiverOfMessages[clientSocket] = action["contact"];
            return;
        }
        else if(actionType == jsonKeys::CHAT_LOBBY){
            const auto& roomName = "GameRoom" + std::to_string(clientGameRoomId[clientId]);
            receiverOfMessages[clientSocket] = roomName;
            clientStates[clientId] = MenuState::chat;
            std::vector<std::pair<std::string,int>> contactStrings = {{roomName, -1}};
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "contacts",{},contactStrings);
            return;
        }
        else if(actionType == jsonKeys::PLAYER_INFO){
            std::string username = clientPseudo[clientId];
            auto [playerName, bestScore] = userManager.getCurrentPlayerInfo(username);
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "player_info", {playerName,bestScore});
            return;
        }
        
        else if (actionType == jsonKeys::TEAMS) {
            //gerer les amis
            clientStates[clientId] = MenuState::Team;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans le menu des amis.");
            return;
        }
        else if(actionType == jsonKeys::FRIENDS){
            clientStates[clientId] = MenuState::Friends;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId]);
            return;
        }
        else if(actionType == jsonKeys::FRIEND_LIST) {
            //la liste des amis
            auto friends = userManager.getFriendList(clientPseudo[clientId]);
            clientStates[clientId] = MenuState::FriendList;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], jsonKeys::FRIEND_LIST, friends);
            return;
        }
        else if(actionType == jsonKeys::FRIEND_REQUEST_LIST) {
            //gerer l'ajout d'amis
            auto friendRequests = userManager.getRequestList(clientPseudo[clientId]);
            clientStates[clientId] = MenuState::FriendRequestList;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId],  jsonKeys::FRIEND_REQUEST_LIST, friendRequests);
            return;
        }
        else if (actionType == jsonKeys::ADD_FRIEND_MENU) {
            //gerer l'ajout d'amis
            clientStates[clientId] = MenuState::AddFriend;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Ami ajouté.");

            return;
        }
        else if(actionType == jsonKeys::ADD_FRIEND) {
            if (userManager.userNotExists(action["friend"])) {
                menuStateManager->sendTemporaryDisplay(clientSocket, "Demande d'ami échouée. L'utilisateur n'existe pas.");
            } else if (userManager.sendFriendRequest(clientPseudo[clientId], action["friend"])) {
                menuStateManager->sendTemporaryDisplay(clientSocket, "Demande d'ami envoyée à " + action["friend"].get<std::string>() + "");
            } else {
                menuStateManager->sendTemporaryDisplay(clientSocket, "Demande d'ami échouée. L'utilisateur a déjà été ajouté.");
            }
            return;
        }
        else if (actionType == jsonKeys::ACCEPT_FRIEND_REQUEST) {
            //gerer l'acceptation d'amis
            if (userManager.acceptFriendRequest(clientPseudo[clientId], action["friend"])){
                menuStateManager->sendTemporaryDisplay(clientSocket, "Ami accepté.");
            } else {
                menuStateManager->sendTemporaryDisplay(clientSocket, "Ami déjà accepté ou demande d'ami non trouvée.");
            }
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Ami accepté.");

            return;
        }
        else if(actionType == jsonKeys::REJECT_FRIEND_REQUEST) {
            //gerer le refus d'amis
            if (!action.contains("friend") || !action["friend"].is_string()) {
                menuStateManager->sendTemporaryDisplay(clientSocket, "Demande d'ami échouée. Données invalides.");
            } else if (userManager.userNotExists(action["friend"])) {
                menuStateManager->sendTemporaryDisplay(clientSocket, "Demande d'ami échouée. L'utilisateur n'existe pas.");
            } else if (userManager.rejectFriendRequest(clientPseudo[clientId], action["friend"])) {
                menuStateManager->sendTemporaryDisplay(clientSocket, "Ami refusé.");
            } else {
                menuStateManager->sendTemporaryDisplay(clientSocket, "Demande d'ami échouée. L'utilisateur a déjà été refusé.");
            }
            return;
        }
        else if(actionType == jsonKeys::REMOVE_FRIEND) {
            //gerer la suppression d'ami
            if (!action.contains("friend") || !action["friend"].is_string()) {
                menuStateManager->sendTemporaryDisplay(clientSocket, "Suppression d'ami échouée. Données invalides.");
            } else if (userManager.userNotExists(action["friend"])) {
                menuStateManager->sendTemporaryDisplay(clientSocket, "Suppression d'ami échouée. L'utilisateur n'existe pas.");
            } else if (userManager.deleteFriend(clientPseudo[clientId], action["friend"])) {
                menuStateManager->sendTemporaryDisplay(clientSocket, "Ami supprimé.");
            } else {
                menuStateManager->sendTemporaryDisplay(clientSocket, "Suppression d'ami échouée. L'utilisateur a déjà été supprimé.");
            }
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Ami supprimé.");
            return;
        }
        else if(actionType == "createjoin"){
            clientStates[clientId] = MenuState::JoinOrCreateGame;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans menu play.");
            return;
        }
        else if(actionType == "choiceMode"){
            clientStates[clientId] = MenuState::CreateGame;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans menu modeJeu .");
            return;
        }
        else if(actionType == "EndlessMode"){
            clientStates[clientId] = MenuState::Play;
            this->keyInputCreateGameRoom(clientId, GameModeName::Endless);
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans menu modeJeu Endless.");
            return;
        }
        else if(actionType == "DuelMode") {
            clientStates[clientId] = MenuState::Settings;
            this->keyInputCreateGameRoom(clientId, GameModeName::Duel);
            auto friends = userManager.getFriendList(clientPseudo[clientId]);
            std::map<std::string,  std::vector<std::string>> active = updateActivePlayerObserver(clientId);
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "mode duel", friends, {}, active);
            return;
            
        }

        else if(actionType == "ClassicMode") {
            clientStates[clientId] = MenuState::Settings;
            this->keyInputCreateGameRoom(clientId, GameModeName::Classic);
            auto friends = userManager.getFriendList(clientPseudo[clientId]);
            std::map<std::string,  std::vector<std::string>> active = updateActivePlayerObserver(clientId); 
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "mode classic", friends, {}, active);
        }
        else if(actionType == "RoyaleMode") {
            clientStates[clientId] = MenuState::Settings;
            this->keyInputCreateGameRoom(clientId, GameModeName::Royal_Competition);
            auto friends = userManager.getFriendList(clientPseudo[clientId]);
            std::map<std::string,  std::vector<std::string>> active = updateActivePlayerObserver(clientId);
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "mode royale", friends, {}, active);
        }
        else if(actionType == "Rejoindre"){
            clientStates[clientId] = MenuState::JoinGame;
            handleMenu(clientSocket, clientId, "", true);
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans menu choisir GameRoom .");
            return;
        }
        else if(actionType == "AcceptRejoindre"){
            std::map<std::string,  std::vector<std::string>> active = updateActivePlayerObserver(clientId);
            auto gameRoom = gameRooms[clientGameRoomId[clientId]];
            clientStates[clientId] = MenuState::Settings;
            for (const auto& cId : gameRoom->getPlayers()){
                auto friends = userManager.getFriendList(clientPseudo[cId]);
                if (gameRoom->getOwnerId() == cId){
                    menuStateManager->sendMenuStateToClient(clientIdToSocket[cId], clientStates[cId], "owner", friends, {}, active);
                }else{
                    menuStateManager->sendMenuStateToClient(clientIdToSocket[cId], clientStates[cId], "player", friends, {}, active);
                }
            }

            for (const auto& observerId : gameRoom->getViewers()) {
                menuStateManager->sendMenuStateToClient(clientIdToSocket[observerId], clientStates[observerId], "observer", {}, {}, active);
            }
            return;
        }
    }
}




void Server::handleMenu(int clientSocket, int clientId, const std::string& action, bool refreshMenu) {
    auto currentClient = clientPseudo[clientId];
    auto roomName = roomToManage[clientId];
    switch (clientStates[clientId]) {
        case MenuState::Welcome:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getMainMenu0()); 
            else keyInputWelcomeMenu(clientSocket, clientId, action);
            break;
        case MenuState::RegisterPseudo:
            if (!refreshMenu) keyInputRegisterPseudoMenu(clientSocket, clientId, action);
            break;
        case MenuState::RegisterPassword:
            if (!refreshMenu) keyInputRegisterPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::LoginPseudo:
            if (!refreshMenu) keyInputLoginPseudoMenu(clientSocket, clientId, action);
            break;
        case MenuState::LoginPassword:
            if (!refreshMenu) keyInputLoginPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::Main:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getMainMenu1());
            else keyInputMainMenu(clientSocket, clientId, action);
            break;
            case MenuState::classement:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getRankingMenu(userManager.getRanking()));
            else keyInputRankingMenu(clientSocket, clientId, action);
            break;
        case MenuState::Team:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getTeamsMenu());
            else keyInputTeamsMenu(clientSocket, clientId, action);
            break;
        case MenuState::CreateTeam:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getCreateChatRoomMenu());
            else keyInputCreateChatRoom(clientSocket, clientId, action);
            break;
        case MenuState::JoinTeam:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getJoinChatRoomMenu());
            else keyInputJoinChatRoom(clientSocket, clientId, action);
            break;
        case MenuState::TeamsInvitation:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getInvitationsRoomsMenu(chatRoomsManage.getTeamsInvitaionForUser(currentClient)));
            else keyInputTeamsInvitation(clientSocket, clientId, action);
            break;
        case MenuState::ManageTeams:
            if(refreshMenu)sendMenuToClient(clientSocket, menu.getManageTeamsMenu(chatRoomsManage.getChatRoomsForUser(sockToPseudo[clientSocket])));
            else keyInputManageTeams(clientSocket, clientId, action);
            break;
        case MenuState::ManageTeam:
            if(refreshMenu)  sendMenuToClient(clientSocket, menu.getManageTeamMenu(chatRoomsManage.isAdmin(sockToPseudo[clientSocket],roomToManage[clientId]), chatRoomsManage.getAdmins(roomToManage[clientId]).size() == 1));
            else keyInputManageTeam(clientSocket, clientId, action);
            break;
        case MenuState::ListTeamMembres:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getListeMembers(chatRoomsManage.getMembers(roomName)));
            else keyInputListMembres(clientSocket, clientId, action);
            break;
        case MenuState::AddMembre:
            if(!refreshMenu) keyInputAddMembre(clientSocket, clientId, action);
            break;
        case MenuState::AddAdmin:
            if(!refreshMenu) keyInputAddAdmin(clientSocket, clientId, action);
            break;
        case MenuState::RoomRequestList:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getListeRequests(chatRoomsManage.getClientPending(roomName)));
            else keyInputRequestList(clientSocket, clientId, action);
            break;
        case MenuState::ConfirmDeleteRoom:
            if(!refreshMenu) keyInputConfirmDeleteRoom(clientSocket, clientId, action);
            break;
        case MenuState::ConfirmQuitRoom:
            keyInputQuitRoom(clientSocket, clientId, action);
            break;
        case MenuState::TeamChat:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getChooseContactMenu(chatRoomsManage.getChatRoomsForUser(sockToPseudo[clientSocket]), false));
            else keyInputChatTeam(clientSocket, clientId, action);
            break;
            case MenuState::Friends:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getFriendMenu());
            else keyInputFriendsMenu(clientSocket, clientId, action);
            break;
        case MenuState::AddFriend:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getAddFriendMenu());
            else keyInputAddFriendMenu(clientSocket, clientId, action);
            break;
        case MenuState::FriendList:
            std::cout << "Client #" << clientId << " a demandé d'ouvrir la liste d'amis." << std::endl;
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getFriendListMenu(userManager.getFriendList(currentClient)));
            else keyInputManageFriendlist(clientSocket, clientId, action);
            break;
        case MenuState::FriendRequestList:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getRequestsListMenu(userManager.getRequestList(currentClient)));
            else keyInputManageFriendRequests(clientSocket, clientId, action);
            break;
        case MenuState::PrivateChat:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getChooseContactMenu(userManager.getFriendList(clientPseudo[clientId]), true));
            else keyInputPrivateChat(clientSocket, clientId, action);
            break;
        case MenuState::JoinOrCreateGame:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
            else keyInputJoinOrCreateGameMenu(clientSocket, clientId, action);
            break;
        case MenuState::JoinGame:
            if(refreshMenu){
                std::vector<std::vector<std::string>> invitations = userManager.getListGameRequest(currentClient);
                sendMenuToClient(clientSocket, menu.getGameRequestsListMenu(invitations));
            }
            keyInputChoiceGameRoom(clientSocket, clientId, action);
            break;
        case MenuState::CreateGame:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getGameModeMenu());
            else keyInputChooseGameModeMenu(clientSocket, clientId, action);
            break;
        case MenuState::GameOver:
            if(refreshMenu){}
            else keyInputGameOverMenu(clientSocket, clientId, action);
            break;
        case MenuState::Settings:
            if(refreshMenu) {}
            else keyInputLobbySettingsMenu(clientSocket, clientId, action);
            break;
        case MenuState::Help:
            if(refreshMenu){}
            else keyInputHelpMenu(clientSocket, clientId, action);
            break;
        case MenuState::Play:
            if(refreshMenu){}
            else sendInputToGameRoom(clientId, action);
            break;
        case MenuState::Observer:
            if(refreshMenu){}
            else sendInputToGameRoom2(clientId, action);
            break;

        default:
            break;
    }
}

/*
Welcome &Main Menu
*/
void Server::keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {  // Se connecter
        clientStates[clientId] = MenuState::LoginPseudo;
        sendMenuToClient(clientSocket, menu.getLoginMenu1());
    }
    else if (action == "2") { // Créer un compte
        clientStates[clientId] = MenuState::RegisterPseudo;
        sendMenuToClient(clientSocket, menu.getRegisterMenu1());
    }
    else if (action == "3") {
        // Quitter
        std::cout << "Client #" << clientId << " déconnecté." << std::endl;
        disconnectPlayer(clientId);
        return;
    }
    else {
        sendMenuToClient(clientSocket, menu.getMainMenu0());
    }
}

void Server::keyInputMainMenu(int clientSocket, int clientId, const std::string& action) {
    (void) clientSocket;
    if (action == "1") { // créer une gameRoom (choisir s'il veut créer sa partie ou rejoindre une partie)
        clientStates[clientId] = MenuState::JoinOrCreateGame;
    }
    else if (action == "2") {
        clientStates[clientId] = MenuState::Friends;
    }
    else if (action == "3") {
        // Classement
        clientStates[clientId] = MenuState::classement;
    }
    else if (action == "4") {
        // Teams
        clientStates[clientId] = MenuState::Team;
    }
    else if (action == "5") {
        // Retour à l'écran précédent
        clientStates[clientId] = MenuState::Welcome;
    }
}

/*
Login & Register
*/

void Server::keyInputRegisterPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    if (userManager.userNotExists(action)){
        // Si le pseudo n'existe pas, on stock en tmp
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::RegisterPassword;
        sendMenuToClient(clientSocket, menu.getRegisterMenu2());
    }
    else {
        returnToMenu(clientSocket,clientId, MenuState::RegisterPseudo, "pseudo already existe");
        sendMenuToClient(clientSocket, menu.getRegisterMenu1());
    }
}

void Server::keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Welcome);
        return;
    }
    userManager.registerUser(clientPseudo[clientId], action);
    sockToPseudo[clientSocket] = clientPseudo[clientId];
    pseudoTosocket[clientPseudo[clientId]] = clientSocket;
    clientStates[clientId] = MenuState::Main;
    sendMenuToClient(clientSocket, menu.getMainMenu1());
}


void Server::keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Welcome);
        return;
    }
    if (!userManager.userNotExists(action)) { // Si le pseudo existe
        std::cout << "Pseudo existe" << std::endl;
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::LoginPassword;
        sendMenuToClient(clientSocket, menu.getLoginMenu2());
    } 
    else {
        std::cout << "Pseudo n'existe pas" << std::endl;
        // si pseudo n'existe pas on annule et on retourne à l'étape 1 (dc dmd de pseudo)
        clientStates[clientId] = MenuState::LoginPseudo;
        sendMenuToClient(clientSocket, menu.getLoginMenuFailed1());
    }
}


void Server::keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    if (userManager.authenticateUser(clientPseudo[clientId], action)) { // Si le mot de passe est correct
        clientStates[clientId] = MenuState::Main;
        pseudoTosocket[clientPseudo[clientId]] = clientSocket;
        sockToPseudo[clientSocket] = clientPseudo[clientId];
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    } 
    else {
        // Si le mot de passe est incorrect, on retourne à l'étape 2 (demande de mot de passe)
        sendMenuToClient(clientSocket, menu.getLoginMenuFailed2());
    }
}

/*
Friends
*/

std::string Server::trim(const std::string& s) {
    const char* whitespace = " \t\n\r";
    std::size_t start = s.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    std::size_t end = s.find_last_not_of(whitespace);
    return s.substr(start, end - start + 1);
}

void Server::keyInputFriendsMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend);
    }
    else if (action == "2") {
        returnToMenu(clientSocket, clientId, MenuState::FriendList);
    }
    else if (action == "3") {
        returnToMenu(clientSocket, clientId, MenuState::FriendRequestList);
    }
    else if (action == "4") {
        returnToMenu(clientSocket, clientId, MenuState::PrivateChat);
    }else if( action == "5"){
        returnToMenu(clientSocket, clientId, MenuState::Main);
    }
}


void Server::keyInputManageFriendRequests(int clientSocket, int clientId, const std::string& action) {
    std::string currentUser = clientPseudo[clientId];

    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
        return;
    }

    std::vector<std::string> requests = userManager.getRequestList(currentUser);
    if (requests.empty()) {
        return;
    }

    if (action == "accept.all" || action == "reject.all") {
        bool accept = (action == "accept.all");
        for (const std::string& friend_request : requests) {
            accept ? userManager.acceptFriendRequest(currentUser, friend_request)
                   : userManager.rejectFriendRequest(currentUser, friend_request);
        }

        returnToMenu(clientSocket, clientId, MenuState::Friends,
            accept ? "Toutes les demandes d'amis ont été acceptées avec succès !" 
                   : "Toutes les demandes d'amis ont été rejetées avec succès !", 4);
        return;
    }

    std::string prefix_accept = "accept.";
    std::string prefix_reject = "reject.";
    bool isAccept = action.rfind(prefix_accept, 0) == 0;
    bool isReject = action.rfind(prefix_reject, 0) == 0;

    if (isAccept || isReject) {
        std::string friend_request = action.substr(isAccept ? prefix_accept.size() : prefix_reject.size());
        if(userManager.userNotExists(friend_request)){
            returnToMenu(clientSocket, clientId, MenuState::FriendRequestList, "Utilisateur n'exite pas !");
            return;
        }
        if(std::find(requests.begin(), requests.end(), friend_request) == requests.end()) {
            returnToMenu(clientSocket, clientId, MenuState::FriendRequestList, "L'utilisateur ne vous a pas envoyé de demande d'ami !");
            return;
        }
            
        isAccept ? userManager.acceptFriendRequest(currentUser, friend_request)
                 : userManager.rejectFriendRequest(currentUser, friend_request);

        returnToMenu(clientSocket, clientId, MenuState::FriendRequestList,
            (isAccept ? "Demande d'ami acceptée avec " : "Demande d'ami rejetée pour '") + friend_request + "'.");
        return;
    }

    returnToMenu(clientSocket, clientId, MenuState::Friends, 
                 "Erreur : Format invalide. Utilisez 'accept.pseudo', 'accept.all', 'reject.pseudo' ou 'reject.all'.");
}


void Server::keyInputManageFriendlist(int clientSocket, int clientId, const std::string& action) {
    std::string currentUser = clientPseudo[clientId];

    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
        return;
    }

    std::vector<std::string> friends = userManager.getFriendList(currentUser);
    if (friends.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::Main, "Vous n'avez aucun ami dans votre liste.");
        return;
    }

    if (action == "del.all") {
        for (const std::string& friend_name : friends) {
            userManager.deleteFriend(currentUser, friend_name);
        }

        returnToMenu(clientSocket, clientId, MenuState::Friends, "Tous vos amis ont été supprimés.");
        return;
    }

    std::string prefix_del = "del.";
    if (action.rfind(prefix_del, 0) == 0) {  // Vérifie si action commence par "del."
        std::string friend_name = action.substr(prefix_del.size());

        if (!userManager.areFriends(currentUser, friend_name)) {
            returnToMenu(clientSocket, clientId, MenuState::Friends, "Erreur : '" + friend_name + "' n'est pas dans votre liste d'amis.");
            return;
        }

        userManager.deleteFriend(currentUser, friend_name);
        returnToMenu(clientSocket, clientId, MenuState::Friends, friend_name + " a été supprimé de votre liste d'amis.");
        return;
    }

    returnToMenu(clientSocket, clientId, MenuState::FriendRequestList, "Erreur : Format invalide. Utilisez 'del.pseudo' ou 'del.all'.");
}


void Server::keyInputAddFriendMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
        return;
    }
    std::string currentUser;
    {
        std::lock_guard<std::mutex> lock(clientPseudoMutex);
        currentUser = clientPseudo[clientId];
    }
    std::string friend_request = trim(action);

    // Vérifier si l'utilisateur existe
    if (!userManager.userExists(friend_request)) {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend, "Erreur : L'utilisateur " + friend_request + " n'existe pas.");
        return;
    }

    // Vérifier si l'utilisateur essaie de s'ajouter lui-même
    if (friend_request == currentUser) {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend, "Erreur : Vous ne pouvez pas vous ajouter vous-même.");
        return;
    }

    // Vérifier si les deux utilisateurs sont déjà amis
    if (userManager.areFriends(currentUser, friend_request)) {
        returnToMenu(clientSocket, clientId, MenuState::Friends, "Erreur : Vous êtes déjà ami avec " + friend_request + ".");
        return;
    }


    // Envoyer la demande d'ami
    if(!userManager.sendFriendRequest(currentUser, friend_request))
         returnToMenu(clientSocket, clientId, MenuState::Friends, "Demande d'ami déja existe!");
    else returnToMenu(clientSocket, clientId, MenuState::Friends, "Demande d'ami'Demande d'ami envoyée à " + friend_request);
    
}

/*
Chat
*/

void Server::sendChatModeToClient(int clientSocket) {
    json message;
    message[jsonKeys::MODE] = "chat";
    message["pseudo"] = sockToPseudo[clientSocket];
    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0);
}


void Server::handleChat(int clientSocket, int clientId, json& receivedData){
    // Handle chat messages
    auto receiver = receiverOfMessages[clientSocket];
    std::cout << "Receiver " << receiver << std::endl;
    std::cout << "Reciver Message " << receivedData["receiver"].get<std::string>() << std::endl;
    bool isRoom = chatRoomsManage.checkroomExist(receiver);
    std::map<std::string, int> receivers;
    bool sendMessage = true;
    auto sender = clientPseudo[clientId];
    if (isRoom) {
        sender = receiver;
        auto members = chatRoomsManage.getMembers(receiver);
        for (const auto& member : members) {
            if (pseudoTosocket.find(member) != pseudoTosocket.end() && member != clientPseudo[clientId] && receiverOfMessages[pseudoTosocket[member]] == receiver) {
                receivers[member] = pseudoTosocket[member];
            } else {
                std::cerr << "Member " << member << " not found in pseudoTosocket." << std::endl;
            }
        }
    } else {
        receivers[receiver] = pseudoTosocket[receiver];
        sendMessage = (receiverOfMessages[pseudoTosocket[receiver]] == clientPseudo[clientId]);
    }
     // enregistrer le message dans la base de données si le message est valide et que le sender n 'a pas quité le chat
    if(isRoom) chatRoomsManage.saveMessageToRoom(clientPseudo[clientId], receiver, receivedData[jsonKeys::MESSAGE]);
    else chat.saveMessage(clientPseudo[clientId], receiver, receivedData[jsonKeys::MESSAGE]);
    
    if(!sendMessage){
        std::cout << "Not sending message to " << receiver << std::endl;
        return;
    }
    if(!chat.processClientChat(sender, receivers, receivedData)){
        if(isRoom){
            clientStates[clientId] = MenuState::Team;
        }else{
            clientStates[clientId] = MenuState::Friends;
        }
        return;
    }
   
}

void Server::keyInputPrivateChat(int clientSocket, int clientId, const std::string& action){
    if(action == "/back"){
        returnToMenu(clientSocket, clientId, MenuState::Main);
    }
    else if(userManager.userNotExists(action)){
        returnToMenu(clientSocket, clientId, MenuState::PrivateChat, "User does not exist.");
    }else if( !userManager.areFriends(sockToPseudo[clientSocket], action)){
        returnToMenu(clientSocket, clientId, MenuState::PrivateChat, "You are not friends with this user.");
    }else{
        receiverOfMessages[clientSocket] = action;                               //stocker le pseudo du destinataire
        clientStates[clientId] = MenuState::chat;                               //changer l'etat du client
        sendChatModeToClient(clientSocket);                                     //envoyer le mode de chat au client
        sleep(0.1); // Attendre un peu pour s'assurer que le client a reçu le mode de chat
        chat.sendOldMessages(clientSocket, sockToPseudo[clientSocket], action); //envoyer les anciens messages
    }
}

void Server::keyInputChatTeam(int clientSocket, int clientId, const std::string& action){
    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Team);
        return;
    }

    if (chatRoomsManage.checkroomExist(action)) { // Vérifie si la room existe
        if (chatRoomsManage.isClient(sockToPseudo[clientSocket], action)) {
            receiverOfMessages[clientSocket] = action;
            clientStates[clientId] = MenuState::chat;
            sendChatModeToClient(clientSocket);
            sleep(0.1); // Attendre un peu pour s'assurer que le client a reçu le mode de chat
            chatRoomsManage.sendOldMessages(clientSocket, action); //envoyer les anciens messages
        } else {
            returnToMenu(clientSocket, clientId, MenuState::Team, "Vous n'êtes pas membre de cette room.");
        }
    } else {
        returnToMenu(clientSocket, clientId, MenuState::Team, "La room spécifiée n'existe pas.");
    }
}


/*
Game Room
*/

void Server::keyInputJoinOrCreateGameMenu(int clientSocket, int clientId, const std::string& action) {
    (void)clientSocket;
    if (action == "1") {
        clientStates[clientId] = MenuState::CreateGame;
        }
    else if (action == "2") {
        clientStates[clientId] = MenuState::JoinGame;
        // Rejoindre une partie
    }
    else if (action == "3") {
        clientStates[clientId] = MenuState::Main;
    }
}

void Server::keyInputChooseGameModeMenu(int clientSocket, int clientId, const std::string& action){
    if(action == "1"){
        //Endless
        std::cout << "Client #" << clientId << " a sélectionné Endless." << std::endl;
        clientStates[clientId] = MenuState::Play;
        this->keyInputCreateGameRoom(clientId, GameModeName::Endless);

    }
    else if(action == "2"){
        //Duel
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu(2, "Duel", 1));
        this->keyInputCreateGameRoom(clientId, GameModeName::Duel);
        
    }
    else if(action == "3"){
        //classic
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu(3, "Classic", 1));
        this->keyInputCreateGameRoom(clientId, GameModeName::Classic);
    }
    else if(action == "4"){
        //royal competition
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu(3, "Royal Competition", 1));
        this->keyInputCreateGameRoom(clientId, GameModeName::Royal_Competition);
    }
    else if(action == "5"){
        clientStates[clientId] = MenuState::JoinOrCreateGame;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }
}

void Server::keyInputCreateGameRoom(int clientId, GameModeName gameMode) { //création de la gameRoom
    std::lock_guard<std::mutex> lock(gameRoomsMutex);

    std::cout << "Création de la GameRoom pour le client #" << clientId << " avec le mode " << static_cast<int>(gameMode) << "." << std::endl;
    
    int gameRoomIndex = gameRoomIdCounter.fetch_add(1);
    gameRooms[gameRoomIndex] = std::make_shared<GameRoom>(gameRoomIndex, clientId, gameMode);
    clientGameRoomId[clientId] = gameRoomIndex;

    //creer le chat pour la gameRoom
    std::string roomName = "GameRoom" + std::to_string(gameRoomIndex);
    chatRoomsManage.createTeam(roomName, clientPseudo[clientId]);
    
    //ajouter le client dans la gameRoom
    std::thread loopgame(&Server::loopGame, this, clientId);
    loopgame.detach();
}

void Server::deleteGameRoom(int roomId, const std::vector<int> players) {
    for (auto player : players)
        clientGameRoomId[player] = -1;
    chatRoomsManage.deleteChatRoom("GameRoom" + std::to_string(roomId));
    gameRooms.erase(roomId);
    gameRoomIdCounter.fetch_sub(1);
    
}

void Server::sendInputToGameRoom(int clientId, const std::string& action) {
    gameRooms[clientGameRoomId[clientId]]->input(clientId, action, "player");
}

void Server::sendInputToGameRoom2(int clientId, const std::string& action) {
    gameRooms[clientGameRoomId[clientId]]->input(clientId, action,"observer");
}

void Server::letObserversObserve(const std::vector<int>& observers) {
    for (int observer : observers) {
        clientStates[observer] = MenuState::Observer;
        menuStateManager->sendMenuStateToClient(clientIdToSocket[observer], 
                                              clientStates[observer], 
                                              "Mode observateur activé. Utilisez 'left' et 'right' pour changer de vue.");
    }
}

void Server::letPlayersPlay(const std::vector<int>& players) {
    for (int player : players){
        clientStates[player] = MenuState::Play;
        menuStateManager->sendMenuStateToClient(clientIdToSocket[player], clientStates[player], "Bienvenue dans menu modeJeu .");
    }
}

void Server::sendMiniGameToPlayer(int clientId, int clientSocket, std::shared_ptr<GameRoom> gameRoom) {
    json message;



    json otherPlayersGrids = json::array();
    for (int otherPlayerId : gameRoom->getPlayers()) {
        if (otherPlayerId != clientId) { // Ne pas inclure la grille du joueur principal
            std::shared_ptr<Game> otherGame = gameRoom->getGame(otherPlayerId);

            json otherPlayerData;
            otherPlayerData["grid"] = otherGame->gridToJson();
            otherPlayerData["tetra"] = otherGame->tetraminoToJson()[jsonKeys::TETRA_PIECE];
            otherPlayerData["playerId"] = gameRoom->getPlayerId(otherPlayerId) + 1;
            otherPlayersGrids.push_back(otherPlayerData);
            
        }
    }
    message["otherPlayersGrids"] = otherPlayersGrids;
    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0); // Un seul envoi
}

std::map<std::string,  std::vector<std::string>> Server::updateActivePlayerObserver(int clientId){
    std::map<std::string,  std::vector<std::string>> active;
    std::vector<int> players = gameRooms[clientGameRoomId[clientId]]->getPlayers();
    std::vector<int> observers = gameRooms[clientGameRoomId[clientId]]->getViewers();
    for (int player : players){
        std::string pseudo = clientPseudo[player];
        active["player"].push_back(pseudo);
    }

    for (int observer : observers){
        std::string pseudo = clientPseudo[observer];
        active["observer"].push_back(pseudo);
    }

    return active;
}



void Server::loopGame(int ownerId) {
    auto gameRoom = gameRooms[clientGameRoomId[ownerId]];
    std::vector<int> players;
    std::vector<int> observers;


    if (gamePreparation(ownerId, gameRoom)){
        players = gameRoom->getPlayers();
        observers = gameRoom->getViewers();
        int maxPlayers = gameRoom->getMaxPlayers();
        gameRoom->createGames();
        int countGameOvers = 0;
        letPlayersPlay(players);
        letObserversObserve(observers);
        gameRoom->startGame();

        auto lastMiniGameUpdateTime = std::chrono::steady_clock::now();
        
        while (gameRoom->getInProgress()) {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastMiniGameUpdateTime).count();
            if(gameRoom->getViewers().size() > observers.size()) {
                observers = gameRoom->getViewers();
            }
            
            countGameOvers = 0;
            for (auto player : players) {
                try {
                    if (gameRoom->getGameIsOver(player))
                    countGameOvers++;
                    else {
                        gameRoom->updatePlayerGame(player);
                        if (gameRoom->getNeedToSendGame(player)) {
                            sendGameToPlayer(player, clientIdToSocket[player], gameRoom);
                            if (elapsedTime >= 500){
                                sendMiniGameToPlayer(player, clientIdToSocket[player], gameRoom);
                                lastMiniGameUpdateTime = currentTime;
                            }
                            for(auto observer : observers) {
                                int idOclient = gameRoom->getclientobserverId(observer);
                                if (idOclient == player) {
                                    sendGameToPlayer(idOclient, clientIdToSocket[observer], gameRoom);
                                }
                                
                            }
                            gameRoom->setNeedToSendGame(false, player);
                        }
                    }
                    if (gameRoom->getAmountOfPlayers() != maxPlayers - countGameOvers) 
                    gameRoom->setAmountOfPlayers(maxPlayers - countGameOvers);
                } catch (const std::exception& e) {
                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                    disconnectPlayer(player);
                    gameRoom->removePlayer(player);
                    std::cerr << "Erreur lors de la mise à jour du jeu pour le joueur #" << player << ": " << e.what() << std::endl;
                }
            }
        }

        std::string message = "GAME OVER";
        for (auto player : players) {
            GameModeName gameMode = gameRoom->getGameModeName();
            try {
                clientStates[player] = MenuState::GameOver;
                menuStateManager->sendMenuStateToClient(clientIdToSocket[player], clientStates[player], "Game End");
                if (gameMode != GameModeName::Endless)
                    message = (!gameRoom->getGameIsOver(player)) ? "YOU WIN !!" : "GAME OVER";
                else
                    userManager.updateHighScore(clientPseudo[player], gameRoom->getScoreValue());
                sendMenuToClient(clientIdToSocket[player], menu.getEndGameMenu(message));
            } catch (const std::exception& e) {
            players.erase(std::remove(players.begin(), players.end(), player), players.end());
            disconnectPlayer(player);
            std::cerr << "Erreur lors de l'envoi du message de fin de jeu au joueur #" << player << ": " << e.what() << std::endl;
            }
        }
        for(auto observer : observers) {
            clientStates[observer] = MenuState::GameOver;
            sendMenuToClient(clientIdToSocket[observer], menu.getEndGameMenu("END GAME"));
            menuStateManager->sendMenuStateToClient(clientIdToSocket[observer], clientStates[observer], "Game End");
        }
    
    }

    {
    std::lock_guard<std::mutex> lock(gameRoomsMutex);
    int roomId = gameRoom->getRoomId();
    if (players.empty())
        players = gameRoom->getPlayers();
    deleteGameRoom(roomId, players);
    }
}

bool Server::gamePreparation(int ownerId, std::shared_ptr<GameRoom> gameRoom){
    if (gameRoom->getGameModeName() != GameModeName::Endless) {
        while(!(gameRoom->getSettingsDone() || gameRoom->getOwnerQuit()))
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (gameRoom->getOwnerQuit()) {
            gameRoom->endGame();
            for (auto player : gameRoom->getPlayers()) {
                if (player != ownerId){
                    clientStates[player] = MenuState::GameOver;
                    sendMenuToClient(clientIdToSocket[player], menu.getEndGameMenu("GAME END"));
                    menuStateManager->sendMenuStateToClient(clientIdToSocket[player], clientStates[player], "owner est quitte");
                }
                else {
                    clientStates[player] = MenuState::JoinOrCreateGame;
                    sendMenuToClient(clientIdToSocket[player], menu.getJoinOrCreateGame());
                    menuStateManager->sendMenuStateToClient(clientIdToSocket[player], clientStates[player], "Bienvenue dans menu creer ou rejoindre .");
                }
            }

            for (auto observer : gameRoom->getViewers()) {
                clientStates[observer] = MenuState::GameOver;
                sendMenuToClient(clientIdToSocket[observer], menu.getEndGameMenu("GAME END"));
                menuStateManager->sendMenuStateToClient(clientIdToSocket[observer], clientStates[observer], "owner est quitte");
            }
            return false;
        }
    }
    return true;
}


void Server::sendGameToPlayer(int clientId, int clientSocket, std::shared_ptr<GameRoom> gameRoom) {
    std::shared_ptr<Game> game = gameRoom->getGame(clientId);
    json message, tetraminos = game->tetraminoToJson();

    message[jsonKeys::SCORE] = game->scoreToJson();
    message[jsonKeys::GRID] = game->gridToJson();
    message[jsonKeys::TETRA_PIECE] = tetraminos[jsonKeys::TETRA_PIECE]; // Ajout du tétrimino dans le même message
    message[jsonKeys::NEXT_PIECE] = tetraminos[jsonKeys::NEXT_PIECE];

    message[jsonKeys::MESSAGE_CIBLE] = gameRoom->messageToJson(clientId);
    message[jsonKeys::PLAYER_NUMBER] = gameRoom->getPlayerId(clientId) + 1;
    std::cout << "Player number: " << message[jsonKeys::PLAYER_NUMBER] << std::endl;
    std::string msg = message.dump() + "\n";

    send(clientSocket, msg.c_str(), msg.size(), 0); // Un seul envoi
}



void Server::keyInputSendGameRequestMenu(int clientSocket, int clientId, std::string receiver, std::string status) {
    //### GÉRER L'INVITATION D'UN VIEWER
    std::string currentUser = clientPseudo[clientId];

    if(receiver == currentUser){
        sendMenuToClient(clientSocket, menu.displayMessage("Erreur : Vous ne pouvez pas vous envoyer une invitation de jeu."));
        sleep(2);
        return;
    }
    if(!userManager.areFriends(currentUser,receiver)){
        sendMenuToClient(clientSocket, menu.displayMessage("Erreur : Vous n'êtes pas ami avec "+receiver+"."));
        sleep(2);
        return;
    }
    int gameRoomId = clientGameRoomId[clientId];
    userManager.sendInvitationToFriend(gameRoomId, currentUser, receiver,status);
    sendMenuToClient(clientSocket, menu.displayMessage("Request Sent To "+receiver+"."));
    sleep(2);

}


void Server::keyInputHelpMenu(int clientSocket, int clientId, const std::string& action) {
    auto gameRoom = gameRooms[clientGameRoomId[clientId]];
    if (action.find("/back") == 0){
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu(getMaxPlayers(clientId), getMode(clientId), getAmountOfPlayers(clientId), gameRoom->getSpeed(), gameRoom->getEnergyLimit()));
    }
    else {
        bool isRC = gameRoom->getGameModeName() == GameModeName::Royal_Competition;
        bool canEditMaxPlayer = gameRoom->getGameModeName() == GameModeName::Classic || isRC;
        keyInputLobbySettingsMenu(clientSocket, clientId, action);
        sendMenuToClient(clientSocket, menu.getHelpMenu(isRC, canEditMaxPlayer));
    }
}

void Server::keyInputLobbySettingsMenu(int clientSocket, int clientId, const std::string& action) {
    std::string status, receiver;
    if (action.find("/invite") == 0) {
        // Extraire le statut et le receiver de l'action
        extractDataBetweenSlashes("/invite", action, status, receiver);

        // Appeler la méthode pour gérer l'envoi de la demande de jeu
        keyInputSendGameRequestMenu(clientSocket, clientId, receiver, status);        
    }

    else if (action.find("/friend") == 0){
        // Extraire le statut et le receiver de l'action
        extractDataBetweenSlashes("/friend", action, status, receiver);     
        if (status == "add")
            keyInputAddFriendMenu(clientSocket, clientId, receiver);
        else if (status == "delete")
            keyInputManageFriendlist(clientSocket, clientId, "del." + receiver);
        else if (status == "ban")
            keyInputManageFriendlist(clientSocket, clientId, "ban." + receiver);
        else if (status == "unban")
            keyInputManageFriendlist(clientSocket, clientId, "unban." + receiver); 
    }

    auto gameRoom = gameRooms[clientGameRoomId[clientId]];

    if (action.find("/help") == 0){
        clientStates[clientId] = MenuState::Help;
        bool isRC = gameRoom->getGameModeName() == GameModeName::Royal_Competition;
        bool canEditMaxPlayer = gameRoom->getGameModeName() == GameModeName::Classic || isRC;
        sendMenuToClient(clientSocket, menu.getHelpMenu(isRC, canEditMaxPlayer));
        return;
    }
    
    if (action.find("/back") == 0){
        if (gameRoom->getOwnerId() == clientId)
            gameRoom->setOwnerQuit();
        else{
            std::vector<int> players = gameRoom->getPlayers();
            if(std::find(players.begin(), players.end(), clientId) != players.end())
                gameRoom->removePlayer(clientId);
            else
                gameRoom->removeViewer(clientId);
        }
            
        clientStates[clientId] = MenuState::JoinOrCreateGame;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
        menuStateManager->sendMenuStateToClient(clientIdToSocket[clientId], clientStates[clientId], "Bienvenue dans menu rejoindre et creer .");

        std::map<std::string,  std::vector<std::string>> active = updateActivePlayerObserver(clientId);
        for (const auto& cId : gameRoom->getPlayers()){
            sendMenuToClient(pseudoTosocket[clientPseudo[cId]], menu.getLobbyMenu(getMaxPlayers(cId), getMode(cId), getAmountOfPlayers(cId), gameRoom->getSpeed(), gameRoom->getEnergyLimit()));
            auto friends = userManager.getFriendList(clientPseudo[cId]);
            if (gameRoom->getOwnerId() == cId){
                menuStateManager->sendMenuStateToClient(clientIdToSocket[cId], clientStates[cId], "owner", friends, {}, active);
            }else{
                menuStateManager->sendMenuStateToClient(clientIdToSocket[cId], clientStates[cId], "player", friends, {}, active);
            }
                
        }

        for (const auto& observerId : gameRoom->getViewers()) {
            sendMenuToClient(pseudoTosocket[clientPseudo[observerId]], menu.getLobbyMenu(getMaxPlayers(observerId ), getMode(observerId ), getAmountOfPlayers(observerId ), gameRoom->getSpeed(), gameRoom->getEnergyLimit()));
            menuStateManager->sendMenuStateToClient(clientIdToSocket[observerId], clientStates[observerId], "observer", {}, {}, active);
        }
        return;
    }
    
    else if (gameRoom != nullptr)
        gameRoom->input(clientId, action, "player");
    
    if ((gameRoom != nullptr) && (!gameRoom->getInProgress()) && (clientStates[clientId] == MenuState::Settings)){
        //refresh le Lobby
        sendMenuToClient(clientSocket, menu.getLobbyMenu(getMaxPlayers(clientId), getMode(clientId), getAmountOfPlayers(clientId), gameRoom->getSpeed(), gameRoom->getEnergyLimit()));
    }   
}



void Server::extractDataBetweenSlashes(const std::string& toFind, const std::string& action, std::string& status, std::string& receiver){
    // Extraire le statut et le receiver de l'action
    std::size_t firstBackSlash = action.find('/', std::string(toFind).size());  // Trouver le premier \ après \invite
    if (firstBackSlash == std::string::npos) {
        // Si le format est incorrect, retourner sans faire rien
        return;
    }

    std::size_t secondBackSlash = action.find('/', firstBackSlash + 1);  // Trouver le deuxième \ après le statut
    if (secondBackSlash == std::string::npos) {
        // Si le format est incorrect, retourner sans faire rien
        return;
    }

    // Extraire le status (par exemple: observer, player)
    status = action.substr(firstBackSlash + 1, secondBackSlash - firstBackSlash - 1); 
    
    // Extraire le receiver (nom du joueur)
    receiver = action.substr(secondBackSlash + 1); 
}

void Server::keyInputChoiceGameRoom(int clientSocket, int clientId, const std::string& action){
    if (action.find("accept.") == 0){
        std::size_t firstDot = action.find(".");
        std::size_t secondDot = action.find(".", firstDot + 1);

        if (firstDot != std::string::npos && secondDot != std::string::npos) {
            // Extraire le status(player/observer)
            std::string status = action.substr(firstDot + 1, secondDot - firstDot - 1);
        
            // Extraire le numéro de la gameRoom
            std::string nbreRoom = action.substr(secondDot + 1);
            
            auto currentClient = clientPseudo[clientId];
            std::vector<std::vector<std::string>> invitations = userManager.getListGameRequest(currentClient);
            bool invitationFound = false;

            // Vérifier si l'invitation existe
            for (const auto& invi : invitations){
                if(invi[1]==status && invi[2]==nbreRoom){
                    invitationFound = true;
                    break;
                }
            }
            // Vérifier si l'invitation existe
            if (!invitationFound) {
                returnToMenu(clientSocket, clientId, MenuState::JoinGame, "Erreur : Vous n'avez pas d'invitation pour cette salle.");
                return;
            }
            int roomNumber = std::stoi(nbreRoom);
            // Vérifier si la salle existe
            if (gameRooms.find(roomNumber) == gameRooms.end()) {
                returnToMenu(clientSocket, clientId, MenuState::JoinGame, "Erreur : La salle demandée n'existe pas.");
                return;
            }

            //accepter l'invitation
            userManager.acceptGameInvitation(roomNumber, clientPseudo[clientId]);
            
            clientGameRoomId[clientId] = roomNumber;
            auto gameRoom = gameRooms[roomNumber];
            
            //ajouter le joueur à la salle de chat de la game Room
            std::string roomName = "GameRoom" + std::to_string(roomNumber);
            chatRoomsManage.addClient(sockToPseudo[clientSocket], roomName);
            // Envoyer le message de bienvenue dans la salle de chat

            if (status == "player") {
                gameRoom->addPlayer(clientId);
                clientStates[clientId] = MenuState::Settings;
                sendMenuToClient(clientSocket, menu.displayMessage("Vous avez rejoint la partie en tant que joueur."));
            } else if (status == "observer") {
                gameRoom->addViewer(clientId); // Ajouter en tant qu'observateur
                if (gameRoom->getInProgress()){                    
                    clientStates[clientId] = MenuState::Observer;
                    
                }
                else {
                    clientStates[clientId] = MenuState::Settings;
                    sendMenuToClient(clientSocket, menu.displayMessage("Vous observez la partie. Utilisez 'left' et 'right' pour changer de joueur."));
                }
            }

        
            if(!gameRoom->getInProgress()){
                std::map<std::string,  std::vector<std::string>> active = updateActivePlayerObserver(clientId);
                for (const auto& cId : gameRoom->getPlayers()){
                    sendMenuToClient(pseudoTosocket[clientPseudo[cId]], menu.getLobbyMenu(getMaxPlayers(cId), getMode(cId), getAmountOfPlayers(cId), gameRoom->getSpeed(), gameRoom->getEnergyLimit()));
                    auto friends = userManager.getFriendList(clientPseudo[cId]);
                    if (gameRoom->getOwnerId() == cId){
                        menuStateManager->sendMenuStateToClient(clientIdToSocket[cId], clientStates[cId], "owner", friends, {}, active);
                    }else{
                        menuStateManager->sendMenuStateToClient(clientIdToSocket[cId], clientStates[cId], "player", friends, {}, active);
                    }
                
                }

                for (const auto& observerId : gameRoom->getViewers()) {
                    sendMenuToClient(pseudoTosocket[clientPseudo[observerId]], menu.getLobbyMenu(getMaxPlayers(observerId ), getMode(observerId ), getAmountOfPlayers(observerId ), gameRoom->getSpeed(), gameRoom->getEnergyLimit()));
                    menuStateManager->sendMenuStateToClient(clientIdToSocket[observerId], clientStates[observerId], "observer", {}, {}, active);
                }
            }
        
            
        } else {
            std::cerr << "Format invalide pour l'action : " << action << std::endl;
        }
    }

    else if(action.find("/back") == 0){
        clientStates[clientId] = MenuState::JoinOrCreateGame;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }
}

std::string Server::getMode(int clientId){
    int gameRoomId = clientGameRoomId[clientId];
    GameModeName mode = gameRooms[gameRoomId]->getGameModeName();
    std::string modestr;

    switch(mode){
        case GameModeName::Endless:
            modestr = "Endless";
            break;
        case GameModeName::Duel:
            modestr = "Duel";
            break;
        case GameModeName::Classic:
            modestr = "Classic";
            break;
        case GameModeName::Royal_Competition:
            modestr = "Royal Competition";
            break;
        default:
            modestr = "";
            break;

    }
    return modestr;
}

int Server::getMaxPlayers(int clientId){
    int gameRoomId = clientGameRoomId[clientId];
    return gameRooms[gameRoomId]->getMaxPlayers();
}

int Server::getAmountOfPlayers(int clientId){
    int gameRoomId = clientGameRoomId[clientId];
    return gameRooms[gameRoomId]->getAmountOfPlayers();
}

/*
Teams Chat
*/

void Server::keyInputTeamsMenu(int clientSocket, int clientId, const std::string& action) {
    if(action == "1") {
        clientStates[clientId] = MenuState::CreateTeam;
    }
    else if(action == "2") {
        clientStates[clientId] = MenuState::JoinTeam;
    }
    else if(action == "3"){
        clientStates[clientId] = MenuState::TeamsInvitation;
    }
    else if(action == "4") {
        clientStates[clientId] = MenuState::ManageTeams;
    }
    else if(action == "5") {    //chat Room
        clientStates[clientId] = MenuState::TeamChat;
    }
    else if(action == "6") {
        returnToMenu(clientSocket, clientId, MenuState::Main);
    }
}


void Server::keyInputCreateChatRoom(int clientSocket, int clientId, const std::string& action) {
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Team);
    }
    else if(chatRoomsManage.checkroomExist(action)){
        returnToMenu(clientSocket, clientId, MenuState::Team, "Room name already exists.");
    }
    else{
        chatRoomsManage.createTeam(action, sockToPseudo[clientSocket]);
        returnToMenu(clientSocket, clientId, MenuState::Team, "Room created successfully.");
    }

}

void Server::keyInputJoinChatRoom(int clientSocket, int clientId, const std::string& action) {
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Team);
    }
    else if(!chatRoomsManage.checkroomExist(action)){
        returnToMenu(clientSocket, clientId, MenuState::Team, "Room name does not exist.");
    }
    else if(chatRoomsManage.isClient(action, sockToPseudo[clientSocket])){
        returnToMenu(clientSocket, clientId, MenuState::Team, "You are already a member in this room");
    }
    else if(chatRoomsManage.isPendingReq(action, sockToPseudo[clientSocket])){
        returnToMenu(clientSocket, clientId, MenuState::Team, "You already sent a request to join this room");
    }
    else{
        chatRoomsManage.joinTeam(sockToPseudo[clientSocket], action);
        returnToMenu(clientSocket, clientId, MenuState::Team, "Request to join room sent succesfully.");
    }
}

void Server::keyInputTeamsInvitation(int clientSocket, int clientId, const std::string& action){
    std::vector<std::string> requests;
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Team);
    }
    else if(action.rfind("accept.", 0) == 0 || action.rfind("refuse.", 0) == 0){
        auto option = action.substr(0, 7);
        auto roomName = action.substr(7);
        auto client = clientPseudo[clientId];
        //si la room est supprimé
        if(!chatRoomsManage.checkroomExist(roomName)){
            returnToMenu(clientSocket, clientId, MenuState::TeamsInvitation, "Room name does not exist.");
        }
        else{
            if(option == "accept."){
                chatRoomsManage.acceptClientRequest(client , roomName);
                returnToMenu(clientSocket, clientId, MenuState::TeamsInvitation, "Request to join room accepted.");
            }
            else{
                chatRoomsManage.refuseClientRequest(client, roomName);
                returnToMenu(clientSocket, clientId, MenuState::TeamsInvitation, "Request to join room refused.");
            }
        }
    }
}


void Server::keyInputManageTeams(int clientSocket, int clientId, const std::string& action) {
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Team);
    }
    else if(!chatRoomsManage.checkroomExist(action)){ 
        returnToMenu(clientSocket, clientId, MenuState::Team, "Room name does not exist.");
    } 
    else {
        roomToManage[clientId] = action;
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
}


void Server::keyInputManageTeam(int clientSocket, int clientId, const std::string& action) {
    auto roomName = roomToManage[clientId];
    if(action == "1"){
            returnToMenu(clientSocket, clientId, MenuState::ListTeamMembres);
            return;
        }
        bool isAdmin = chatRoomsManage.isAdmin(sockToPseudo[clientSocket], roomName);
        if(isAdmin) {
            if(action == "2") {
                clientStates[clientId] = MenuState::AddMembre;
                sendMenuToClient(clientSocket, menu.getAddMembreMenu());
            }
            else if(action == "3") {
                clientStates[clientId] = MenuState::AddAdmin;
                sendMenuToClient(clientSocket, menu.getAddAdmin());
            }
            else if(action == "4") {
                //les demandes d'ajout
                clientStates[clientId] = MenuState::RoomRequestList;
            }
            //Supression de la room
            else if(action == "5") {
                if (chatRoomsManage.getMembers(roomName).size() == 1) {
                    clientStates[clientId] = MenuState::ConfirmDeleteRoom;
                    sendMenuToClient(clientSocket, menu.getDeleteRoomConfirmationMenu(roomName));

                } else {
                    clientStates[clientId] = MenuState::ConfirmQuitRoom;
                    sendMenuToClient(clientSocket, menu.getQuitRoomConfirmationMenu(roomName));
                }
            }else if(action == "6") {
                returnToMenu(clientSocket, clientId, MenuState::ManageTeams);//retour
            }
        }
        else {
            if(action == "2") {
                returnToMenu(clientSocket, clientId, MenuState::ConfirmQuitRoom);
            }else if(action == "3") {
                returnToMenu(clientSocket, clientId, MenuState::ManageTeams);//retour
            }
        }
}


void Server::keyInputListMembres(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
    else if(action.rfind("del.", 0) == 0){
        std::string pseudo = action.substr(4);
        if (chatRoomsManage.isClient(pseudo,roomName)){
            chatRoomsManage.removeClient(pseudo,roomName);
            returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User removed successfully.");
        }else if (chatRoomsManage.removeClient(roomName, pseudo)){
            returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User removed successfully.");
        }
    }
    
    else {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }

}

void Server::keyInputAddMembre(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];

    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
    else if (chatRoomsManage.isClient(action, roomName)) { 
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User is already in the room.");
    }
    else if (userManager.userNotExists(action)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User does not exist.");
    }
    else if (chatRoomsManage.sendInvitationToClient(action, roomName)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "Request sent successfully.");
    }
    else {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User has already sent a Request to Join Room !");
    }
}


void Server::keyInputAddAdmin(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
    else if(chatRoomsManage.isAdmin(action, roomName)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User is already an admin.");
    }
    else if(!chatRoomsManage.isClient(action, roomName)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User is not in the room.");
    }
    else {
        chatRoomsManage.addAdmin(action, roomName);
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User is now an admin.");
    }
}

void Server::keyInputRequestList(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
    else if(action.rfind("accept.", 0) == 0){
        std::string pseudo = action.substr(7);
        if (chatRoomsManage.isPendingReq(pseudo, roomName )){
            chatRoomsManage.acceptClientRequest(pseudo, roomName);
            returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User added successfully.");
        }
        else if(chatRoomsManage.isClient(pseudo, roomName)){
            returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User is already in the room.");
        }
    }
    else if(action.rfind("reject.", 0) == 0){
        std::string pseudo = action.substr(7);
        if (chatRoomsManage.isPendingReq(pseudo, roomName)){
            chatRoomsManage.refuseClientRequest(pseudo, roomName);
            returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User removed successfully.");
        }
        else if(chatRoomsManage.isClient(pseudo, roomName)){
            returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User is already in the room.");
        }
    }
    else {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
}

void Server::keyInputQuitRoom(int clientSocket, int clientId, const std::string& action){
    std::string roomName = roomToManage[clientId];
    std::string userPseudo = sockToPseudo[clientSocket];

    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
        return;
    }
    bool isAdmin = chatRoomsManage.isAdmin(userPseudo, roomName);
    if (action == "oui"){
        if (isAdmin) {
            if (chatRoomsManage.getAdmins(roomName).size() > 1) {
                chatRoomsManage.removeAdmin(userPseudo, roomName);
                returnToMenu(clientSocket, clientId, MenuState::ManageTeams, "You have left the room.");
            } else {
                chatRoomsManage.deleteChatRoom(roomName);
                returnToMenu(clientSocket, clientId, MenuState::ManageTeams, "Room deleted successfully.");
            }
        } else {
            chatRoomsManage.removeClient(userPseudo, roomName);
            returnToMenu(clientSocket, clientId, MenuState::ManageTeams, "You have left the room.");
        }        
    }else{
        //retour au menu de gestion de la room (si on a pas quitté/ supprimé la room)
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
}

void Server::keyInputConfirmDeleteRoom(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if (action == "oui") {
        chatRoomsManage.deleteChatRoom(roomName);
        returnToMenu(clientSocket, clientId, MenuState::ManageTeams, "Room '" + roomName + "' supprimée avec succès.");
    } else {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeams, "Suppression de la room est refuseee.");
    }
}

//Définir les actions possibles dans le classement
void Server::keyInputRankingMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "/back") { 
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    }
}

void Server::keyInputGameOverMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        clientStates[clientId] = MenuState::JoinOrCreateGame;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }
    else if (action == "2") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    }
}



int main() {
    // le client ne doit pas l'igniorer faudra sans doute faire un handler pour le SIGPIPE ? 
    signal(SIGPIPE, SIG_IGN);  // le client arrivait à crasher le serveur en fermant la connexion
    
    try {
        std::ofstream serverLog("server.log"); // Créer un fichier de log
        // Rediriger std::cout et std::cerr vers le fichier log
        std::cout.rdbuf(serverLog.rdbuf());
        std::cerr.rdbuf(serverLog.rdbuf());

        Server server(12345);
        if (!server.start()) {
            std::cerr << "Erreur: Impossible de démarrer le serveur." << std::endl;
            return 1;
        }

        while (true) {
            server.acceptClients();
        } 

        server.stop();
    } 
    catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }
    return 0;
}

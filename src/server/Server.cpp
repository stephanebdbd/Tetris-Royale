#include "Server.hpp"

#include "../common/json.hpp"
#include <ncurses.h>
#include <unistd.h>
#include <csignal>
#include <thread>
#include <netinet/in.h>
#include <fstream>
#include <iostream>


Server::Server(int port, Game* game) 
    : port(port), serverSocket(-1), game(game), clientIdCounter(0)
    {
        userManager = std::make_unique<UserManager>("./users.txt");
    
        friendList = std::make_unique<FriendList>();
    
    }

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
    
    clientStates[clientId] = MenuState::Welcome;

    clear();
    sendMenuToClient(clientSocket, game->getMainMenu0()); 
    refresh();

    // Lancer un thread pour gérer ce client
    std::thread clientThread(&Server::handleClient, this, clientSocket, clientId);
    clientThread.detach();
}


void Server::handleClient(int clientSocket, int clientId) {
    char buffer[1024];

    while (true) {

        if(runningChats[clientId]) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Attendre 100ms avant de vérifier à nouveau
            continue;
        }

        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Client #" << clientId << " déconnecté." << std::endl;
            runningGames[clientId] = false; // Arrêter la partie du client
            games.erase(clientId); // Supprimer la partie du client
            runningGames.erase(clientId); // Supprimer le booléen de jeu
            clientStates.erase(clientId); // Supprimer l'état des menus du client
            close(clientSocket); // Fermer la connexion
            break;
        }

        try {
            json receivedData = json::parse(buffer);
            
            if (!receivedData.contains("action") || !receivedData["action"].is_string()) {
                std::cerr << "Erreur: 'action' manquant ou invalide dans le JSON reçu." << std::endl;
                return;
            }
        
            std::string action = receivedData["action"];
        
            handleMenu(clientSocket, clientId, action);
             
            // Si le joueur est en jeu, lancer un thread pour recevoir les inputs
            if (runningGames[clientId]) {
                receiveInputFromClient(clientSocket, clientId);
            }
        
        } catch (json::parse_error& e) {
            std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
        }
        
    }
}




void Server::handleMenu(int clientSocket, int clientId, const std::string& action) {
    auto state = clientStates[clientId];
    switch (state) {
        case MenuState::Welcome:
            keyInputWelcomeMenu(clientSocket, clientId, action);
            break;
        case MenuState::RegisterPseudo:
            keyInputRegisterPseudoMenu(clientSocket, clientId, action);
            break;
        case MenuState::RegisterPassword:
            keyInputRegisterPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::LoginPseudo:
            keyInputLoginPseudoMenu(clientSocket, clientId, action);
            break;
        case MenuState::LoginPassword:
            std::cout << "LoginPassword" << std::endl;
            keyInputLoginPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::Main:
            keyInputMainMenu(clientSocket, clientId, action);
            break;
        case MenuState::Game:
            keyInputGameMenu(clientSocket, clientId, action);
            break;
        case MenuState::classement:
            // Classement
            break;
        case MenuState::chat:
            keyInputChatMenu(clientSocket, clientId, action);
            break;
        case MenuState::Friends:
            keyInputFriendsMenu(clientSocket, clientId, action);
            break;
        case MenuState::AddFriend:
            keyInputAddFriendMenu(clientSocket, clientId, action);
            break;
        case MenuState::FriendList:
            keyInputManageFriendlist(clientSocket, clientId,action);
            break;
        case MenuState::FriendRequestList:
            keyInputManageFriendRequests(clientSocket, clientId,action);
            break;

        default:
            std::cerr << "Erreur : état inconnu dans handleMenu !" << std::endl;
            break;
    }


}
void Server::keyInputManageFriendRequests(int clientSocket, int clientId, const std::string& action) {
    std::string currentUser = clientPseudo[clientId];
    if (action == "./ret") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        return;
    }

    // Vérifier si l'action est vide
    if (action.empty()) {
        return;
    }

    std::vector<std::string> requests = friendList->getRequestList(currentUser);
    if (requests.empty()) {
        sendMenuToClient(clientSocket, game->displayMessage("Vous n'avez aucune demande d'ami en attente."));
        sleep(3);
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        return;
    }

    // Gestion des commandes "accept.all" et "reject.all"
    if (action == "accept.all" || action == "reject.all") {
        for (const std::string& friend_request : requests) {
            if (action == "accept.all") {
                friendList->acceptFriendRequest(currentUser, friend_request);
            } else {
                friendList->rejectFriendRequest(currentUser, friend_request);
            }
        }

        sendMenuToClient(clientSocket, game->displayMessage((action == "accept.all") ? 
            " Toutes les demandes d'amis ont été acceptées avec succès ! " :
            " Toutes les demandes d'amis ont été rejetées."));
        sleep(4);
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        return;
    }

    // Vérification du format "accept.pseudo" ou "reject.pseudo"
    std::string prefix_accept = "accept.";
    std::string prefix_reject = "reject.";
    
    if (action.substr(0, prefix_accept.size()) == prefix_accept) {
        std::string friend_request = action.substr(prefix_accept.size());
        
        if (!friendList->isPendingRequest(friend_request,currentUser)) {
            sendMenuToClient(clientSocket, game->displayMessage("⚠️ Erreur : Aucune demande d'ami en attente de '" + friend_request + "'."));
            sleep(3);
            clientStates[clientId] = MenuState::Main;
            sendMenuToClient(clientSocket, game->getMainMenu1());
            return;
        }

        friendList->acceptFriendRequest(currentUser, friend_request);
        sleep(3);
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        sendMenuToClient(clientSocket, " Demande d'ami acceptée avec '" + friend_request + ".");
    } 
    else if (action.substr(0, prefix_reject.size()) == prefix_reject) {
        std::string friend_request = action.substr(prefix_reject.size());
        
        if (!friendList->isPendingRequest(currentUser, friend_request)) {
            sendMenuToClient(clientSocket, game->displayMessage("⚠️ Erreur : Aucune demande d'ami en attente de '" + friend_request + "'."));
            sleep(3);
            clientStates[clientId] = MenuState::Main;
            sendMenuToClient(clientSocket, game->getMainMenu1());
            return;
        }

        friendList->rejectFriendRequest(currentUser, friend_request);
        sendMenuToClient(clientSocket, " Demande d'ami rejetée pour '" + friend_request + "'.");
        sleep(3);
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
    } 
    else {
        sendMenuToClient(clientSocket, game->displayMessage("⚠️ Erreur : Format invalide. Utilisez 'accept.pseudo', 'accept.all', 'reject.pseudo' ou 'reject.all'."));
        sleep(3);
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        return;
    }

}

void Server::keyInputManageFriendlist(int clientSocket, int clientId, const std::string& action) {
    std::string currentUser = clientPseudo[clientId];
    if (action == "./ret") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        return;
    }
    // Vérifier si l'action est vide
    if (action.empty()) {
        
        sendMenuToClient(clientSocket, game->displayMessage("⚠️ Erreur : Aucune action spécifiée."));
        sleep(3);
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        return;
    }

    std::vector<std::string> friends = friendList->getFriendList(currentUser);
    if (friends.empty()) {
        sendMenuToClient(clientSocket, game->displayMessage("📭 Vous n'avez aucun ami dans votre liste."));
        sleep(3);
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        return;
    }

    // Gestion de "del.all"
    if (action == "del.all") {
        for (const std::string& friend_name : friends) {
            friendList->removeFriend(currentUser, friend_name);
        }

        sendMenuToClient(clientSocket, game->displayMessage("🗑️ Tous vos amis ont été supprimés."));
        sleep(3);
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        return;
    }

    // Vérification du format "del.pseudo"
    std::string prefix_del = "del.";

    if (action.substr(0, prefix_del.size()) == prefix_del) {
        std::string friend_name = action.substr(prefix_del.size());

        if (!friendList->areFriends(currentUser, friend_name)) {
            sendMenuToClient(clientSocket, game->displayMessage("⚠️ Erreur : '" + friend_name + "' n'est pas dans votre liste d'amis."));
            sleep(3);
            clientStates[clientId] = MenuState::Main;
            sendMenuToClient(clientSocket, game->getMainMenu1());
            return;
        }

        friendList->removeFriend(currentUser, friend_name);
        sendMenuToClient(clientSocket, game->displayMessage(friend_name + "' a été supprimé de votre liste d'amis."));
    } else {
        sendMenuToClient(clientSocket, game->displayMessage("⚠️ Erreur : Format invalide. Utilisez 'del.pseudo' ou 'del.all'."));
        sleep(3);
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        return;
    }

}






void Server::keyInputFriendsMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "./ret") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        return;
    }
    if (action == "1") {
        clientStates[clientId] = MenuState::AddFriend;
        sendMenuToClient(clientSocket, game->getAddFriendMenu());
    
    }
    else if (action == "2") {
        clientStates[clientId] = MenuState::FriendList;
        sendMenuToClient(clientSocket, game->getFriendListMenu(friendList->getFriendList(clientPseudo[clientId])));
        
    }
    else if (action == "3") {
        clientStates[clientId] = MenuState::FriendRequestList;
        sendMenuToClient(clientSocket, game->getRequestsListMenu(friendList->getRequestList(clientPseudo[clientId])));
    
    }
    else if (action == "4") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
    }

}

void Server::keyInputAddFriendMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "./ret") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        return;
    }
    if (action.empty()) {

        return;
    }

    std::string friend_request = action; 
    std::string currentUser = clientPseudo[clientId]; 

    // Vérifier si l'utilisateur existe
    if (!friendList->userExists(friend_request)) {
        sendMenuToClient(clientSocket, game->displayMessage("Erreur : L'utilisateur " + friend_request + " n'existe pas."));
        sleep(3);
        clientStates[clientId] = MenuState::AddFriend;
        sendMenuToClient(clientSocket, game->getAddFriendMenu());
        
        return;
    }

    // Vérifier si l'utilisateur essaie de s'ajouter lui-même
    if (friend_request == currentUser) {
        sendMenuToClient(clientSocket, game->displayMessage("Erreur : Vous ne pouvez pas vous ajouter vous-même."));
        sleep(3);
        clientStates[clientId] = MenuState::AddFriend;
        sendMenuToClient(clientSocket, game->getAddFriendMenu());
        return;
    }

    // Vérifier si les deux utilisateurs sont déjà amis
    if (friendList->areFriends(currentUser, friend_request)) {
        sendMenuToClient(clientSocket, game->displayMessage("Erreur : Vous êtes déjà ami avec " + friend_request + "."));
        sleep(3);
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());

        return;
    }

    // Envoyer la demande d'ami
    friendList->sendFriendRequest(currentUser, friend_request);
    sendMenuToClient(clientSocket, game->displayMessage("Demande d'ami envoyée à " + friend_request + ". Veuiller consulter la listes des amis pour voir si la demande a été acceptée."));
    sleep(3);
    clientStates[clientId] = MenuState::Main;
    sendMenuToClient(clientSocket, game->getMainMenu1());
}

void Server::keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {  // Se connecter
        clientStates[clientId] = MenuState::LoginPseudo;
        sendMenuToClient(clientSocket, game->getLoginMenu1());
    }
    else if (action == "2") { // Créer un compte
        clientStates[clientId] = MenuState::RegisterPseudo; // Passage à l'état Register
        sendMenuToClient(clientSocket, game->getRegisterMenu1());
    }
    else if (action == "3") {
        // Quitter
        close(clientSocket);
    }
}

void Server::keyInputRegisterPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    if (userManager->userNotExists(action)) { 
        // Si le pseudo n'existe pas, on stock en tmp
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::RegisterPassword;
        sockToPseudo[clientSocket] = clientPseudo[clientId];
        sendMenuToClient(clientSocket, game->getRegisterMenu2());
    } 
    else {
        sendMenuToClient(clientSocket, game->getRegisterMenuFailed());
    }
}

void Server::keyInputRegisterPseudoMenuFailed(int clientSocket, int clientId, const std::string& action) {
    if (userManager->userNotExists(action)) { 
        // Si le pseudo n'existe pas, on stock en tmp
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::RegisterPassword;
        pseudoTosocket[action] = clientSocket;
        sockToPseudo[clientSocket] = clientPseudo[clientId];
        std::cout << "Client #" << clientId << " connecté en tant que " << clientPseudo[clientId] << std::endl;
        sendMenuToClient(clientSocket, game->getRegisterMenu2());
    } 
    else {
        sendMenuToClient(clientSocket, game->getRegisterMenuFailed());
    }
}

void Server::keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    userManager->registerUser(clientPseudo[clientId], action);
    //friendList->registerUser(clientPseudo[clientId]);
    pseudoTosocket[clientPseudo[clientId]] = clientSocket;
    clientPseudo.erase(clientId);
    clientStates[clientId] = MenuState::Main;
    sendMenuToClient(clientSocket, game->getMainMenu1());
}

void Server::keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    if (!userManager->userNotExists(action)) { // Si le pseudo existe
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::LoginPassword;
        sendMenuToClient(clientSocket, game->getLoginMenu2());
    } 
    else {
        // si pseudo n'existe pas on annule et on retourne à l'étape 1 (dc dmd de pseudo)
        sendMenuToClient(clientSocket, game->getLoginMenuFailed1());
    }
}

void Server::keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    if (userManager->authenticateUser(clientPseudo[clientId], action)) { // Si le mot de passe est correct
        clientStates[clientId] = MenuState::Main;
        pseudoTosocket[clientPseudo[clientId]] = clientSocket;
        sockToPseudo[clientSocket] = clientPseudo[clientId];
        std::cout << "Client #" << clientId << " connecté en tant que " << clientPseudo[clientId] << std::endl;
        sendMenuToClient(clientSocket, game->getMainMenu1());

    } 
    else {
        // Si le mot de passe est incorrect, on retourne à l'étape 2 (dmd de mdp)
        sendMenuToClient(clientSocket, game->getLoginMenuFailed2());
    }
}


void Server::keyInputMainMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "./ret") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
        return;
    }
    if (action == "1") {
        clientStates[clientId] = MenuState::JoinOrCreateGame;
        sendMenuToClient(clientSocket, game->getJoinOrCreateGame());

    }
    if (action == "2") {
        //ici on va gerer la liste des amis
        clientStates[clientId] = MenuState::Friends;
        sendMenuToClient(clientSocket, game->getFriendMenu());
        //processClientFriendList(clientSocket, clientId);

    }

    else if (action == "3") {
        // classement => à implémenter 
        
    }

    else if (action == "4") {
        // Chat
        clientStates[clientId] = MenuState::chat;
        sendMenuToClient(clientSocket, game->getChatMenu());

    }
    
    else if (action == "5") {
        // Retour à l'écran précédent
        clientStates[clientId] = MenuState::Welcome;
        sendMenuToClient(clientSocket, game->getMainMenu0());
    }
}

void Server::keyInputJoinOrCreateGameMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        // Créer une partie
        clientStates[clientId] = MenuState::GameMode;
        sendMenuToClient(clientSocket, game->getGameMode());
    }
    else if (action == "2") {
        // Rejoindre une partie
    }else if (action == "3") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
    }
}

void Server::keyInputChatMenu(int clientSocket, int clientId, const std::string& action) {
    if(action == "1") {
        // a implémenter
    }else if(action == "2") {
        // a implémenter
    }else if(action == "3") {
        // a implémenter
    }else if(action == "4") {
        sendChatModeToClient(clientSocket);
        runningChats[clientId] = true;
        chat->processClientChat(clientSocket, clientId, *this, MenuState::chat, game->getChatMenu());
    }else if(action == "5") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, game->getMainMenu1());
    }
}

void Server::keyInputModeGameMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        // endless
    }
    else if (action == "2") {
        // classic
    }
    else if (action == "3") {
        // duel
    } 
    else if (action == "4") {
        // royal competition
    }else if (action == "5") {
        clientStates[clientId] = MenuState::JoinOrCreateGame;
        sendMenuToClient(clientSocket, game->getJoinOrCreateGame());
        return;
    }
    receiveInputFromClient(clientSocket, clientId); // lance un thread pour recevoir les inputs
    clientStates[clientId] = MenuState::Game;
    runningGames[clientId] = true; 

    games[clientId] = std::make_unique<Game>(10, 20);
    loopGame(clientSocket, clientId);
}



void Server::keyInputGameMenu(int clientSocket, int clientId,const std::string& unicodeAction) {
    std::string action = convertUnicodeToText(unicodeAction);  // Convertir \u0005 en "right"

    auto& game = games[clientId];

    if (action == "right") { 
        game->moveCurrentPieceRight();
        game->setNeedToSendGame(true);
    }
    else if (action == "left") { 
        game->moveCurrentPieceLeft();
        game->setNeedToSendGame(true);
    }
    else if (action == "up") { 
        game->rotateCurrentPiece();
        game->setNeedToSendGame(true);
    }
    else if (action == "down"){
        game->moveCurrentPieceDown();
        game->setNeedToSendGame(true);
    }
    else if(action == "drop") { // space
        game->dropCurrentPiece();
        game->setNeedToSendGame(true);
    }
    if (game->getNeedToSendGame()){ 
        sendGameToClient(clientSocket, clientId);
        game->setNeedToSendGame(false);
    }
}

std::string Server::convertUnicodeToText(const std::string& unicode) {
    auto action = unicodeToText.find(unicode);
    return (action != unicodeToText.end()) ? action->second : "///"; 
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

void Server::sendGameToClient(int clientSocket, int clientId) { //TODO: Deplacer le main pour faire un fichier game ??? 
    auto& game = games[clientId]; // Récupérer la partie du client

    json message;
    
    message["score"] = game->getScore().scoreToJson();
    message["grid"] = game->getGrid().gridToJson();
    message["tetraPiece"] = game->getCurrentPiece().tetraminoToJson(); // Ajout du tétrimino dans le même message

    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0); // Un seul envoi
}

//recuperer les inputs du client
void Server::receiveInputFromClient(int clientSocket, int clientId) {
    std::thread inputThread([this, clientSocket, clientId]() {
        char buffer[1024];

        while (runningGames[clientId]) {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); 
            if (bytesReceived > 0) {
                try {
                    json receivedData = json::parse(buffer);
                    std::string action = receivedData["action"];

                    std::cout << "Action reçue du client " << clientId << " : " << action << std::endl;
                    keyInputGameMenu(clientSocket, clientId, action);
                } 
                catch (json::parse_error& e) {
                    std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
                }
            }
        }
    });

    inputThread.detach(); // Permet d’exécuter en parallèle
}

void Server::loopGame(int clientSocket, int clientId) {
    auto& game = games[clientId]; // Récupérer la partie du client
    
    std::thread gameThread([this, clientId]() { // Lancer un thread pour mettre à jour le jeu
        auto& gameInstance = games[clientId];
        while (runningGames[clientId]) {
            gameInstance->update(); 
        }
    });

    gameThread.detach();

    while (runningGames[clientId]) { 
        if (game->getNeedToSendGame()) { 
            sendGameToClient(clientSocket, clientId);
            game->setNeedToSendGame(false);
        }
    }
}

void Server::sendChatModeToClient(int clientSocket) {
    json message;
    message["mode"] = "chat";
    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0);
}

int main() {
    // le client ne doit pas l'igniorer faudra sans doute faire un handler pour le SIGPIPE ? 
    signal(SIGPIPE, SIG_IGN);  // le client arrivait à crasher le serveur en fermant la connexion
    
    try {
        std::ofstream serverLog("server.log"); // Créer un fichier de log
        // Rediriger std::cout et std::cerr vers le fichier log
        std::cout.rdbuf(serverLog.rdbuf());
        std::cerr.rdbuf(serverLog.rdbuf());

        Game game(10, 20);
        Server server(12345, &game);
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

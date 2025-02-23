#include "ClientChat.hpp"

using json = nlohmann::json;

bool ClientChat::messagesWaitForDisplay = false;

void ClientChat::run(){
    //initialiser la mémoire des messages
    if (!initMessageMemory()) {
        std::cerr << "Erreur lors de l'initialisation de la mémoire des messages !\n";
        return;
    }
    std::cout << "Mode chat activé. Tapez votre message et appuyez sur Entrée.\n";
    
    //lancer un tread pour envoyer les messages du chat
    std::thread sendThread(&ClientChat::sendChatMessages, this);
    sendThread.detach();
    
    
    while (true) {
        //recevoir les messages du chat
        receiveChatMessages();
    }
    //vider la mémoire des messages
    if (messagesWaitForDisplay &&!FlushMemory()) {
        std::cerr << "Erreur lors de la suppression de la mémoire des messages !\n";
    }

}


void ClientChat::sendChatMessages() {
    while (true) {
        std::string input;
        std::getline(std::cin, input);

        if (input.empty()) {
            std::cerr << "Le message ne peut pas être vide !\n";
            continue;
        }

        if (input.size() < 4 || input[0] != '.' || input[1] != '/') {
            std::cerr << "Format invalide ! Utilisez: ./receiver-name message\n";
            continue;
        }

        size_t pos = input.find(' ');
        if (pos == std::string::npos) {
            std::cerr << "Format incorrect !\n";
            continue;
        }

        std::string receiver = input.substr(2, pos - 2);
        std::string message = input.substr(pos + 1);

        if (message.empty()) {
            std::cerr << "Le message ne peut pas être vide !\n";
            continue;
        }

        // Construire le message JSON
        json msg_json = {{"receiver", receiver}, {"message", message}};
        std::string msg = msg_json.dump();

        // Envoi du message
        if (!network.sendData(msg, clientSocket)) {
            std::cerr << "Erreur d'envoi du message !\n";
        }
    }

}


void ClientChat::receiveChatMessages() {
    char buffer[1024];

    while (true) {
        int bytes_received = network.receivedData(clientSocket, buffer);
        if(bytes_received == -1){
            std::cerr << "Erreur lors de la réception du message !\n";
            return;
        }

        try {
            json msg = json::parse(std::string(buffer, bytes_received));
            if(isPlaying)
                saveMessage(msg.dump());
            else 
                displayChatMessage(msg["sender"], msg["message"]);


        } catch (const std::exception& e) {
            std::cerr << "Erreur JSON: " << e.what() << std::endl;
        }
    }
}

void ClientChat::displayChatMessage(std::string sender, const std::string& message){
    // Obtenir les dimensions de la fenêtre
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // Calculer la position centrale
    int y = rows - 1; // Dernière ligne
    int x = 0; // Coin gauche

    mvprintw(y, x, "[%s] : %s", sender.c_str(), message.c_str());
}

bool ClientChat::initMessageMemory() {
    
    std::ifstream file("messages.json");
    if (!file.good()) { // Vérifie si le fichier existe
        std::ofstream newFile("messages.json");
        if (newFile.is_open()) {
            newFile << ""; // Fichier vide (on ajoute ligne par ligne)
            newFile.close();
            return true;
        }
    }
    return false;
}

bool ClientChat::saveMessage(const std::string& message) {

    std::ofstream file("messages.json", std::ios::app); // Mode append
    if (file.is_open()) {
        file << message << std::endl; // Écrire JSON en une seule ligne
        file.close();
        messagesWaitForDisplay = true;
        return true;
    }
    return false;

}

bool ClientChat::FlushMemory() {

    std::ifstream file("messages.json");
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) { // Lire ligne par ligne
            json message = json::parse(line);
            std::cout << message.dump(4) << std::endl;
        }
        file.close();

        // Nettoyer le fichier après extraction
        std::ofstream clearFile("messages.json", std::ios::trunc);
        messagesWaitForDisplay = false;
        return true;

    }
    return false;

}

void ClientChat::setIsPlaying(bool isPlaying){
    this->isPlaying = isPlaying;
}

void ClientChat::setClientSocket(int clientSocket){
    this->clientSocket = clientSocket;
}
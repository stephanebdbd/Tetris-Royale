#include "ClientChat.hpp"
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>

using json = nlohmann::json;
std::mutex mtx; // Mutex pour synchroniser l'accès aux fenêtres ncurses

#define INPUT_HEIGHT 3  // Hauteur de la zone de saisie
bool ClientChat::messagesWaitForDisplay = false;

void ClientChat::run() {
    initscr();       // Initialise ncurses
    cbreak();        // Désactive le buffering de ligne
    noecho();        // Empêche l'affichage automatique des entrées utilisateur
    keypad(stdscr, TRUE); // Active la gestion des touches spéciales
    scrollok(stdscr, TRUE); // Permet le défilement du texte

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x); // Récupère les dimensions du terminal

    WINDOW *chat_win = newwin(max_y - INPUT_HEIGHT, max_x, 0, 0);
    WINDOW *input_win = newwin(INPUT_HEIGHT, max_x, max_y - INPUT_HEIGHT, 0);
    scrollok(chat_win, TRUE);
    box(input_win, 0, 0);
    wrefresh(chat_win);
    wrefresh(input_win);

    if (!initMessageMemory()) {
        std::cerr << "Erreur lors de l'initialisation de la mémoire des messages !\n";
        endwin();
        return;
    }

    std::thread sendThread(&ClientChat::sendChatMessages, this, input_win);
    sendThread.join();  // Attendre la fin du thread avant de détruire l'objet


    while (true) {
        receiveChatMessages();
    }

    delwin(chat_win);
    delwin(input_win);
    endwin();
}

void ClientChat::sendChatMessages(WINDOW *input_win) {
    std::string inputStr;
    int ch;
    while (true) {
        mtx.lock();
        werase(input_win);
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 1, "%s", inputStr.c_str());
        wrefresh(input_win);
        mtx.unlock();

        std::string receiver, message;

        ch = getch();
        if (ch == 10) {  // Entrée
            if (inputStr.empty()) continue;
            if (inputStr.size() < 4 || inputStr[0] != '.' || inputStr[1] != '/') {
                std::cerr << "Format invalide ! Utilisez: ./receiver-name message\n";
                continue;
            }
            if(inputStr == "./exit"){
                receiver = "server";
                message = "exit";
                break;
            }else{
                size_t pos = inputStr.find(' ');
                if (pos == std::string::npos) {
                    std::cerr << "Format incorrect !\n";
                    continue;
                }
                receiver = inputStr.substr(2, pos - 2);
                message = inputStr.substr(pos + 1);
            }

            if (message.empty()) continue;
            json msg_json = { {"receiver", receiver}, {"message", message} };

            if (!network.sendData(msg_json.dump(), clientSocket)) {
                std::cerr << "Erreur d'envoi du message !\n";
            }
            displayChatMessage("Moi->"+receiver, message);
            y++;
            inputStr.clear();

        } else if (ch == 127 || ch == KEY_BACKSPACE) {
            if (!inputStr.empty()) inputStr.pop_back();
        } else if (isprint(ch)) {
            inputStr += static_cast<char>(ch);
        }
    }
}

void ClientChat::receiveChatMessages() {
    char buffer[1024];
    while (true) {
        int bytes_received = network.receivedData(clientSocket, buffer);
        if (bytes_received <= 0) {
            std::cerr << "Erreur lors de la réception du message !\n";
            return;
        }

        try {
            json msg = json::parse(std::string(buffer, bytes_received));
            std::cout << "Message reçu: " << msg.dump() << std::endl;
            
            if (isPlaying)
                saveMessage(msg.dump());
            else{
                displayChatMessage(msg["sender"], msg["message"]);y++;
            }
                
        } catch (const std::exception& e) {
            std::cerr << "Erreur JSON: " << e.what() << std::endl;
        }
    }
}

void ClientChat::displayChatMessage(std::string sender, const std::string& message) {
    mtx.lock();
    mvprintw(y, 0, "[%s] : %s", sender.c_str(), message.c_str());
    refresh();
    mtx.unlock();
}

bool ClientChat::initMessageMemory() {
    std::ifstream file("messages.json");
    if (!file.good()) {
        std::ofstream newFile("messages.json");
        if (newFile.is_open()) {
            newFile.close();
            return true;
        }
        std::cerr << "Erreur lors de la création du fichier messages.json." << std::endl;
        return false;
    }
    return true;
}

bool ClientChat::saveMessage(const std::string& message) {
    std::ofstream file("messages.json", std::ios::app);
    if (file.is_open()) {
        file << message << std::endl;
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
        while (getline(file, line)) {
            json message = json::parse(line);
            std::cout << message.dump(4) << std::endl;
        }
        file.close();
        std::ofstream clearFile("messages.json", std::ios::trunc);
        messagesWaitForDisplay = false;
        return true;
    }
    return false;
}

void ClientChat::setIsPlaying(bool isPlaying) {
    this->isPlaying = isPlaying;
}

void ClientChat::setClientSocket(int clientSocket) {
    this->clientSocket = clientSocket;
}

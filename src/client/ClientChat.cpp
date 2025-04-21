#include "ClientChat.hpp"
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>


std::mutex mtx; // Mutex pour synchroniser l'accès aux fenêtres ncurses
#define INPUT_HEIGHT 4  // Hauteur de la zone de saisie

void ClientChat::run() {
    noecho();        // Empêche l'affichage automatique des entrées utilisateur
    curs_set(1); // Rendre le curseur visible
    keypad(stdscr, TRUE); // Activer les touches spéciales
    nodelay(stdscr, TRUE); // Ne pas bloquer l'entrée utilisateur
    //start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Vos messages
    init_pair(2, COLOR_WHITE, COLOR_BLACK); // Messages reçus

    int height, width;
    getmaxyx(stdscr, height, width); // Récupère les dimensions du terminal

    // Fenêtre pour afficher les messages (en haut)
    displayWin = newwin(height - 3, width, 0, 0); // Hauteur : height - 3, Largeur : width
    scrollok(displayWin, TRUE); // Activer le défilement pour cette fenêtre
    box(displayWin, 0, 0); // Dessiner une bordure autour de la fenêtre
    wrefresh(displayWin); // Rafraîchir la fenêtre

    // Fenêtre pour saisir les messages (en bas)
    inputWin = newwin(INPUT_HEIGHT, width, height - INPUT_HEIGHT, 0);
    scrollok(inputWin, TRUE); // Activer le défilement pour cette fenêtre
    box(inputWin, 0, 0); // Dessiner une bordure autour de la fenêtre
    
    wrefresh(inputWin); // Rafraîchir la fenêtre

    std::thread sendThread(&ClientChat::sendChatMessages, this); // Lancer le thread d'envoi de messages
    sendThread.join(); // Attendre la fin du thread d'envoi
    chatMessages.clear(); // Vider les messages du chat

    delwin(displayWin);
    delwin(inputWin);
    curs_set(0); // Rendre le curseur invisible
    echo(); // Réactiver l'affichage automatique des entrées utilisateur
}


void ClientChat::sendChatMessages() {
    std::string inputStr;
    std::string constReceiver;
    json msg_json;
    int ch;

    while (true) {
        mtx.lock();
        werase(inputWin);
        box(inputWin, 0, 0);
        mvwprintw(inputWin, 1, 1, "> %s", inputStr.c_str()); // Afficher le buffer de saisie
        wrefresh(inputWin);
        mtx.unlock();

        ch = getch();
        // Vérifier si la touche up ou down est pressée
        if (ch == KEY_UP) {
            // Gérer les flèches haut/bas
            continue;
        }
        if (ch == 10) {  // Entrée
            if (inputStr.empty()) continue;

            msg_json = {
                {"message", inputStr},
            };
            
            if (!network.sendData(msg_json.dump(), clientSocket)) {
                std::cerr << "Erreur d'envoi du message !\n";
            }
            if (inputStr == "/exit") {
                std::cout << "Déconnexion du chat." << std::endl;
                break;
            }
            // Afficher le message dans la fenêtre de chat
            msg_json["sender"] = MyPseudo;
            addChatMessage(msg_json);
            displayChatMessages();
            inputStr.clear();

        } else if (ch == 127 || ch == KEY_BACKSPACE) {
            if (!inputStr.empty()) inputStr.pop_back();
        } else if (isprint(ch)) {
            inputStr += static_cast<char>(ch);
        }
    }
    y = 1;
}


void ClientChat::receiveChatMessages(const json& msg) {
    chatMessages.push_back(msg);
    displayChatMessages();
}


void ClientChat::displayMessage(const std::string& sender, const std::string& message) {
    mtx.lock();
    int max_y, max_x;
    getmaxyx(displayWin, max_y, max_x);
    int current_y = getcury(displayWin);

    // Si on est en bas de la fenêtre, faire défiler
    if (current_y >= max_y - 2) {
        scroll(displayWin);
        current_y = max_y - 2;
    }

    bool isMyMessage = (sender == MyPseudo);
    std::string formatted_msg = "[" + sender + "]: " + message;

    // Calculer la position x
    int x_pos = isMyMessage ? (max_x - formatted_msg.length() - 3) : 2;

    // Si le message est trop long, le diviser en plusieurs lignes
    size_t start = 0;
    while (start < formatted_msg.length()) {
        int available_width = isMyMessage ? (max_x - x_pos - 2) : (max_x - 4);
        std::string line = formatted_msg.substr(start, available_width);
        
        if (isMyMessage) {
            // Recalculer la position pour chaque ligne (au cas où)
            x_pos = max_x - line.length() - 3;
            if (x_pos < 2) x_pos = 2; // Ne pas dépasser à gauche
        }
        
        mvwprintw(displayWin, current_y++, x_pos, "%s", line.c_str());
        start += available_width;

        // Si on atteint le bas après avoir ajouté une ligne, faire défiler
        if (current_y >= max_y - 2) {
            scroll(displayWin);
            current_y = max_y - 2;
        }
    }

    wrefresh(displayWin);
    mtx.unlock();
}

void ClientChat::displayChatMessages() {
    mtx.lock();
    if (!displayWin) { // Check if displayWin is valid
        std::cerr << "Error: displayWin is not initialized.\n";
        mtx.unlock();
        return;
    }

    werase(displayWin); // Clear the window
    
    int max_y, max_x;
    getmaxyx(displayWin, max_y, max_x);
    int current_y = 1; // Start below the border
    
    for (const auto& msg : chatMessages) {
        std::string sender = msg["sender"];
        std::string message = msg["message"];
        bool isMyMessage = (sender == MyPseudo);
        
        std::string formatted_msg = "[" + sender + "]: " + message;
        int x_pos = isMyMessage ? (max_x - formatted_msg.length() - 3) : 2;

        // Handle multi-line messages
        size_t start = 0;
        while (start < formatted_msg.length()) {
            int available_width = isMyMessage ? (max_x - x_pos - 2) : (max_x - 4);
            std::string line = formatted_msg.substr(start, available_width);
            
            if (isMyMessage) {
                x_pos = max_x - line.length() - 3;
                if (x_pos < 2) x_pos = 2;
                wattron(displayWin, COLOR_PAIR(1));
            } else {
                wattron(displayWin, COLOR_PAIR(2));
            }
            
            mvwprintw(displayWin, current_y++, x_pos, "%s", line.c_str());
            wattroff(displayWin, COLOR_PAIR(isMyMessage ? 1 : 2));
            start += available_width;

            if (current_y >= max_y - 1) {
                scroll(displayWin);
                current_y = max_y - 2;
            }
        }
    }
    box(displayWin, 0, 0);
    wrefresh(displayWin);
    mtx.unlock();
}

void ClientChat::addChatMessage(const json& msg) {
    chatMessages.push_back(msg);
}

void ClientChat::setClientSocket(int clientSocket) {
    this->clientSocket = clientSocket;
}

void ClientChat::setMyPseudo(const std::string& pseudo) {
    MyPseudo = pseudo;
}

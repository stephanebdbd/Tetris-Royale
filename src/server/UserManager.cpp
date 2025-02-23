#include "UserManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// Constructeur qui charge les utilisateurs depuis le fichier
UserManager::UserManager(const std::string& filename) : file(filename) {
    loadUsers();
}

// Enregistre un nouvel utilisateur si le pseudo n'existe pas encore
bool UserManager::registerUser(const std::string& username, const std::string& password) {
    if (users.find(username) != users.end()) {
        return false; // L'utilisateur existe déjà
    }
    users[username] = {password, 0}; // Nouveau joueur avec un highscore de 0
    saveUsers();
    return true;
}

// Vérifie si les identifiants fournis sont valides
bool UserManager::authenticateUser(const std::string& username, const std::string& password) {
    return users.find(username) != users.end() && users[username].password == password;
}

// Retourne le highscore d'un joueur
int UserManager::getHighscore(const std::string& username) {
    if (users.find(username) != users.end()) {
        return users[username].highscore;
    }
    return 0;
}

// Met à jour le highscore si le nouveau score est plus élevé
void UserManager::updateHighscore(const std::string& username, int score) {
    if (users.find(username) != users.end() && score > users[username].highscore) {
        users[username].highscore = score;
        saveUsers();
    }
}

// Charge les utilisateurs depuis le fichier texte
void UserManager::loadUsers() {
    std::ifstream infile(file);
    if (!infile) return;
    std::string line, username, password;
    int highscore;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        if (std::getline(iss, username, ':') && std::getline(iss, password, ':') && iss >> highscore) {
            users[username] = {password, highscore};
        }
    }
}

// Sauvegarde la liste des utilisateurs dans le fichier texte
void UserManager::saveUsers() {
    std::ofstream outfile(file);
    for (const auto& [username, data] : users) {
        outfile << username << ":" << data.password << ":" << data.highscore << "\n";
    }
}

std::vector<std::pair<std::string, int>> UserManager::getRanking() const {
    std::vector<std::pair<std::string, int>> ranking;
    for (const auto& [username, data] : users) {
        ranking.push_back({username, data.highscore});
    }
    // Tri décroissant par apport au highscore
    std::sort(ranking.begin(), ranking.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });
    return ranking;
}

#include "query_result.hpp"
#include <iostream>

// Constructeur par défaut
QueryResult::QueryResult() : error(SUCCESS) {}

// Méthode pour obtenir le premier élément de la première ligne
std::string QueryResult::getFirst() const {
    if (!data.empty() && !data[0].empty()) {
        return data[0][0];
    }
    return "";
}

std::vector<std::string> QueryResult::getVector(int columnIndex) const {
    std::vector<std::string> result;
    for (const auto& row : data) {
        if (columnIndex < static_cast<int>(row.size())) {
            result.push_back(row[columnIndex]);  // Ajoute la valeur de la colonne spécifiée
        }
    }
    return result;
}


// Méthode pour vérifier si tout s'est bien passé
bool QueryResult::isOk() const {
    return error == SUCCESS;
}

// Méthode pour obtenir l'erreur (si elle existe)
std::string QueryResult::getError() const {
    if (isOk()) return "Tout s'est bien passé !";
    return humanReadableError(error);
}

std::string humanReadableError(const std::string& rawError) {
    if (rawError.find("UNIQUE constraint failed") != std::string::npos)
        return "Unicité violée : cet élément existe déjà.";
    if (rawError.find("FOREIGN KEY") != std::string::npos)
        return "Clé étrangère : un ID référencé est manquant.";
    if (rawError.find("syntax error") != std::string::npos)
        return "Erreur de syntaxe dans la requête SQL.";
    return " Erreur inconnue : " + rawError;
}
std::vector<std::vector<std::string>> QueryResult::getData() const {
    return data;
}
#ifndef DATA_HPP
#define DATA_HPP

#include "../common/json.hpp"  // Inclusion de la bibliothèque JSON
using json = nlohmann::json;
#include <string>
#include <set>
#include <fstream>
#include <iostream>

class Data {
public:
    // Constructeur de la classe Data
    Data() = default;

    // Destructeur de la classe Data
    ~Data() = default;

    json openFile(const std::string& filename) const;
    void writeFile(const std::string& filename, const json& j) const;
    // Méthodes de gestion des données

    virtual void saveData(const std::string& filename, const std::string& dataLoc, const std::string& data) const;
    virtual void deleteData(const std::string& filename, const std::string& dataLoc, const std::string& data) const;
    virtual std::vector<std::string> loadData(const std::string& filename, const std::string& dataLoc) const;
};

#endif // DATA_HPP
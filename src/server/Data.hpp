#ifndef DATA_HPP
#define DATA_HPP

#include "../common/json.hpp"  // Inclusion de la bibliothèque JSON
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
    virtual bool isInKey(const std::string& filename, const std::string& key, const std::string& pseudo) const;

    virtual void saveData(const std::string& filename, const std::string& key, const std::string& value) const;
    virtual void deleteData(const std::string& filename, const std::string& key, const std::string& value) const;
    virtual std::vector<std::string> loadData(const std::string& filename, const std::string& key) const;
};

#endif // DATA_HPP
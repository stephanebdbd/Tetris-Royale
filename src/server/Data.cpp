#include "Data.hpp"

json Data::openFile(const std::string& filename) const {
    std::ifstream file(filename);
    json j;
    if (file.is_open()) {
        file >> j;
    } else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
    return j;
}

void Data::writeFile(const std::string& filename, const json& j) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    } else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
}


// Méthodes de gestion des données
void Data::saveData(const std::string& filename, const std::string& key, const std::string& value) const {
    std::cout << "Saving value to " << key << " : " << value << std::endl;
    try {
        json j = openFile(filename);

        j[key].emplace_back(value);

        writeFile(filename, j);
    } catch (const std::exception& e) {
        std::cerr << "Error writing to file (save): " << e.what() << std::endl;
    }
}


void Data::deleteData(const std::string& filename, const std::string& key, const std::string& value) const {
    std::cout << "Deleting value from " << key << " : " << value << std::endl;
    try {
        json j = openFile(filename);

        j[key].erase(std::remove(j[key].begin(), j[key].end(), value), j[key].end());
        
        writeFile(filename, j);
    } catch (const std::exception& e) {
        std::cerr << "Error writing to file(delete): " << e.what() << std::endl;
    }
}

std::vector<std::string> Data::loadData(const std::string& filename, const std::string& key) const {
    std::vector<std::string> value;
    try {
        json j = openFile(filename);
        
        value = j[key].get<std::vector<std::string>>();
    } catch (const std::exception& e) {
        std::cerr << "Error reading from file(load): " << e.what() << std::endl;
    }
    return value;
}
bool Data::isInKey(const std::string& filename, const std::string& key, const std::string& pseudo) const {
    try {
        json j = openFile(filename);
        if (j.contains(key)) {
            return std::find(j[key].begin(), j[key].end(), pseudo) != j[key].end();
        }
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error checking in key (isInKey): " << e.what() << std::endl;
    }
    return false;
}
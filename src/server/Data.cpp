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
void Data::saveData(const std::string& filename, const std::string& dataLoc, const std::string& data) const {
    std::cout << "Saving data to " << dataLoc << " : " << data << std::endl;
    try {
        json j = openFile(filename);

        j[dataLoc].push_back(data);

        writeFile(filename, j);
    } catch (const std::exception& e) {
        std::cerr << "Error writing to file (save): " << e.what() << std::endl;
    }
}


void Data::deleteData(const std::string& filename, const std::string& dataLoc, const std::string& data) const {
    try {
        json j = openFile(filename);

        j[dataLoc].erase(std::remove(j[dataLoc].begin(), j[dataLoc].end(), data), j[dataLoc].end());
        
        writeFile(filename, j);
    } catch (const std::exception& e) {
        std::cerr << "Error writing to file(delete): " << e.what() << std::endl;
    }
}

std::vector<std::string> Data::loadData(const std::string& filename, const std::string& dataLoc) const {
    std::vector<std::string> data;
    try {
        json j = openFile(filename);
        
        data = j[dataLoc];
    } catch (const std::exception& e) {
        std::cerr << "Error reading from file(load): " << e.what() << std::endl;
    }
    return data;
}
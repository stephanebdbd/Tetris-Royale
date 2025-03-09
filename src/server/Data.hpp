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

    // Méthodes de gestion des données
    template <typename T>
    void saveData(const std::string& filename, const std::string& dataLoc, const T& data) const {
        std::cout << "Saving data to " << dataLoc << " : " << data << std::endl;
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl;
                return;
            }
            json j;
            file >> j;
            file.close();

            j[dataLoc].push_back(data);

            std::ofstream outFile(filename, std::ios::trunc);
            if (!outFile.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl;
                return;
            }
            outFile << j.dump(4);
            outFile.close();
        } catch (const std::exception& e) {
            std::cerr << "Error writing to file: " << e.what() << std::endl;
        }
    }

    template <typename T>
    void deleteData(const std::string& filename, const std::string& dataLoc, const T& data) const {
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl;
                return;
            }
            json j;
            file >> j;
            file.close();

            j[dataLoc].erase(std::remove(j[dataLoc].begin(), j[dataLoc].end(), data), j[dataLoc].end());
            
            std::ofstream outFile(filename, std::ios::trunc);
            if (!outFile.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl;
                return;
            }
            outFile << j.dump(4);
            outFile.close();
        } catch (const std::exception& e) {
            std::cerr << "Error writing to file: " << e.what() << std::endl;
        }
    }

    std::set<std::string> loadData(const std::string& filename, const std::string& dataLoc) const {
        std::set<std::string> data;
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl;
                return data;
            }
            json j;
            file >> j;
            data = j[dataLoc].get<std::set<std::string>>();
            file.close();
        } catch (const std::exception& e) {
            std::cerr << "Error reading from file: " << e.what() << std::endl;
        }
        return data;
    }

    // Setter et getter
    void setFilename(const std::string& filename);
    std::string getFilename() const;
};

#endif // DATA_HPP
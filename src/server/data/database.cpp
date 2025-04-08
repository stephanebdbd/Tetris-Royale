#include "database.hpp"
#include "query_result.hpp"

#include <iostream>
#include <sqlite3.h>
#include <fstream>
#include <sstream>
#include <ctime>

#define DATABASE_DIR "data/full_schema.sql"
#define DB_NAME "mama.db"



QueryResult DataBase::createDb() {
    QueryResult result;
    int rc = sqlite3_open(DB_NAME, &this->db);
    if (rc != SQLITE_OK) {
        this->db = nullptr;
        closeConnection();
        result.error = sqlite3_errmsg(this->db);
        std::cout << "Erreur : Impossible de créer ou d'ouvrir la base de données. Code d'erreur : "
                  << rc << ", " << result.error << std::endl;
        return result;
    }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    result.error = SUCCESS;
    return result;
}



QueryResult DataBase::createTables(){
    QueryResult result;

    // Read DDL file
    std::ifstream file(DATABASE_DIR);
    if (!file.is_open()) {
        // Si l'ouverture du fichier échoue, mettre l'erreur dans le QueryResult
        result.error = "Erreur : Impossible d'ouvrir le fichier DDL.";
        std::cout << result.error << std::endl;
        return result; // Retourner avec l'erreur
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sql = buffer.str();

    // Execute DDL file
    char* errMsg = nullptr;
    int rc = sqlite3_exec(this->db, sql.c_str(), NULL, 0, &errMsg);
    if (rc != SQLITE_OK) {
        // Si l'exécution échoue, mettre l'erreur dans le QueryResult
        result.error = "Erreur : Échec de l'exécution du fichier DDL. " + std::string(errMsg);
        std::cout << result.error << std::endl;
        sqlite3_free(errMsg); // Libérer le message d'erreur
        return result; // Retourner avec l'erreur
    }
    result.error = SUCCESS; // Succès de la création des tables
    return result; 
}


int DataBase::callback(void *data, int argc, char **argv, char **az_col_name){
    QueryResult *query_result = static_cast<QueryResult*>(data);
    std::vector<std::string> row;

    (void) az_col_name;
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? argv[i] : "NULL");
    }
    query_result->data.push_back(row);
    return 0;
}


QueryResult DataBase::executeQuery(const std::string& sql_query) {
    QueryResult result;
    int rc = sqlite3_exec(this->db, sql_query.c_str(), callback, &result, NULL);
    if (rc != SQLITE_OK) {
        result.error = sqlite3_errmsg(this->db);
    }
    else{
        result.error = SUCCESS;
    }
    return result;
}



QueryResult DataBase::insertEntry(const std::string &table_name, const std::string& columns, const std::string& values){
    std::string sql = "INSERT INTO " + table_name + " (" + columns + ") VALUES (" + values + ");";
    return executeQuery(sql);
}


QueryResult DataBase::deleteEntry(const std::string &table_name, const std::string& condition) {
    std::string sql = "DELETE FROM " + table_name + " WHERE " + condition + ";";
    QueryResult result = executeQuery(sql);
    //int num_del_rows = sqlite3_changes(db);
    return executeQuery(sql);
}


QueryResult DataBase::updateEntry(const std::string& table_name, const std::string& columns, const std::string& condition) {
    std::string sql = "UPDATE " + table_name + " SET " + columns + " WHERE " + condition + ";";
    return executeQuery(sql);
}


QueryResult DataBase::selectFromTable(const std::string& table_name, const std::string& columns, const std::string& condition) {
    std::string sql = "SELECT " + columns + " FROM " + table_name;
    if (!condition.empty()) {
        sql += " WHERE " + condition;
    }
    return executeQuery(sql);
}


void DataBase::rollBack(){
    sqlite3_exec(this->db, "ROLLBACK", NULL, NULL, NULL);
}


void DataBase::closeConnection(){
        sqlite3_close(this->db);    
}

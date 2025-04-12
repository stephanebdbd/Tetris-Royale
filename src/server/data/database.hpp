#pragma once

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sqlite3.h>

#include "query_result.hpp"

class DataBase {
    protected:
        sqlite3 *db; // Pointeur vers la base de données SQLite
    
        QueryResult createDb();
        QueryResult createTables();
        static int callback(void *data, int argc, char **argv, char **az_col_name);
    
    public:
    DataBase() {
            createDb();
            createTables();
        }
    
        virtual ~DataBase() {
            closeConnection();
        }
    
        QueryResult executeQuery(const std::string& sql_query);
        QueryResult insertEntry(const std::string &table_name, const std::string& columns, const std::string& values);
        QueryResult deleteEntry(const std::string &table_name, const std::string& condition);
        QueryResult updateEntry(const std::string& table_name, const std::string& columns, const std::string& condition);
        QueryResult selectFromTable(const std::string& table_name, const std::string& columns, const std::string& condition);
    
        void rollBack();
        void closeConnection();
        
        std::string getTime();
};
/*
class DataBase
{
private:

    sqlite3 *db; // the pointer to the sqlite database 

    QueryResult createDb();

    QueryResult createTables();


    static int callback(void *data, int argc, char **argv, char **az_col_name);




public:

    DataBase(){
        createDb();
        createTables();
    };

    ~DataBase(){closeConnection();};
    QueryResult executeQuery(std::string& sql_query);


    QueryResult insertEntry(const std::string &table_name, const std::string& columns, const std::string& values);


    QueryResult deleteEntry(const std::string &table_name, const std::string& condition);

    QueryResult updateEntry(const std::string& table_name, const std::string& columns, const std::string& condition);

    QueryResult selectFromTable(const std::string& table_name, const std::string& columns, const std::string& condition);


    void rollBack();

    void closeConnection();
};
*/
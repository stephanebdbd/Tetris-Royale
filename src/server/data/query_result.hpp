#pragma once

#include <sqlite3.h>
#include <vector>
#include <string>

const std::string SUCCESS = "SUCCESS";

struct QueryResult {
    QueryResult();
    std::vector<std::vector<std::string>> data;
    std::string error;
    bool isOk() const;
    std::string getFirst() const;
    std::vector<std::string> getVector(int columnIndex) const;
    std::string getError() const;
    std::vector<std::vector<std::string>> getData() const;
};

std::string humanReadableError(const std::string& rawError);


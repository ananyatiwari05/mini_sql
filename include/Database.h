#ifndef DATABASE_H
#define DATABASE_H

#include "Table.h"
#include "Parser.h"
#include <string>
#include <unordered_map>
#include <memory>

// Main Database class
class Database {
private:
    std::unordered_map<std::string, std::shared_ptr<Table>> tables;
    std::string dataDirectory;

    // Helper methods
    void loadTablesFromDisk();
    std::string getTableFilePath(const std::string& tableName) const;

public:
    Database(const std::string& dataDir = "data");

    // Database operations
    bool createTable(const std::string& tableName, const std::vector<std::string>& columns);
    bool insert(const std::string& tableName, const std::vector<std::string>& values);
    std::vector<Record> select(const std::string& tableName, const std::vector<std::string>& columns,
                               const Condition* condition = nullptr);
    bool deleteRecords(const std::string& tableName, const Condition& condition);

    // Query execution
    std::string executeQuery(const std::string& query);

    // Utility methods
    bool tableExists(const std::string& tableName) const;
    const std::vector<std::string>& getTableColumns(const std::string& tableName) const;
};

#endif // DATABASE_H

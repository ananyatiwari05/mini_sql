#ifndef DATABASE_H
#define DATABASE_H

#include "Table.h"
#include "Parser.h"
#include <string>
#include <unordered_map>
#include <memory>
using namespace std;
// Main Database class
class Database {
private:
    unordered_map<string, shared_ptr<Table>> tables;
    string dataDirectory;

    // Helper methods
    void loadTablesFromDisk();
    string getTableFilePath(const string& tableName) const;
public:
    Database(const string& dataDir = "data");

    // Database operations
    bool createTable(const string& tableName, const vector<string>& columns);
    bool dropTable(const string& tableName); // Added missing dropTable declaration
    bool insert(const string& tableName, const vector<string>& values);
    vector<Record> select(const string& tableName, const vector<string>& columns,
                               const Condition* condition = nullptr);
    bool deleteRecords(const string& tableName, const Condition& condition);
    bool updateRecords(const string& tableName, const vector<UpdateAssignment>& assignments, const Condition& condition); // Added missing updateRecords declaration

    // Query execution
    string executeQuery(const string& query);
    // Utility methods
    bool tableExists(const string& tableName) const;
    const vector<string>& getTableColumns(const string& tableName) const;
};

#endif // DATABASE_H

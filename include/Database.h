#ifndef DATABASE_H
#define DATABASE_H

#include "Table.h"
#include "Parser.h"
#include <string>
#include <unordered_map>
#include <memory>
using namespace std;

class Database {
private:
    unordered_map<string, unordered_map<string, shared_ptr<Table>>> databases;
    string currentDatabase;
    string baseDirectory;

    void loadDatabasesFromDisk();
    string getDatabaseDirectory(const string& dbName) const;
    string getTableFilePath(const string& dbName, const string& tableName) const;

public:
    Database(const string& baseDir = "databases");

    bool createDatabase(const string& databaseName);
    bool useDatabase(const string& databaseName);
    string getCurrentDatabase() const;

    bool createTable(const string& tableName, const vector<string>& columns);
    bool dropTable(const string& tableName);
    bool insert(const string& tableName, const vector<string>& values);
    bool updateRecords(const string& tableName, const vector<pair<string, string>>& updates, const Condition& condition);
    bool alterTable(const string& tableName, const string& action, const string& columnName, const string& columnType);
    
    vector<Record> select(const string& tableName, const vector<string>& columns,
                               const Condition* condition = nullptr);
    bool deleteRecords(const string& tableName, const Condition& condition);

    string executeQuery(const string& query);

    bool tableExists(const string& tableName) const;
    const vector<string>& getTableColumns(const string& tableName) const;
};

#endif // DATABASE_H

#ifndef TABLE_H
#define TABLE_H

#include "Record.h"
#include "Utils.h"
#include <string>
#include <vector>
using namespace std;
// Condition for WHERE clause
struct Condition {
    string columnName;
    string op;  // "=", "==", ">", "<", ">=", "<=", "!="
    string value;
};

// Represents a database table
class Table {
private:
    string tableName;
    vector<string> columns;
    vector<Record> rows;

    // Helper to get column index
    int getColumnIndex(const string& columnName) const;

    // Helper to evaluate condition for a record
    bool evaluateCondition(const Record& record, const Condition& condition) const;

    // Helper to convert value to number if possible
    double toNumber(const string& val) const;

public:
    Table(const string& name, const vector<string>& cols);

    // Basic operations
    const string& getTableName() const;
    const vector<string>& getColumns() const;
    const vector<Record>& getRows() const;

    // Data manipulation
    void insertRow(const Record& record);
    void deleteWhere(const Condition& condition);
    vector<Record> selectWhere(const Condition& condition) const;
    vector<Record> selectAll() const;

    // File operations
    bool saveToFile(const string& filename) const;
    static Table* loadFromFile(const string& filename);
};

#endif // TABLE_H

#ifndef TABLE_H
#define TABLE_H

#include "Record.h"
#include "Utils.h"
#include <string>
#include <vector>

// Condition for WHERE clause
struct Condition {
    std::string columnName;
    std::string op;  // "=", "==", ">", "<", ">=", "<=", "!="
    std::string value;
};

// Represents a database table
class Table {
private:
    std::string tableName;
    std::vector<std::string> columns;
    std::vector<Record> rows;

    // Helper to get column index
    int getColumnIndex(const std::string& columnName) const;

    // Helper to evaluate condition for a record
    bool evaluateCondition(const Record& record, const Condition& condition) const;

    // Helper to convert value to number if possible
    double toNumber(const std::string& val) const;

public:
    Table(const std::string& name, const std::vector<std::string>& cols);

    // Basic operations
    const std::string& getTableName() const;
    const std::vector<std::string>& getColumns() const;
    const std::vector<Record>& getRows() const;

    // Data manipulation
    void insertRow(const Record& record);
    void deleteWhere(const Condition& condition);
    std::vector<Record> selectWhere(const Condition& condition) const;
    std::vector<Record> selectAll() const;

    // File operations
    bool saveToFile(const std::string& filename) const;
    static Table* loadFromFile(const std::string& filename);
};

#endif // TABLE_H

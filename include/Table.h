#ifndef TABLE_H
#define TABLE_H

#include "Record.h"
#include "AVLTree.h"
#include "BPlusTree.h"
#include <vector>
#include <string>
#include <map>

struct Condition {
    std::string columnName;
    std::string op;
    std::string value;
};

struct UpdateAssignment {
    std::string column;
    std::string value;
};

class Table {
private:
    std::string tableName;
    std::vector<std::string> columns;
    std::vector<Record> rows;

    // AVL Tree for exact match lookups on any column
    std::map<std::string, AVLTree<std::string, size_t>> avlIndices; // column -> (value -> row index)
    
    // B+ Tree for range queries on numeric columns
    std::map<std::string, BPlusTree<double, size_t>> bplusIndices; // column -> (numeric value -> row index)

    double toNumber(const std::string& val) const;
    bool evaluateCondition(const Record& record, const Condition& condition) const;
    void buildIndices(); // Rebuild all indices after data changes

public:
    Table(const std::string& name, const std::vector<std::string>& cols);

    const std::string& getTableName() const;
    const std::vector<std::string>& getColumns() const;
    const std::vector<Record>& getRows() const;
    std::vector<Record>& getRows();

    int getColumnIndex(const std::string& columnName) const;

    void insertRow(const Record& record);
    void deleteWhere(const Condition& condition);

    std::vector<Record> selectWhereExactMatch(const std::string& columnName, const std::string& value) const;
    
    std::vector<Record> selectWhereRange(const std::string& columnName, const std::string& op, double value) const;
    
    std::vector<Record> selectWhere(const Condition& condition) const;
    std::vector<Record> selectAll() const;

    bool saveToFile(const std::string& filename) const;
    static Table* loadFromFile(const std::string& filename);
};

#endif // TABLE_H

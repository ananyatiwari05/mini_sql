#ifndef TABLE_H
#define TABLE_H

#include "Record.h"
#include "Utils.h"
#include "BPlusTree.h"
#include <string>
#include <vector>
#include <memory>
using namespace std;

struct Condition {
    string columnName;
    string op;
    string value;
};

class Table {
private:
    string tableName;
    vector<string> columns;
    vector<Record> rows;
    unique_ptr<BPlusTree> primaryIndex;
    string primaryKeyColumn;

    int getColumnIndex(const string& columnName) const;
    double toNumber(const string& val) const;

public:
    Table(const string& name, 
          const vector<string>& cols, 
          const string& primaryKey = "");

    // Getters
    const string& getTableName() const;
    const vector<string>& getColumns() const;

    const vector<Record>& getRows() const;  // const getter
    vector<Record>& getRows();              // non-const getter

    // Operations
    void insertRow(const Record& record);
    void deleteWhere(const Condition& condition);
    vector<Record> selectWhere(const Condition& condition) const;
    vector<Record> selectAll() const;

    vector<Record> selectOrderBy(const string& columnName, 
                                      bool descending = false) const;

    vector<Record> selectGroupBy(const string& columnName) const;

    bool evaluateCondition(const Record& record, const Condition& condition) const;

    bool saveToFile(const string& filename) const;
    static Table* loadFromFile(const string& filename);
};

#endif // TABLE_H

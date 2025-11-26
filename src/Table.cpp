// src/Table.cpp
#include "Table.h"
#include "FileManager.h"
#include "Utils.h"
#include "AVLTree.h"
#include <iostream>
#include <algorithm>
#include <memory>   // for make_unique (optional but explicit)
using namespace std;

Table::Table(const string& name, const vector<string>& cols, const string& primaryKey)
    : tableName(name), columns(cols), primaryKeyColumn(primaryKey) {
    // create primary index (B+ tree) even if no primaryKey specified;
    // the BPlusTree implementation can ignore inserts if primaryKey is empty.
    primaryIndex = make_unique<BPlusTree>();
}

const string& Table::getTableName() const {
    return tableName;
}

const vector<string>& Table::getColumns() const {
    return columns;
}

// const getter (declared in header)
const vector<Record>& Table::getRows() const {
    return rows;
}

// non-const getter (implementation was missing and caused linker errors)
vector<Record>& Table::getRows() {
    return rows;
}

int Table::getColumnIndex(const string& columnName) const {
    for (size_t i = 0; i < columns.size(); ++i) {
        if (Utils::toLower(columns[i]) == Utils::toLower(columnName)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

double Table::toNumber(const string& val) const {
    try {
        return stod(val);
    } catch (...) {
        return 0.0;
    }
}

bool Table::evaluateCondition(const Record& record, const Condition& condition) const {
    int colIndex = getColumnIndex(condition.columnName);
    if (colIndex < 0 || colIndex >= static_cast<int>(record.getSize())) {
        return false;
    }

    string recordValue = record.getValue(colIndex);
    string conditionValue = condition.value;

    if (condition.op == "=" || condition.op == "==") {
        return recordValue == conditionValue;
    } else if (condition.op == "!=") {
        return recordValue != conditionValue;
    } else if (condition.op == ">") {
        return toNumber(recordValue) > toNumber(conditionValue);
    } else if (condition.op == "<") {
        return toNumber(recordValue) < toNumber(conditionValue);
    } else if (condition.op == ">=") {
        return toNumber(recordValue) >= toNumber(conditionValue);
    } else if (condition.op == "<=") {
        return toNumber(recordValue) <= toNumber(conditionValue);
    }
    return false;
}

void Table::insertRow(const Record& record) {
    rows.push_back(record);
    if (!primaryKeyColumn.empty() && record.getSize() > 0) {
        int pkIndex = getColumnIndex(primaryKeyColumn);
        if (pkIndex >= 0) {
            // store pointer/row index in B+ tree leaf
            primaryIndex->insert(record.getValue(pkIndex), static_cast<int>(rows.size() - 1));
        }
    }
}

void Table::deleteWhere(const Condition& condition) {
    // If you want the primaryIndex to remain consistent, you'd need to rebuild it
    // after deletion (or update it on each deletion). For now, do a simple erase-by-condition.
    rows.erase(
        remove_if(rows.begin(), rows.end(),
                       [this, &condition](const Record& r) {
                           return evaluateCondition(r, condition);
                       }),
        rows.end()
    );

    // NOTE: primaryIndex is not updated here. If you rely on the index after deletes,
    // you should rebuild it (iterate rows and re-insert keys) or mark deleted entries in data pages.
}

vector<Record> Table::selectWhere(const Condition& condition) const {
    vector<Record> result;
    for (const auto& record : rows) {
        if (evaluateCondition(record, condition)) {
            result.push_back(record);
        }
    }
    return result;
}

vector<Record> Table::selectAll() const {
    return rows;
}

vector<Record> Table::selectOrderBy(const string& columnName, bool descending) const {
    int colIndex = getColumnIndex(columnName);
    if (colIndex < 0) return rows;

    AVLTree<string> avlTree;
    vector<Record> result;

    // Insert row indices keyed by the column value. If multiple rows share same key,
    // the AVL implementation should handle duplicate keys (e.g., store a list).
    for (size_t i = 0; i < rows.size(); ++i) {
        avlTree.insert(rows[i].getValue(colIndex), to_string(static_cast<int>(i)));
    }

    auto sortedIndices = avlTree.getInOrder();
    for (const auto& indexStr : sortedIndices) {
        try {
            int idx = stoi(indexStr);
            if (idx >= 0 && static_cast<size_t>(idx) < rows.size()) {
                result.push_back(rows[idx]);
            }
        } catch (...) {
            // ignore parse errors
        }
    }

    if (descending) {
        reverse(result.begin(), result.end());
    }

    return result;
}

vector<Record> Table::selectGroupBy(const string& columnName) const {
    int colIndex = getColumnIndex(columnName);
    if (colIndex < 0) return {};

    AVLTree<string> groupTree;
    vector<Record> result;

    // Build tree of unique keys
    for (const auto& record : rows) {
        string key = record.getValue(colIndex);
        groupTree.insert(key, key); // value not important here - we just want keys unique and sorted
    }

    auto uniqueKeys = groupTree.getInOrder();
    for (const auto& key : uniqueKeys) {
        Record groupRecord;
        groupRecord.addValue(key);
        int count = 0;
        for (const auto& record : rows) {
            if (record.getValue(colIndex) == key) count++;
        }
        groupRecord.addValue(to_string(count));
        result.push_back(groupRecord);
    }

    return result;
}

bool Table::saveToFile(const string& filename) const {
    vector<string> lines;
    lines.push_back(Utils::join(columns, ","));
    for (const auto& record : rows) {
        lines.push_back(record.toCSV());
    }
    return FileManager::writeLines(filename, lines);
}

Table* Table::loadFromFile(const string& filename) {
    if (!FileManager::fileExists(filename)) {
        return nullptr;
    }

    vector<string> lines = FileManager::readLines(filename);
    if (lines.empty()) {
        return nullptr;
    }

    string tableNameFromFile = filename;
    size_t lastSlash = tableNameFromFile.find_last_of("/\\");
    if (lastSlash != string::npos) {
        tableNameFromFile = tableNameFromFile.substr(lastSlash + 1);
    }
    size_t dotPos = tableNameFromFile.find_last_of(".");
    if (dotPos != string::npos) {
        tableNameFromFile = tableNameFromFile.substr(0, dotPos);
    }

    vector<string> cols = Utils::split(lines[0], ',');
    Table* table = new Table(tableNameFromFile, cols);

    for (size_t i = 1; i < lines.size(); ++i) {
        if (!lines[i].empty()) {
            Record record = Record::fromCSV(lines[i]);
            table->insertRow(record);
        }
    }

    return table;
}

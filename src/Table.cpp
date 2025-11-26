#include "Table.h"
#include "FileManager.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>

Table::Table(const std::string& name, const std::vector<std::string>& cols)
    : tableName(name), columns(cols) {}

const std::string& Table::getTableName() const {
    return tableName;
}

const std::vector<std::string>& Table::getColumns() const {
    return columns;
}

const std::vector<Record>& Table::getRows() const {
    return rows;
}

int Table::getColumnIndex(const std::string& columnName) const {
    for (size_t i = 0; i < columns.size(); ++i) {
        if (Utils::toLower(columns[i]) == Utils::toLower(columnName)) {
            return i;
        }
    }
    return -1;
}

double Table::toNumber(const std::string& val) const {
    try {
        return std::stod(val);
    } catch (...) {
        return 0.0;
    }
}

void Table::buildIndices() {
    avlIndices.clear();
    bplusIndices.clear();

    for (size_t colIdx = 0; colIdx < columns.size(); ++colIdx) {
        const std::string& colName = columns[colIdx];
        avlIndices[colName] = AVLTree<std::string, size_t>();
        bplusIndices[colName] = BPlusTree<double, size_t>();

        for (size_t rowIdx = 0; rowIdx < rows.size(); ++rowIdx) {
            std::string value = rows[rowIdx].getValue(colIdx);
            // Add to AVL Tree for exact match
            avlIndices[colName].insert(value, rowIdx);
            // Add to B+ Tree for range queries
            double numValue = toNumber(value);
            bplusIndices[colName].insert(numValue, rowIdx);
        }
    }
}

bool Table::evaluateCondition(const Record& record, const Condition& condition) const {
    int colIndex = getColumnIndex(condition.columnName);
    if (colIndex < 0 || colIndex >= (int)record.getSize()) {
        return false;
    }

    std::string recordValue = record.getValue(colIndex);
    std::string conditionValue = condition.value;

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

std::vector<Record> Table::selectWhereExactMatch(const std::string& columnName, const std::string& value) const {
    std::vector<Record> result;

    auto avlIt = avlIndices.find(columnName);
    if (avlIt == avlIndices.end()) {
        return result;
    }

    size_t rowIdx;
    if (const_cast<AVLTree<std::string, size_t>&>(avlIt->second).find(value, rowIdx)) {
        result.push_back(rows[rowIdx]);
    }

    return result;
}

std::vector<Record> Table::selectWhereRange(const std::string& columnName, const std::string& op, double value) const {
    std::vector<Record> result;

    auto bplusIt = bplusIndices.find(columnName);
    if (bplusIt == bplusIndices.end()) {
        return result;
    }

    std::vector<size_t> indices;

    if (op == ">") {
        // Get all values greater than target
        indices = const_cast<BPlusTree<double, size_t>&>(bplusIt->second).rangeQuery(value + 0.0001, 1e9);
    } else if (op == "<") {
        // Get all values less than target
        indices = const_cast<BPlusTree<double, size_t>&>(bplusIt->second).rangeQuery(-1e9, value - 0.0001);
    } else if (op == ">=") {
        indices = const_cast<BPlusTree<double, size_t>&>(bplusIt->second).rangeQuery(value, 1e9);
    } else if (op == "<=") {
        indices = const_cast<BPlusTree<double, size_t>&>(bplusIt->second).rangeQuery(-1e9, value);
    }

    for (size_t idx : indices) {
        if (idx < rows.size()) {
            result.push_back(rows[idx]);
        }
    }

    return result;
}

std::vector<Record> Table::selectWhere(const Condition& condition) const {
    if (condition.op == "=" || condition.op == "==") {
        // Use AVL Tree for exact match - O(log n)
        return selectWhereExactMatch(condition.columnName, condition.value);
    } else if (condition.op == ">" || condition.op == "<" || 
               condition.op == ">=" || condition.op == "<=") {
        // Use B+ Tree for range queries - O(log n + k)
        return selectWhereRange(condition.columnName, condition.op, toNumber(condition.value));
    } else if (condition.op == "!=") {
        // Inequality: get all records and filter
        std::vector<Record> result;
        for (const auto& record : rows) {
            if (evaluateCondition(record, condition)) {
                result.push_back(record);
            }
        }
        return result;
    }

    return std::vector<Record>();
}

std::vector<Record> Table::selectAll() const {
    return rows;
}

void Table::insertRow(const Record& record) {
    rows.push_back(record);
    size_t newRowIdx = rows.size() - 1;
    
    for (size_t colIdx = 0; colIdx < columns.size() && colIdx < record.getSize(); ++colIdx) {
        std::string colName = columns[colIdx];
        std::string value = record.getValue(colIdx);

        if (avlIndices.find(colName) == avlIndices.end()) {
            avlIndices[colName] = AVLTree<std::string, size_t>();
        }
        if (bplusIndices.find(colName) == bplusIndices.end()) {
            bplusIndices[colName] = BPlusTree<double, size_t>();
        }

        avlIndices[colName].insert(value, newRowIdx);
        bplusIndices[colName].insert(toNumber(value), newRowIdx);
    }
}

void Table::deleteWhere(const Condition& condition) {
    rows.erase(
        std::remove_if(rows.begin(), rows.end(),
                      [this, &condition](const Record& r) {
                          return evaluateCondition(r, condition);
                      }),
        rows.end()
    );
    buildIndices();
}

bool Table::saveToFile(const std::string& filename) const {
    std::vector<std::string> lines;

    // First line: column names
    lines.push_back(Utils::join(columns, ","));

    // Remaining lines: data rows
    for (const auto& record : rows) {
        lines.push_back(record.toCSV());
    }

    return FileManager::writeLines(filename, lines);
}

Table* Table::loadFromFile(const std::string& filename) {
    if (!FileManager::fileExists(filename)) {
        return nullptr;
    }

    std::vector<std::string> lines = FileManager::readLines(filename);
    if (lines.empty()) {
        return nullptr;
    }

    // Extract table name from filename
    std::string tableNameFromFile = filename;
    size_t lastSlash = tableNameFromFile.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        tableNameFromFile = tableNameFromFile.substr(lastSlash + 1);
    }
    size_t dotPos = tableNameFromFile.find_last_of(".");
    if (dotPos != std::string::npos) {
        tableNameFromFile = tableNameFromFile.substr(0, dotPos);
    }

    // Parse columns from first line
    std::vector<std::string> columns = Utils::split(lines[0], ',');
    Table* table = new Table(tableNameFromFile, columns);

    // Parse data rows
    for (size_t i = 1; i < lines.size(); ++i) {
        if (!lines[i].empty()) {
            Record record = Record::fromCSV(lines[i]);
            table->insertRow(record);
        }
    }

    table->buildIndices();

    return table;
}

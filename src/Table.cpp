#include "Table.h"
#include "FileManager.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>
using namespace std;
Table::Table(const string& name, const vector<string>& cols)
    : tableName(name), columns(cols) {}

const string& Table::getTableName() const {
    return tableName;
}

const vector<string>& Table::getColumns() const {
    return columns;
}

const vector<Record>& Table::getRows() const {
    return rows;
}

int Table::getColumnIndex(const string& columnName) const {
    for (size_t i = 0; i < columns.size(); ++i) {
        if (Utils::toLower(columns[i]) == Utils::toLower(columnName)) {
            return i;
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
    if (colIndex < 0 || colIndex >= (int)record.getSize()) {
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
}

void Table::deleteWhere(const Condition& condition) {
    rows.erase(
        remove_if(rows.begin(), rows.end(),
                      [this, &condition](const Record& r) {
                          return evaluateCondition(r, condition);
                      }),
        rows.end()
    );
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

bool Table::saveToFile(const string& filename) const {
    vector<string> lines;

    // First line: column names
    lines.push_back(Utils::join(columns, ","));

    // Remaining lines: data rows
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

    // Extract table name from filename
    string tableNameFromFile = filename;
    size_t lastSlash = tableNameFromFile.find_last_of("/\\");
    if (lastSlash != string::npos) {
        tableNameFromFile = tableNameFromFile.substr(lastSlash + 1);
    }
    size_t dotPos = tableNameFromFile.find_last_of(".");
    if (dotPos != string::npos) {
        tableNameFromFile = tableNameFromFile.substr(0, dotPos);
    }

    // Parse columns from first line
    vector<string> columns = Utils::split(lines[0], ',');
    Table* table = new Table(tableNameFromFile, columns);

    // Parse data rows
    for (size_t i = 1; i < lines.size(); ++i) {
        if (!lines[i].empty()) {
            Record record = Record::fromCSV(lines[i]);
            table->insertRow(record);
        }
    }

    return table;
}

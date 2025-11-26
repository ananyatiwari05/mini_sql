#include "Database.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "FileManager.h"
#include "Utils.h"
#include "Colors.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

Database::Database(const string& baseDir)
    : baseDirectory(baseDir), currentDatabase("") {
    FileManager::createDirectory(baseDirectory);
    loadDatabasesFromDisk();
}

void Database::loadDatabasesFromDisk() {
    vector<string> dbDirs = FileManager::listFilesInDirectory(baseDirectory);
    for (const auto& dbDir : dbDirs) {
        string dbName = dbDir.substr(dbDir.find_last_of("/\\") + 1);
        databases[dbName] = unordered_map<string, shared_ptr<Table>>();
        
        vector<string> files = FileManager::listFilesInDirectory(dbDir);
        for (const auto& filepath : files) {
            Table* table = Table::loadFromFile(filepath);
            if (table) {
                databases[dbName][table->getTableName()] = shared_ptr<Table>(table);
            }
        }
    }
}

string Database::getDatabaseDirectory(const string& dbName) const {
    return baseDirectory + "/" + dbName;
}

string Database::getTableFilePath(const string& dbName, const string& tableName) const {
    return getDatabaseDirectory(dbName) + "/" + tableName + ".tbl";
}

bool Database::createDatabase(const string& databaseName) {
    if (databases.find(databaseName) != databases.end()) {
        return false;
    }
    
    databases[databaseName] = unordered_map<string, shared_ptr<Table>>();
    FileManager::createDirectory(getDatabaseDirectory(databaseName));
    return true;
}

bool Database::useDatabase(const string& databaseName) {
    if (databases.find(databaseName) == databases.end()) {
        return false;
    }
    currentDatabase = databaseName;
    return true;
}

string Database::getCurrentDatabase() const {
    return currentDatabase;
}

bool Database::createTable(const string& tableName, const vector<string>& columns) {
    if (currentDatabase.empty()) {
        return false;
    }
    
    auto& dbTables = databases[currentDatabase];
    if (dbTables.find(tableName) != dbTables.end()) {
        return false;
    }

    shared_ptr<Table> table(new Table(tableName, columns));
    dbTables[tableName] = table;
    return table->saveToFile(getTableFilePath(currentDatabase, tableName));
}

bool Database::dropTable(const string& tableName) {
    if (currentDatabase.empty() || databases[currentDatabase].find(tableName) == databases[currentDatabase].end()) {
        return false;
    }

    databases[currentDatabase].erase(tableName);
    string filePath = getTableFilePath(currentDatabase, tableName);
    return remove(filePath.c_str()) == 0;
}

bool Database::insert(const string& tableName, const vector<string>& values) {
    if (currentDatabase.empty() || databases[currentDatabase].find(tableName) == databases[currentDatabase].end()) {
        return false;
    }

    shared_ptr<Table> table = databases[currentDatabase][tableName];
    if (values.size() != table->getColumns().size()) {
        return false;
    }

    Record record(values);
    table->insertRow(record);
    return table->saveToFile(getTableFilePath(currentDatabase, tableName));
}

bool Database::updateRecords(const string& tableName, const vector<pair<string, string>>& updates, const Condition& condition) {
    if (currentDatabase.empty() || databases[currentDatabase].find(tableName) == databases[currentDatabase].end()) {
        return false;
    }

    shared_ptr<Table> table = databases[currentDatabase][tableName];
    const auto& columns = table->getColumns();
    auto& rows = const_cast<vector<Record>&>(table->getRows());

    for (auto& record : rows) {
        if (table->evaluateCondition(record, condition)) {
            for (const auto& update : updates) {
                int colIdx = -1;
                for (size_t i = 0; i < columns.size(); ++i) {
                    if (Utils::toLower(columns[i]) == Utils::toLower(update.first)) {
                        colIdx = i;
                        break;
                    }
                }
                if (colIdx >= 0) {
                    record.setValue(colIdx, update.second);
                }
            }
        }
    }

    return table->saveToFile(getTableFilePath(currentDatabase, tableName));
}

bool Database::alterTable(const string& tableName, const string& action, const string& columnName, const string& columnType) {
    if (currentDatabase.empty() || databases[currentDatabase].find(tableName) == databases[currentDatabase].end()) {
        return false;
    }

    shared_ptr<Table> table = databases[currentDatabase][tableName];
    auto columns = table->getColumns();

    if (action == "ADD") {
        columns.push_back(columnName);
    } else if (action == "DROP") {
        auto it = find(columns.begin(), columns.end(), columnName);
        if (it != columns.end()) {
            columns.erase(it);
        }
    }

    // Recreate table with new columns
    shared_ptr<Table> newTable(new Table(tableName, columns));
    for (const auto& record : table->getRows()) {
        newTable->insertRow(record);
    }

    databases[currentDatabase][tableName] = newTable;
    return newTable->saveToFile(getTableFilePath(currentDatabase, tableName));
}

string Database::executeQuery(const string& query) {
    string trimmedQuery = Utils::trim(query);
    if (trimmedQuery.empty()) {
        return Colors::error("Error: Empty query");
    }

    Tokenizer tokenizer(trimmedQuery);
    vector<Token> tokens = tokenizer.tokenize();

    Parser parser(tokens);
    ParsedQuery parsedQuery = parser.parse();

    stringstream result;

    switch (parsedQuery.type) {
        case ParsedQuery::QueryType::CREATE_DATABASE: {
            if (createDatabase(parsedQuery.databaseName)) {
                result << Colors::BRIGHT_GREEN << "[✓]" << Colors::RESET << " Database '"
                       << Colors::BRIGHT_YELLOW << parsedQuery.databaseName << Colors::RESET
                       << "' created successfully.";
            } else {
                result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: Database '"
                       << Colors::BRIGHT_RED << parsedQuery.databaseName << Colors::RESET << "' already exists.";
            }
            break;
        }

        case ParsedQuery::QueryType::USE_DATABASE: {
            if (useDatabase(parsedQuery.databaseName)) {
                result << Colors::BRIGHT_GREEN << "[✓]" << Colors::RESET << " Using database '"
                       << Colors::BRIGHT_YELLOW << parsedQuery.databaseName << Colors::RESET << "'.";
            } else {
                result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: Database '"
                       << Colors::BRIGHT_RED << parsedQuery.databaseName << Colors::RESET << "' does not exist.";
            }
            break;
        }

        case ParsedQuery::QueryType::CREATE_TABLE: {
            if (createTable(parsedQuery.tableName, parsedQuery.columns)) {
                result << Colors::BRIGHT_GREEN << "[✓]" << Colors::RESET << " Table '"
                       << Colors::BRIGHT_YELLOW << parsedQuery.tableName << Colors::RESET
                       << "' created successfully.";
            } else {
                result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: Could not create table '"
                       << Colors::BRIGHT_RED << parsedQuery.tableName << Colors::RESET << "'.";
            }
            break;
        }

        case ParsedQuery::QueryType::DROP_TABLE: {
            if (dropTable(parsedQuery.tableName)) {
                result << Colors::BRIGHT_GREEN << "[✓]" << Colors::RESET << " Table '"
                       << Colors::BRIGHT_YELLOW << parsedQuery.tableName << Colors::RESET
                       << "' dropped successfully.";
            } else {
                result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: Could not drop table '"
                       << Colors::BRIGHT_RED << parsedQuery.tableName << Colors::RESET << "'.";
            }
            break;
        }

        case ParsedQuery::QueryType::INSERT: {
            if (insert(parsedQuery.tableName, parsedQuery.values)) {
                result << Colors::BRIGHT_GREEN << "[✓]" << Colors::RESET << " Record inserted successfully.";
            } else {
                result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: Could not insert record into '"
                       << Colors::BRIGHT_RED << parsedQuery.tableName << Colors::RESET << "'.";
            }
            break;
        }

        case ParsedQuery::QueryType::UPDATE: {
            if (!parsedQuery.conditions.empty()) {
                if (updateRecords(parsedQuery.tableName, parsedQuery.updateValues, parsedQuery.conditions[0])) {
                    result << Colors::BRIGHT_GREEN << "[✓]" << Colors::RESET << " Records updated successfully.";
                } else {
                    result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: Could not update records.";
                }
            } else {
                result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: UPDATE requires a WHERE clause.";
            }
            break;
        }

        case ParsedQuery::QueryType::ALTER_TABLE: {
            if (alterTable(parsedQuery.tableName, parsedQuery.alterAction, parsedQuery.alterColumnName, parsedQuery.alterColumnType)) {
                result << Colors::BRIGHT_GREEN << "[✓]" << Colors::RESET << " Table '"
                       << Colors::BRIGHT_YELLOW << parsedQuery.tableName << Colors::RESET
                       << "' altered successfully.";
            } else {
                result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: Could not alter table '"
                       << Colors::BRIGHT_RED << parsedQuery.tableName << Colors::RESET << "'.";
            }
            break;
        }

        case ParsedQuery::QueryType::SELECT: {
            if (currentDatabase.empty()) {
                result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: No database selected.";
                break;
            }
            
            vector<Record> records;
            if (parsedQuery.selectAll) {
                parsedQuery.columns.push_back("*");
            }

            Condition* condition = parsedQuery.conditions.empty() ? nullptr : &parsedQuery.conditions[0];
            records = select(parsedQuery.tableName, parsedQuery.columns, condition);

            if (!parsedQuery.orderByColumn.empty()) {
                if (databases[currentDatabase].find(parsedQuery.tableName) != databases[currentDatabase].end()) {
                    records = databases[currentDatabase][parsedQuery.tableName]->selectOrderBy(parsedQuery.orderByColumn, parsedQuery.orderByDesc);
                    
                    if (condition) {
                        vector<Record> filtered;
                        for (const auto& rec : records) {
                            if (databases[currentDatabase][parsedQuery.tableName]->evaluateCondition(rec, *condition)) {
                                filtered.push_back(rec);
                            }
                        }
                        records = filtered;
                    }
                }
            }

            if (!parsedQuery.groupByColumn.empty()) {
                if (databases[currentDatabase].find(parsedQuery.tableName) != databases[currentDatabase].end()) {
                    records = databases[currentDatabase][parsedQuery.tableName]->selectGroupBy(parsedQuery.groupByColumn);
                }
            }

            if (!tableExists(parsedQuery.tableName)) {
                result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: Table '"
                       << Colors::BRIGHT_RED << parsedQuery.tableName << Colors::RESET << "' does not exist.";
            } else {
                const auto& columns = parsedQuery.selectAll ? getTableColumns(parsedQuery.tableName) : parsedQuery.columns;
                
                int colWidth = 18;
                int totalWidth = (colWidth * columns.size()) + (columns.size() - 1) + 4;
                
                result << "\n" << Colors::BRIGHT_CYAN << "+" << string(totalWidth - 2, '-') << "+" << Colors::RESET << "\n";
                
                result << Colors::BRIGHT_CYAN << "| " << Colors::RESET;
                for (size_t i = 0; i < columns.size(); ++i) {
                    result << Colors::BRIGHT_YELLOW << left << setw(colWidth - 1) << columns[i] << Colors::RESET;
                    if (i < columns.size() - 1) {
                        result << Colors::BRIGHT_CYAN << " | " << Colors::RESET;
                    }
                }
                result << " " << Colors::BRIGHT_CYAN << "|" << Colors::RESET << "\n";
                
                result << Colors::BRIGHT_CYAN << "+" << string(totalWidth - 2, '-') << "+" << Colors::RESET << "\n";

                if (records.empty()) {
                    result << Colors::BRIGHT_CYAN << "| " << Colors::RESET
                           << Colors::dim("(No records found)")
                           << string(totalWidth - 20, ' ')
                           << Colors::BRIGHT_CYAN << "|" << Colors::RESET << "\n";
                } else {
                    for (const auto& record : records) {
                        result << Colors::BRIGHT_CYAN << "| " << Colors::RESET;
                        for (size_t i = 0; i < record.getSize(); ++i) {
                            string value = record.getValue(i);
                            if (value.length() > colWidth - 1) {
                                value = value.substr(0, colWidth - 4) + "...";
                            }
                            result << Colors::CYAN << left << setw(colWidth - 1) << value << Colors::RESET;
                            if (i < record.getSize() - 1) {
                                result << Colors::BRIGHT_CYAN << " | " << Colors::RESET;
                            }
                        }
                        result << " " << Colors::BRIGHT_CYAN << "|" << Colors::RESET << "\n";
                    }
                }
                
                result << Colors::BRIGHT_CYAN << "+" << string(totalWidth - 2, '-') << "+" << Colors::RESET;
                result << "\n" << Colors::dim("Total rows: " + to_string(records.size()));
            }
            break;
        }

        case ParsedQuery::QueryType::DELETE: {
            if (!parsedQuery.conditions.empty()) {
                if (deleteRecords(parsedQuery.tableName, parsedQuery.conditions[0])) {
                    result << Colors::BRIGHT_GREEN << "[✓]" << Colors::RESET << " Records deleted successfully.";
                } else {
                    result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: Could not delete records.";
                }
            } else {
                result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: DELETE requires a WHERE clause.";
            }
            break;
        }

        default:
            result << Colors::BRIGHT_RED << "[✗]" << Colors::RESET << " Error: Invalid query.";
            break;
    }

    return result.str();
}

bool Database::tableExists(const string& tableName) const {
    if (currentDatabase.empty()) return false;
    auto it = databases.find(currentDatabase);
    if (it == databases.end()) return false;
    return it->second.find(tableName) != it->second.end();
}

const vector<string>& Database::getTableColumns(const string& tableName) const {
    static vector<string> empty;
    if (currentDatabase.empty()) return empty;
    auto it = databases.find(currentDatabase);
    if (it == databases.end()) return empty;
    auto tableIt = it->second.find(tableName);
    if (tableIt != it->second.end()) {
        return tableIt->second->getColumns();
    }
    return empty;
}

vector<Record> Database::select(const string& tableName, const vector<string>& columns,
                                     const Condition* condition) {
    vector<Record> result;

    if (currentDatabase.empty() || databases[currentDatabase].find(tableName) == databases[currentDatabase].end()) {
        return result;
    }

    shared_ptr<Table> table = databases[currentDatabase][tableName];

    vector<Record> records;
    if (condition) {
        records = table->selectWhere(*condition);
    } else {
        records = table->selectAll();
    }

    if (!columns.empty() && columns[0] != "*") {
        vector<int> columnIndices;
        for (const auto& colName : columns) {
            int idx = -1;
            const auto& tableColumns = table->getColumns();
            for (size_t i = 0; i < tableColumns.size(); ++i) {
                if (Utils::toLower(tableColumns[i]) == Utils::toLower(colName)) {
                    idx = i;
                    break;
                }
            }
            if (idx >= 0) {
                columnIndices.push_back(idx);
            }
        }

        for (const auto& record : records) {
            Record filteredRecord;
            for (int idx : columnIndices) {
                filteredRecord.addValue(record.getValue(idx));
            }
            result.push_back(filteredRecord);
        }
    } else {
        result = records;
    }

    return result;
}

bool Database::deleteRecords(const string& tableName, const Condition& condition) {
    if (currentDatabase.empty() || databases[currentDatabase].find(tableName) == databases[currentDatabase].end()) {
        return false;
    }

    shared_ptr<Table> table = databases[currentDatabase][tableName];
    table->deleteWhere(condition);
    return table->saveToFile(getTableFilePath(currentDatabase, tableName));
}

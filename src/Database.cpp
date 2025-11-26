#include "Database.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "FileManager.h"
#include "Utils.h"
#include "Colors.h"
#include <iostream>
#include <sstream>
using namespace std;        
Database::Database(const string& dataDir)
    : dataDirectory(dataDir) {
    FileManager::createDirectory(dataDirectory);
    loadTablesFromDisk();
}

void Database::loadTablesFromDisk() {
    vector<string> files = FileManager::listFilesInDirectory(dataDirectory);
    for (const auto& filepath : files) {
        Table* table = Table::loadFromFile(filepath);
        if (table) {
            tables[table->getTableName()] = shared_ptr<Table>(table);
        }
    }
}

string Database::getTableFilePath(const string& tableName) const {
    return dataDirectory + "/" + tableName + ".tbl";
}

bool Database::createTable(const string& tableName, const vector<string>& columns) {
    if (tables.find(tableName) != tables.end()) {
        return false; // Table already exists
    }

    shared_ptr<Table> table(new Table(tableName, columns));
    tables[tableName] = table;

    // Save to file
    return table->saveToFile(getTableFilePath(tableName));
}

bool Database::dropTable(const string& tableName) {
    if (tables.find(tableName) == tables.end()) {
        return false; // Table doesn't exist
    }

    tables.erase(tableName);
    
    // Delete the file
    string filepath = getTableFilePath(tableName);
    return remove(filepath.c_str()) == 0;
}

bool Database::insert(const string& tableName, const vector<string>& values) {
    if (tables.find(tableName) == tables.end()) {
        return false; // Table doesn't exist
    }

    shared_ptr<Table> table = tables[tableName];
    if (values.size() != table->getColumns().size()) {
        return false; // Column count mismatch
    }

    Record record(values);
    table->insertRow(record);

    // Save to file
    return table->saveToFile(getTableFilePath(tableName));
}

vector<Record> Database::select(const string& tableName, const vector<string>& columns,
                                     const Condition* condition) {
    vector<Record> result;

    if (tables.find(tableName) == tables.end()) {
        return result; // Table doesn't exist
    }

    shared_ptr<Table> table = tables[tableName];

    // Get records
    vector<Record> records;
    if (condition) {
        records = table->selectWhere(*condition);
    } else {
        records = table->selectAll();
    }

    // Filter columns if not selecting all
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
    if (tables.find(tableName) == tables.end()) {
        return false; // Table doesn't exist
    }

    shared_ptr<Table> table = tables[tableName];
    table->deleteWhere(condition);

    // Save to file
    return table->saveToFile(getTableFilePath(tableName));
}

bool Database::updateRecords(const string& tableName, const vector<UpdateAssignment>& assignments, const Condition& condition) {
    if (tables.find(tableName) == tables.end()) {
        return false; // Table doesn't exist
    }

    shared_ptr<Table> table = tables[tableName];
    vector<Record>& rows = const_cast<vector<Record>&>(table->getRows());

    // Update matching records
    for (auto& record : rows) {
        if (table->evaluateCondition(record, condition)) {
            for (const auto& assignment : assignments) {
                int colIndex = table->getColumnIndex(assignment.column);
                if (colIndex >= 0 && colIndex < (int)record.getSize()) {
                    record.getValue(colIndex) = assignment.value;
                }
            }
        }
    }

    // Save to file
    return table->saveToFile(getTableFilePath(tableName));
}

string Database::executeQuery(const string& query) {
    string trimmedQuery = Utils::trim(query);
    if (trimmedQuery.empty()) {
        return Colors::error("Error: Empty query");
    }

    // Tokenize
    Tokenizer tokenizer(trimmedQuery);
    vector<Token> tokens = tokenizer.tokenize();

    // Parse
    Parser parser(tokens);
    ParsedQuery parsedQuery = parser.parse();

    stringstream result;

    switch (parsedQuery.type) {
        case QueryType::CREATE_TABLE: {
            if (createTable(parsedQuery.tableName, parsedQuery.columns)) {
                result << Colors::BRIGHT_GREEN << "✓" << Colors::RESET << " Table '"
                       << Colors::BRIGHT_YELLOW << parsedQuery.tableName << Colors::RESET
                       << "' created successfully.";
            } else {
                result << Colors::BRIGHT_RED << "✗" << Colors::RESET << " Error: Could not create table '"
                       << Colors::BRIGHT_RED << parsedQuery.tableName << Colors::RESET << "'.";
            }
            break;
        }

        case QueryType::DROP_TABLE: {
            if (dropTable(parsedQuery.tableName)) {
                result << Colors::BRIGHT_GREEN << "✓" << Colors::RESET << " Table '"
                       << Colors::BRIGHT_YELLOW << parsedQuery.tableName << Colors::RESET
                       << "' dropped successfully.";
            } else {
                result << Colors::BRIGHT_RED << "✗" << Colors::RESET << " Error: Could not drop table '"
                       << Colors::BRIGHT_RED << parsedQuery.tableName << Colors::RESET << "'.";
            }
            break;
        }

        case QueryType::INSERT: {
            if (insert(parsedQuery.tableName, parsedQuery.values)) {
                result << Colors::BRIGHT_GREEN << "✓" << Colors::RESET << " Record inserted successfully.";
            } else {
                result << Colors::BRIGHT_RED << "✗" << Colors::RESET << " Error: Could not insert record into '"
                       << Colors::BRIGHT_RED << parsedQuery.tableName << Colors::RESET << "'.";
            }
            break;
        }

        case QueryType::SELECT: {
            vector<Record> records;
            if (parsedQuery.selectAll) {
                parsedQuery.selectColumns.push_back("*");
            }

            Condition* condition = parsedQuery.conditions.empty() ? nullptr : &parsedQuery.conditions[0];
            records = select(parsedQuery.tableName, parsedQuery.selectColumns, condition);

            if (!tableExists(parsedQuery.tableName)) {
                result << Colors::BRIGHT_RED << "✗" << Colors::RESET << " Error: Table '"
                       << Colors::BRIGHT_RED << parsedQuery.tableName << Colors::RESET << "' does not exist.";
            } else {
                const auto& columns = parsedQuery.selectAll ? getTableColumns(parsedQuery.tableName) : parsedQuery.selectColumns;
                
                result << "\n" << Colors::BRIGHT_CYAN << "+" << string(70, '-') << "+" << Colors::RESET << "\n";
                
                // Print header row
                result << Colors::BRIGHT_CYAN << "| " << Colors::RESET;
                for (size_t i = 0; i < columns.size(); ++i) {
                    result << Colors::tableHeader(columns[i]);
                    if (i < columns.size() - 1) {
                        result << Colors::RESET << " " << Colors::BRIGHT_CYAN << "|" << Colors::RESET << " ";
                    }
                }
                result << Colors::RESET << " " << Colors::BRIGHT_CYAN << "|" << Colors::RESET << "\n";
                
                // Print separator
                result << Colors::BRIGHT_CYAN << "+" << string(70, '-') << "+" << Colors::RESET << "\n";

                // Print records
                if (records.empty()) {
                    result << Colors::BRIGHT_CYAN << "| " << Colors::RESET
                           << Colors::dim("(No records found)")
                           << Colors::RESET << string(53, ' ')
                           << Colors::BRIGHT_CYAN << "|" << Colors::RESET << "\n";
                } else {
                    for (const auto& record : records) {
                        result << Colors::BRIGHT_CYAN << "| " << Colors::RESET;
                        for (size_t i = 0; i < record.getSize(); ++i) {
                            string value = record.getValue(i);
                            // Pad values to align columns
                            value.resize(min(size_t(10), value.length() + 2), ' ');
                            result << Colors::CYAN << value << Colors::RESET;
                            if (i < record.getSize() - 1) {
                                result << Colors::BRIGHT_CYAN << "|" << Colors::RESET << " ";
                            }
                        }
                        result << " " << Colors::BRIGHT_CYAN << "|" << Colors::RESET << "\n";
                    }
                }
                
                // Print bottom border
                result << Colors::BRIGHT_CYAN << "+" << string(70, '-') << "+" << Colors::RESET;
            }
            break;
        }

        case QueryType::DELETE: {
            if (!parsedQuery.conditions.empty()) {
                if (deleteRecords(parsedQuery.tableName, parsedQuery.conditions[0])) {
                    result << Colors::BRIGHT_GREEN << "✓" << Colors::RESET << " Records deleted successfully.";
                } else {
                    result << Colors::BRIGHT_RED << "✗" << Colors::RESET << " Error: Could not delete records.";
                }
            } else {
                result << Colors::BRIGHT_RED << "✗" << Colors::RESET << " Error: DELETE requires a WHERE clause.";
            }
            break;
        }

        case QueryType::UPDATE: {
            if (!parsedQuery.conditions.empty() && !parsedQuery.assignments.empty()) {
                if (updateRecords(parsedQuery.tableName, parsedQuery.assignments, parsedQuery.conditions[0])) {
                    result << Colors::BRIGHT_GREEN << "✓" << Colors::RESET << " Records updated successfully.";
                } else {
                    result << Colors::BRIGHT_RED << "✗" << Colors::RESET << " Error: Could not update records.";
                }
            } else {
                result << Colors::BRIGHT_RED << "✗" << Colors::RESET << " Error: UPDATE requires SET and WHERE clauses.";
            }
            break;
        }

        default:
            result << Colors::BRIGHT_RED << "✗" << Colors::RESET << " Error: Invalid query.";
            break;
    }

    return result.str();
}

bool Database::tableExists(const string& tableName) const {
    return tables.find(tableName) != tables.end();
}

const vector<string>& Database::getTableColumns(const string& tableName) const {
    static vector<string> empty;
    auto it = tables.find(tableName);
    if (it != tables.end()) {
        return it->second->getColumns();
    }
    return empty;
}

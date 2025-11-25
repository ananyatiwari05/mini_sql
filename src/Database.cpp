#include "Database.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "FileManager.h"
#include "Utils.h"
#include "Colors.h"
#include <iostream>
#include <sstream>

Database::Database(const std::string& dataDir)
    : dataDirectory(dataDir) {
    FileManager::createDirectory(dataDirectory);
    loadTablesFromDisk();
}

void Database::loadTablesFromDisk() {
    std::vector<std::string> files = FileManager::listFilesInDirectory(dataDirectory);
    for (const auto& filepath : files) {
        Table* table = Table::loadFromFile(filepath);
        if (table) {
            tables[table->getTableName()] = std::shared_ptr<Table>(table);
        }
    }
}

std::string Database::getTableFilePath(const std::string& tableName) const {
    return dataDirectory + "/" + tableName + ".tbl";
}

bool Database::createTable(const std::string& tableName, const std::vector<std::string>& columns) {
    if (tables.find(tableName) != tables.end()) {
        return false; // Table already exists
    }

    std::shared_ptr<Table> table(new Table(tableName, columns));
    tables[tableName] = table;

    // Save to file
    return table->saveToFile(getTableFilePath(tableName));
}

bool Database::insert(const std::string& tableName, const std::vector<std::string>& values) {
    if (tables.find(tableName) == tables.end()) {
        return false; // Table doesn't exist
    }

    std::shared_ptr<Table> table = tables[tableName];
    if (values.size() != table->getColumns().size()) {
        return false; // Column count mismatch
    }

    Record record(values);
    table->insertRow(record);

    // Save to file
    return table->saveToFile(getTableFilePath(tableName));
}

std::vector<Record> Database::select(const std::string& tableName, const std::vector<std::string>& columns,
                                     const Condition* condition) {
    std::vector<Record> result;

    if (tables.find(tableName) == tables.end()) {
        return result; // Table doesn't exist
    }

    std::shared_ptr<Table> table = tables[tableName];

    // Get records
    std::vector<Record> records;
    if (condition) {
        records = table->selectWhere(*condition);
    } else {
        records = table->selectAll();
    }

    // Filter columns if not selecting all
    if (!columns.empty() && columns[0] != "*") {
        std::vector<int> columnIndices;
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

bool Database::deleteRecords(const std::string& tableName, const Condition& condition) {
    if (tables.find(tableName) == tables.end()) {
        return false; // Table doesn't exist
    }

    std::shared_ptr<Table> table = tables[tableName];
    table->deleteWhere(condition);

    // Save to file
    return table->saveToFile(getTableFilePath(tableName));
}

std::string Database::executeQuery(const std::string& query) {
    std::string trimmedQuery = Utils::trim(query);
    if (trimmedQuery.empty()) {
        return Colors::error("Error: Empty query");
    }

    // Tokenize
    Tokenizer tokenizer(trimmedQuery);
    std::vector<Token> tokens = tokenizer.tokenize();

    // Parse
    Parser parser(tokens);
    ParsedQuery parsedQuery = parser.parse();

    std::stringstream result;

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
            std::vector<Record> records;
            if (parsedQuery.selectAll) {
                parsedQuery.columns.push_back("*");
            }

            Condition* condition = parsedQuery.conditions.empty() ? nullptr : &parsedQuery.conditions[0];
            records = select(parsedQuery.tableName, parsedQuery.columns, condition);

            if (!tableExists(parsedQuery.tableName)) {
                result << Colors::BRIGHT_RED << "✗" << Colors::RESET << " Error: Table '"
                       << Colors::BRIGHT_RED << parsedQuery.tableName << Colors::RESET << "' does not exist.";
            } else {
                const auto& columns = parsedQuery.selectAll ? getTableColumns(parsedQuery.tableName) : parsedQuery.columns;
                
                result << "\n" << Colors::BRIGHT_CYAN << "+" << std::string(70, '-') << "+" << Colors::RESET << "\n";
                
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
                result << Colors::BRIGHT_CYAN << "+" << std::string(70, '-') << "+" << Colors::RESET << "\n";

                // Print records
                if (records.empty()) {
                    result << Colors::BRIGHT_CYAN << "| " << Colors::RESET
                           << Colors::dim("(No records found)")
                           << Colors::RESET << std::string(53, ' ')
                           << Colors::BRIGHT_CYAN << "|" << Colors::RESET << "\n";
                } else {
                    for (const auto& record : records) {
                        result << Colors::BRIGHT_CYAN << "| " << Colors::RESET;
                        for (size_t i = 0; i < record.getSize(); ++i) {
                            std::string value = record.getValue(i);
                            // Pad values to align columns
                            value.resize(std::min(size_t(10), value.length() + 2), ' ');
                            result << Colors::CYAN << value << Colors::RESET;
                            if (i < record.getSize() - 1) {
                                result << Colors::BRIGHT_CYAN << "|" << Colors::RESET << " ";
                            }
                        }
                        result << " " << Colors::BRIGHT_CYAN << "|" << Colors::RESET << "\n";
                    }
                }
                
                // Print bottom border
                result << Colors::BRIGHT_CYAN << "+" << std::string(70, '-') << "+" << Colors::RESET;
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

        default:
            result << Colors::BRIGHT_RED << "✗" << Colors::RESET << " Error: Invalid query.";
            break;
    }

    return result.str();
}

bool Database::tableExists(const std::string& tableName) const {
    return tables.find(tableName) != tables.end();
}

const std::vector<std::string>& Database::getTableColumns(const std::string& tableName) const {
    static std::vector<std::string> empty;
    auto it = tables.find(tableName);
    if (it != tables.end()) {
        return it->second->getColumns();
    }
    return empty;
}

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "Tokenizer.h"

// Query types
enum class QueryType {
    CREATE_TABLE,
    CREATE_DATABASE,
    DROP_TABLE,
    DROP_DATABASE,
    INSERT,
    SELECT,
    DELETE,
    UPDATE,
    ALTER_TABLE,
    USE_DATABASE,
    SHOW_TABLES,
    SHOW_DATABASES,
    INVALID
};

// Update assignment
struct UpdateAssignment {
    std::string column;
    std::string value;
};

// Forward declare Condition from Table.h
struct Condition;

// Parsed query structure
struct ParsedQuery {
    QueryType type = QueryType::INVALID;
    bool selectAll = false;
    
    // For CREATE TABLE / DROP TABLE / ALTER TABLE
    std::string tableName;
    std::vector<std::string> columns;
    
    // For CREATE DATABASE / DROP DATABASE / USE DATABASE
    std::string databaseName;
    
    // For INSERT
    std::vector<std::string> values;
    
    // For SELECT
    std::vector<std::string> selectColumns;
    std::vector<Condition> conditions;
    
    // For UPDATE
    std::vector<UpdateAssignment> assignments;
    
    // For ALTER TABLE
    std::string alterType; // ADD, DROP, MODIFY
    std::string newColumnName;
    std::string newColumnType;
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t position;
    
    Token peek(int offset = 0) const;
    Token consume();
    bool match(TokenType type);
    bool match(const std::string& value);
    bool check(TokenType type) const;
    bool check(const std::string& value) const;
    
    ParsedQuery parseCreateTable();
    ParsedQuery parseCreateDatabase();
    ParsedQuery parseDropTable();
    ParsedQuery parseDropDatabase();
    ParsedQuery parseInsert();
    ParsedQuery parseSelect();
    ParsedQuery parseDelete();
    ParsedQuery parseUpdate();
    ParsedQuery parseAlterTable();
    ParsedQuery parseUseDatabase();
    Condition parseCondition();
    
public:
    Parser(const std::vector<Token>& tokens);
    ParsedQuery parse();
};

#endif // PARSER_H

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "Tokenizer.h"
using namespace std;
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
    string column;
    string value;
};

// Forward declare Condition from Table.h
struct Condition;

// Parsed query structure
struct ParsedQuery {
    QueryType type = QueryType::INVALID;
    bool selectAll = false;
    
    // For CREATE TABLE / DROP TABLE / ALTER TABLE
    string tableName;
    vector<string> columns;
    
    // For CREATE DATABASE / DROP DATABASE / USE DATABASE
    string databaseName;
    
    // For INSERT
    vector<string> values;
    
    // For SELECT
    vector<string> selectColumns;
    vector<Condition> conditions;
    
    // For UPDATE
    vector<UpdateAssignment> assignments;
    
    // For ALTER TABLE
    string alterType; // ADD, DROP, MODIFY
    string newColumnName;
    string newColumnType;
};

class Parser {
private:
    vector<Token> tokens;
    size_t position;
    
    Token peek(int offset = 0) const;
    Token consume();
    bool match(TokenType type);
    bool match(const string& value);
    bool check(TokenType type) const;
    bool check(const string& value) const;
    
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
    Parser(const vector<Token>& tokens);
    ParsedQuery parse();
};

#endif // PARSER_H

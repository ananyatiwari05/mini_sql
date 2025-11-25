#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "Table.h"
#include <string>
#include <vector>

// Represents parsed query structure
struct ParsedQuery {
    enum class QueryType {
        CREATE_TABLE,
        INSERT,
        SELECT,
        DELETE,
        INVALID
    };

    QueryType type;
    std::string tableName;
    std::vector<std::string> columns;          // For CREATE TABLE or SELECT
    std::vector<std::string> values;           // For INSERT
    std::vector<Condition> conditions;         // For WHERE clauses
    bool selectAll;                            // For SELECT *

    ParsedQuery()
        : type(QueryType::INVALID), selectAll(false) {}
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

    // Helper functions
    Token peek() const;
    Token consume();
    bool match(TokenType type);
    bool match(const std::string& value);
    bool check(TokenType type) const;
    bool check(const std::string& value) const;

    // Specific parsers
    ParsedQuery parseCreateTable();
    ParsedQuery parseInsert();
    ParsedQuery parseSelect();
    ParsedQuery parseDelete();
    Condition parseCondition();

public:
    Parser(const std::vector<Token>& toks);

    // Main parsing method
    ParsedQuery parse();
};

#endif // PARSER_H

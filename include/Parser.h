#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "Table.h"
#include <string>
#include <vector>
using namespace std;

struct ParsedQuery {
    enum class QueryType {
        CREATE_DATABASE,
        USE_DATABASE,
        CREATE_TABLE,
        DROP_TABLE,
        INSERT,
        SELECT,
        DELETE,
        UPDATE,
        ALTER_TABLE,
        INVALID
    };

    QueryType type;
    string databaseName;
    string tableName;
    vector<string> columns;
    vector<string> values;
    vector<Condition> conditions;
    string orderByColumn;
    bool orderByDesc;
    string groupByColumn;
    bool selectAll;
    
    vector<pair<string, string>> updateValues;
    string alterColumnName;
    string alterColumnType;
    string alterAction; // ADD, DROP, MODIFY

    ParsedQuery()
        : type(QueryType::INVALID), selectAll(false), orderByDesc(false) {}
};

class Parser {
private:
    vector<Token> tokens;
    size_t current;

    Token peek() const;
    Token consume();
    bool match(TokenType type);
    bool match(const string& value);
    bool check(TokenType type) const;
    bool check(const string& value) const;

    ParsedQuery parseCreateDatabase();
    ParsedQuery parseUseDatabase();
    ParsedQuery parseCreateTable();
    ParsedQuery parseDropTable();
    ParsedQuery parseInsert();
    ParsedQuery parseSelect();
    ParsedQuery parseDelete();
    ParsedQuery parseUpdate();
    ParsedQuery parseAlterTable();
    Condition parseCondition();

public:
    Parser(const vector<Token>& toks);
    ParsedQuery parse();
};

#endif // PARSER_H

#include "Parser.h"
#include "Utils.h"
#include <iostream>
using namespace std;

Parser::Parser(const vector<Token>& toks)
    : tokens(toks), current(0) {}

Token Parser::peek() const {
    if (current < tokens.size()) {
        return tokens[current];
    }
    return Token(TokenType::END_OF_INPUT, "");
}

Token Parser::consume() {
    Token token = peek();
    if (current < tokens.size()) {
        current++;
    }
    return token;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        consume();
        return true;
    }
    return false;
}

bool Parser::match(const string& value) {
    Token token = peek();
    if (token.value == value) {
        consume();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) const {
    return peek().type == type;
}

bool Parser::check(const string& value) const {
    return peek().value == value;
}

ParsedQuery Parser::parseCreateTable() {
    ParsedQuery query;
    query.type = ParsedQuery::QueryType::CREATE_TABLE;

    consume(); // CREATE
    consume(); // TABLE

    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }

    if (!match(TokenType::PUNCTUATION) || tokens[current - 1].value != "(") {
        query.type = ParsedQuery::QueryType::INVALID;
        return query;
    }

    while (!check(TokenType::PUNCTUATION) || peek().value != ")") {
        if (check(TokenType::IDENTIFIER)) {
            string colName = consume().value;
            if (check(TokenType::IDENTIFIER)) {
                consume();
            }
            query.columns.push_back(colName);
        }
        if (peek().value == ",") {
            consume();
        }
    }

    match(TokenType::PUNCTUATION);
    return query;
}

ParsedQuery Parser::parseInsert() {
    ParsedQuery query;
    query.type = ParsedQuery::QueryType::INSERT;

    consume(); // INSERT
    consume(); // INTO

    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }

    if (match("values")) {
        if (match(TokenType::PUNCTUATION) && tokens[current - 1].value == "(") {
            while (!check(TokenType::PUNCTUATION) || peek().value != ")") {
                if (check(TokenType::NUMBER)) {
                    query.values.push_back(consume().value);
                } else if (check(TokenType::STRING)) {
                    query.values.push_back(consume().value);
                } else if (check(TokenType::IDENTIFIER)) {
                    query.values.push_back(consume().value);
                }
                if (peek().value == ",") {
                    consume();
                }
            }
            match(TokenType::PUNCTUATION);
        }
    }

    return query;
}

Condition Parser::parseCondition() {
    Condition condition;

    if (check(TokenType::IDENTIFIER)) {
        condition.columnName = consume().value;
    }

    if (check(TokenType::OPERATOR)) {
        condition.op = consume().value;
    }

    if (check(TokenType::NUMBER)) {
        condition.value = consume().value;
    } else if (check(TokenType::STRING)) {
        condition.value = consume().value;
    } else if (check(TokenType::IDENTIFIER)) {
        condition.value = consume().value;
    }

    return condition;
}

ParsedQuery Parser::parseSelect() {
    ParsedQuery query;
    query.type = ParsedQuery::QueryType::SELECT;

    consume();

    if (match(TokenType::PUNCTUATION) && tokens[current - 1].value == "*") {
        query.selectAll = true;
    } else {
        while (check(TokenType::IDENTIFIER) || check(TokenType::PUNCTUATION)) {
            if (check(TokenType::IDENTIFIER)) {
                query.columns.push_back(consume().value);
            }
            if (peek().value == ",") {
                consume();
            } else {
                break;
            }
        }
    }

    if (match("from")) {
        if (check(TokenType::IDENTIFIER)) {
            query.tableName = consume().value;
        }
    }

    if (match("where")) {
        query.conditions.push_back(parseCondition());
    }

    if (match("order")) {
        if (match("by")) {
            if (check(TokenType::IDENTIFIER)) {
                query.orderByColumn = consume().value;
            }
            if (match("desc")) {
                query.orderByDesc = true;
            }
        }
    }

    if (match("group")) {
        if (match("by")) {
            if (check(TokenType::IDENTIFIER)) {
                query.groupByColumn = consume().value;
            }
        }
    }

    return query;
}

ParsedQuery Parser::parseDelete() {
    ParsedQuery query;
    query.type = ParsedQuery::QueryType::DELETE;

    consume(); // DELETE
    consume(); // FROM

    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }

    if (match("where")) {
        query.conditions.push_back(parseCondition());
    }

    return query;
}

ParsedQuery Parser::parseCreateDatabase() {
    ParsedQuery query;
    query.type = ParsedQuery::QueryType::CREATE_DATABASE;
    
    consume(); // CREATE
    consume(); // DATABASE
    
    if (check(TokenType::IDENTIFIER)) {
        query.databaseName = consume().value;
    }
    
    return query;
}

ParsedQuery Parser::parseUseDatabase() {
    ParsedQuery query;
    query.type = ParsedQuery::QueryType::USE_DATABASE;
    
    consume(); // USE
    
    if (check(TokenType::IDENTIFIER)) {
        query.databaseName = consume().value;
    }
    
    return query;
}

ParsedQuery Parser::parseDropTable() {
    ParsedQuery query;
    query.type = ParsedQuery::QueryType::DROP_TABLE;

    consume(); // DROP
    consume(); // TABLE

    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }

    return query;
}

ParsedQuery Parser::parseUpdate() {
    ParsedQuery query;
    query.type = ParsedQuery::QueryType::UPDATE;

    consume(); // UPDATE

    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }

    if (match("set")) {
        while (true) {
            if (check(TokenType::IDENTIFIER)) {
                string colName = consume().value;
                if (match("=")) {
                    string value;
                    if (check(TokenType::NUMBER)) {
                        value = consume().value;
                    } else if (check(TokenType::STRING)) {
                        value = consume().value;
                    } else if (check(TokenType::IDENTIFIER)) {
                        value = consume().value;
                    }
                    query.updateValues.push_back({colName, value});
                }
            }
            if (peek().value == ",") {
                consume();
            } else {
                break;
            }
        }
    }

    if (match("where")) {
        query.conditions.push_back(parseCondition());
    }

    return query;
}

ParsedQuery Parser::parseAlterTable() {
    ParsedQuery query;
    query.type = ParsedQuery::QueryType::ALTER_TABLE;

    consume(); // ALTER
    consume(); // TABLE

    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }

    if (match("add")) {
        query.alterAction = "ADD";
        if (check(TokenType::IDENTIFIER)) {
            query.alterColumnName = consume().value;
        }
        if (check(TokenType::IDENTIFIER)) {
            query.alterColumnType = consume().value;
        }
    } else if (match("drop")) {
        query.alterAction = "DROP";
        if (check(TokenType::IDENTIFIER)) {
            query.alterColumnName = consume().value;
        }
    } else if (match("modify")) {
        query.alterAction = "MODIFY";
        if (check(TokenType::IDENTIFIER)) {
            query.alterColumnName = consume().value;
        }
        if (check(TokenType::IDENTIFIER)) {
            query.alterColumnType = consume().value;
        }
    }

    return query;
}

ParsedQuery Parser::parse() {
    ParsedQuery query;

    Token first = peek();
    if (first.type != TokenType::KEYWORD) {
        return query;
    }

    string keyword = first.value;

    if (keyword == "create") {
        Token second = tokens.size() > 1 ? tokens[1] : Token(TokenType::END_OF_INPUT, "");
        if (second.value == "database") {
            query = parseCreateDatabase();
        } else {
            query = parseCreateTable();
        }
    } else if (keyword == "use") {
        query = parseUseDatabase();
    } else if (keyword == "drop") {
        query = parseDropTable();
    } else if (keyword == "insert") {
        query = parseInsert();
    } else if (keyword == "select") {
        query = parseSelect();
    } else if (keyword == "delete") {
        query = parseDelete();
    } else if (keyword == "update") {
        query = parseUpdate();
    } else if (keyword == "alter") {
        query = parseAlterTable();
    }

    return query;
}

#include "Parser.h"
#include "Utils.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& toks)
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

bool Parser::match(const std::string& value) {
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

bool Parser::check(const std::string& value) const {
    return peek().value == value;
}

ParsedQuery Parser::parseCreateTable() {
    ParsedQuery query;
    query.type = ParsedQuery::QueryType::CREATE_TABLE;

    // Consume CREATE TABLE keywords
    consume(); // CREATE
    consume(); // TABLE

    // Get table name
    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }

    // Expect (
    if (!match(TokenType::PUNCTUATION) || tokens[current - 1].value != "(") {
        query.type = ParsedQuery::QueryType::INVALID;
        return query;
    }

    // Parse column definitions
    while (!check(TokenType::PUNCTUATION) || peek().value != ")") {
        if (check(TokenType::IDENTIFIER)) {
            std::string colName = consume().value;
            // Skip type (INT, TEXT, etc.)
            if (check(TokenType::IDENTIFIER)) {
                consume();
            }
            query.columns.push_back(colName);
        }
        if (peek().value == ",") {
            consume();
        }
    }

    // Consume )
    match(TokenType::PUNCTUATION);

    return query;
}

ParsedQuery Parser::parseInsert() {
    ParsedQuery query;
    query.type = ParsedQuery::QueryType::INSERT;

    // Consume INSERT INTO keywords
    consume(); // INSERT
    consume(); // INTO

    // Get table name
    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }

    // Consume VALUES keyword
    if (match("values")) {
        // Expect (
        if (match(TokenType::PUNCTUATION) && tokens[current - 1].value == "(") {
            // Parse values
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
            // Consume )
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

    // Consume SELECT keyword
    consume();

    // Check for * or column list
    if (match(TokenType::PUNCTUATION) && tokens[current - 1].value == "*") {
        query.selectAll = true;
    } else {
        // Parse column list
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

    // Expect FROM
    if (match("from")) {
        if (check(TokenType::IDENTIFIER)) {
            query.tableName = consume().value;
        }
    }

    // Check for WHERE clause
    if (match("where")) {
        query.conditions.push_back(parseCondition());
    }

    return query;
}

ParsedQuery Parser::parseDelete() {
    ParsedQuery query;
    query.type = ParsedQuery::QueryType::DELETE;

    // Consume DELETE FROM keywords
    consume(); // DELETE
    consume(); // FROM

    // Get table name
    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }

    // Check for WHERE clause
    if (match("where")) {
        query.conditions.push_back(parseCondition());
    }

    return query;
}

ParsedQuery Parser::parse() {
    ParsedQuery query;

    Token first = peek();
    if (first.type != TokenType::KEYWORD) {
        return query;
    }

    std::string keyword = first.value;

    if (keyword == "create") {
        query = parseCreateTable();
    } else if (keyword == "insert") {
        query = parseInsert();
    } else if (keyword == "select") {
        query = parseSelect();
    } else if (keyword == "delete") {
        query = parseDelete();
    }

    return query;
}

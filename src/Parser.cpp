#include "Parser.h"
#include "Utils.h"
#include "Table.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), position(0) {}

Token Parser::peek(int offset) const {
    size_t pos = position + offset;
    if (pos < tokens.size()) {
        return tokens[pos];
    }
    return Token(TokenType::END_OF_INPUT, "");
}

Token Parser::consume() {
    if (position < tokens.size()) {
        return tokens[position++];
    }
    return Token(TokenType::END_OF_INPUT, "");
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        consume();
        return true;
    }
    return false;
}

bool Parser::match(const std::string& value) {
    if (check(value)) {
        consume();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) const {
    return peek().type == type;
}

bool Parser::check(const std::string& value) const {
    return Utils::toLower(peek().value) == Utils::toLower(value);
}

ParsedQuery Parser::parseCreateDatabase() {
    ParsedQuery query;
    query.type = QueryType::CREATE_DATABASE;
    
    consume(); // CREATE
    consume(); // DATABASE
    
    if (check(TokenType::IDENTIFIER)) {
        query.databaseName = consume().value;
    }
    
    return query;
}

ParsedQuery Parser::parseDropDatabase() {
    ParsedQuery query;
    query.type = QueryType::DROP_DATABASE;
    
    consume(); // DROP
    consume(); // DATABASE
    
    if (check(TokenType::IDENTIFIER)) {
        query.databaseName = consume().value;
    }
    
    return query;
}

ParsedQuery Parser::parseUseDatabase() {
    ParsedQuery query;
    query.type = QueryType::USE_DATABASE;
    
    consume(); // USE
    
    if (check(TokenType::IDENTIFIER)) {
        query.databaseName = consume().value;
    }
    
    return query;
}

ParsedQuery Parser::parseCreateTable() {
    ParsedQuery query;
    query.type = QueryType::CREATE_TABLE;
    
    consume(); // CREATE
    consume(); // TABLE
    
    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }
    
    if (peek().value == "(") {
        match(TokenType::PUNCTUATION); // consume "("
        
        // Parse columns
        while (!check(")") && !check(TokenType::END_OF_INPUT)) {
            if (check(TokenType::IDENTIFIER)) {
                query.columns.push_back(consume().value);
            }
            if (check(",")) {
                consume();
            } else if (!check(")")) {
                break;
            }
        }
        match(TokenType::PUNCTUATION); // consume ")"
    }
    
    return query;
}

ParsedQuery Parser::parseDropTable() {
    ParsedQuery query;
    query.type = QueryType::DROP_TABLE;
    
    consume(); // DROP
    consume(); // TABLE
    
    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }
    
    return query;
}

ParsedQuery Parser::parseInsert() {
    ParsedQuery query;
    query.type = QueryType::INSERT;
    
    consume(); // INSERT
    consume(); // INTO
    
    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }
    
    // Parse VALUES (...)
    if (check("VALUES")) {
        consume();
        
        if (peek().value == "(") {
            match(TokenType::PUNCTUATION);
            
            while (!check(")") && !check(TokenType::END_OF_INPUT)) {
                if (check(TokenType::STRING) || check(TokenType::NUMBER) || check(TokenType::IDENTIFIER)) {
                    query.values.push_back(consume().value);
                }
                if (check(",")) {
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
    
    // Parse operator
    if (check("=") || check("==") || check(">") || check("<") || check(">=") || check("<=") || check("!=")) {
        condition.op = consume().value;
    }
    
    // Parse value
    if (check(TokenType::STRING) || check(TokenType::NUMBER) || check(TokenType::IDENTIFIER)) {
        condition.value = consume().value;
    }
    
    return condition;
}

ParsedQuery Parser::parseSelect() {
    ParsedQuery query;
    query.type = QueryType::SELECT;
    
    consume(); // SELECT
    
    // Parse columns or *
    if (peek().value == "*") {
        query.selectAll = true;
        consume();
    } else {
        while (!check("FROM") && !check(TokenType::END_OF_INPUT)) {
            if (check(TokenType::IDENTIFIER)) {
                query.selectColumns.push_back(consume().value);
            }
            if (check(",")) {
                consume();
            }
        }
    }
    
    // Parse FROM
    if (check("FROM")) {
        consume();
        
        if (check(TokenType::IDENTIFIER)) {
            query.tableName = consume().value;
        }
    }
    
    // Parse WHERE clause
    if (check("WHERE")) {
        consume();
        query.conditions.push_back(parseCondition());
    }
    
    return query;
}

ParsedQuery Parser::parseDelete() {
    ParsedQuery query;
    query.type = QueryType::DELETE;
    
    consume(); // DELETE
    consume(); // FROM
    
    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }
    
    // Parse WHERE clause
    if (check("WHERE")) {
        consume();
        query.conditions.push_back(parseCondition());
    }
    
    return query;
}

ParsedQuery Parser::parseUpdate() {
    ParsedQuery query;
    query.type = QueryType::UPDATE;
    
    consume(); // UPDATE
    
    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }
    
    consume(); // SET
    
    // Parse assignments
    while (!check("WHERE") && !check(TokenType::END_OF_INPUT)) {
        UpdateAssignment assignment;
        if (check(TokenType::IDENTIFIER)) {
            assignment.column = consume().value;
        }
        consume(); // =
        if (check(TokenType::STRING) || check(TokenType::NUMBER) || check(TokenType::IDENTIFIER)) {
            assignment.value = consume().value;
        }
        query.assignments.push_back(assignment);
        
        if (check(",")) {
            consume();
        }
    }
    
    // Parse WHERE
    if (check("WHERE")) {
        consume();
        query.conditions.push_back(parseCondition());
    }
    
    return query;
}

ParsedQuery Parser::parseAlterTable() {
    ParsedQuery query;
    query.type = QueryType::ALTER_TABLE;
    
    consume(); // ALTER
    consume(); // TABLE
    
    if (check(TokenType::IDENTIFIER)) {
        query.tableName = consume().value;
    }
    
    if (check("ADD")) {
        query.alterType = "ADD";
        consume();
        if (check(TokenType::IDENTIFIER)) {
            query.newColumnName = consume().value;
        }
    } else if (check("DROP")) {
        query.alterType = "DROP";
        consume();
        if (check(TokenType::IDENTIFIER)) {
            query.newColumnName = consume().value;
        }
    }
    
    return query;
}

ParsedQuery Parser::parse() {
    if (tokens.empty()) {
        return ParsedQuery();
    }
    
    const std::string& cmd = Utils::toLower(tokens[0].value);
    
    if (cmd == "create") {
        if (check("DATABASE") || (position + 1 < tokens.size() && Utils::toLower(tokens[1].value) == "database")) {
            return parseCreateDatabase();
        } else if (check("TABLE") || (position + 1 < tokens.size() && Utils::toLower(tokens[1].value) == "table")) {
            return parseCreateTable();
        }
    } else if (cmd == "drop") {
        if (check("DATABASE") || (position + 1 < tokens.size() && Utils::toLower(tokens[1].value) == "database")) {
            return parseDropDatabase();
        } else if (check("TABLE") || (position + 1 < tokens.size() && Utils::toLower(tokens[1].value) == "table")) {
            return parseDropTable();
        }
    } else if (cmd == "use") {
        return parseUseDatabase();
    } else if (cmd == "insert") {
        return parseInsert();
    } else if (cmd == "select") {
        return parseSelect();
    } else if (cmd == "delete") {
        return parseDelete();
    } else if (cmd == "update") {
        return parseUpdate();
    } else if (cmd == "alter") {
        return parseAlterTable();
    }
    
    return ParsedQuery();
}

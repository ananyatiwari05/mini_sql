#include "Tokenizer.h"
#include <cctype>
#include <algorithm>

Tokenizer::Tokenizer(const std::string& sql) : input(sql), position(0) {}

bool Tokenizer::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Tokenizer::isOperator(char c) {
    return c == '=' || c == '>' || c == '<' || c == '!' || c == '+' || c == '-' || c == '*' || c == '/';
}

bool Tokenizer::isLetter(char c) {
    return std::isalpha(c) || c == '_';
}

bool Tokenizer::isDigit(char c) {
    return std::isdigit(c);
}

Token Tokenizer::readString() {
    char quote = input[position++];
    std::string value;
    while (position < input.length() && input[position] != quote) {
        value += input[position++];
    }
    if (position < input.length() && input[position] == quote) {
        position++;
    }
    return Token(TokenType::STRING, value);
}

Token Tokenizer::readNumber() {
    std::string value;
    while (position < input.length() && (isDigit(input[position]) || input[position] == '.')) {
        value += input[position++];
    }
    return Token(TokenType::NUMBER, value);
}

Token Tokenizer::readIdentifierOrKeyword() {
    std::string value;
    while (position < input.length() && (isLetter(input[position]) || isDigit(input[position]))) {
        value += input[position++];
    }
    
    if (isKeyword(value)) {
        return Token(TokenType::KEYWORD, value);
    }
    return Token(TokenType::IDENTIFIER, value);
}

bool Tokenizer::isKeyword(const std::string& word) {
    std::string lower = word;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    static const std::vector<std::string> keywords = {
        "select", "insert", "create", "delete", "from", "where", "table", 
        "values", "and", "or", "int", "text", "show", "tables", "into",
        "drop", "alter", "update", "use", "database", "add", "modify", "set",
        "databases"
    };
    
    return std::find(keywords.begin(), keywords.end(), lower) != keywords.end();
}

std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;
    
    while (position < input.length()) {
        // Skip whitespace
        if (isWhitespace(input[position])) {
            position++;
            continue;
        }
        
        // Handle quoted strings
        if (input[position] == '"' || input[position] == '\'') {
            tokens.push_back(readString());
            continue;
        }
        
        // Handle punctuation
        if (input[position] == '(' || input[position] == ')' || input[position] == ',' || input[position] == ';') {
            std::string value(1, input[position++]);
            tokens.push_back(Token(TokenType::PUNCTUATION, value));
            continue;
        }
        
        // Handle operators
        if (isOperator(input[position])) {
            std::string value(1, input[position++]);
            if ((value == "=" || value == "!" || value == "<" || value == ">") && 
                position < input.length() && input[position] == '=') {
                value += input[position++];
            }
            tokens.push_back(Token(TokenType::OPERATOR, value));
            continue;
        }
        
        // Handle numbers
        if (isDigit(input[position])) {
            tokens.push_back(readNumber());
            continue;
        }
        
        // Handle identifiers and keywords
        if (isLetter(input[position])) {
            tokens.push_back(readIdentifierOrKeyword());
            continue;
        }
        
        // Unknown character, skip
        position++;
    }
    
    tokens.push_back(Token(TokenType::END_OF_INPUT, ""));
    return tokens;
}

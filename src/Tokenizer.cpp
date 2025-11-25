#include "Tokenizer.h"
#include "Utils.h"
#include <cctype>
#include <iostream>

Tokenizer::Tokenizer(const std::string& sql)
    : input(sql), position(0) {}

bool Tokenizer::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Tokenizer::isLetter(char c) {
    return std::isalpha(c) || c == '_';
}

bool Tokenizer::isDigit(char c) {
    return std::isdigit(c);
}

bool Tokenizer::isOperator(char c) {
    return c == '=' || c == '>' || c == '<' || c == '!';
}

Token Tokenizer::readString() {
    // Consume opening quote
    position++;
    std::string result;
    while (position < input.length() && input[position] != '"' && input[position] != '\'') {
        result += input[position];
        position++;
    }
    // Consume closing quote if present
    if (position < input.length()) {
        position++;
    }
    return Token(TokenType::STRING, result);
}

Token Tokenizer::readNumber() {
    std::string result;
    while (position < input.length() && isDigit(input[position])) {
        result += input[position];
        position++;
    }
    return Token(TokenType::NUMBER, result);
}

Token Tokenizer::readIdentifierOrKeyword() {
    std::string result;
    while (position < input.length() && (isLetter(input[position]) || isDigit(input[position]))) {
        result += input[position];
        position++;
    }
    std::string lower = Utils::toLower(result);
    if (isKeyword(lower)) {
        return Token(TokenType::KEYWORD, lower);
    }
    return Token(TokenType::IDENTIFIER, result);
}

std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;

    while (position < input.length()) {
        char current = input[position];

        // Skip whitespace
        if (isWhitespace(current)) {
            position++;
            continue;
        }

        // String literals
        if (current == '"' || current == '\'') {
            tokens.push_back(readString());
            continue;
        }

        // Numbers
        if (isDigit(current)) {
            tokens.push_back(readNumber());
            continue;
        }

        // Identifiers and keywords
        if (isLetter(current)) {
            tokens.push_back(readIdentifierOrKeyword());
            continue;
        }

        // Operators
        if (isOperator(current)) {
            std::string op;
            op += current;
            position++;
            // Handle multi-character operators like == and !=
            if ((current == '=' || current == '!' || current == '>' || current == '<') &&
                position < input.length() && input[position] == '=') {
                op += input[position];
                position++;
            }
            tokens.push_back(Token(TokenType::OPERATOR, op));
            continue;
        }

        // Punctuation
        if (current == '(' || current == ')' || current == ',' || current == ';') {
            tokens.push_back(Token(TokenType::PUNCTUATION, std::string(1, current)));
            position++;
            continue;
        }

        // Unknown character
        position++;
    }

    tokens.push_back(Token(TokenType::END_OF_INPUT, ""));
    return tokens;
}

bool Tokenizer::isKeyword(const std::string& word) {
    static const std::vector<std::string> keywords = {
        "select", "insert", "create", "delete", "from", "into", "values",
        "table", "where", "and", "or", "not"
    };
    for (const auto& keyword : keywords) {
        if (word == keyword) return true;
    }
    return false;
}

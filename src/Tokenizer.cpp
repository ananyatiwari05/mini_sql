// Tokenizer.cpp (replace your current file with this)
#include "Tokenizer.h"
#include "Utils.h"
#include <cctype>
#include <iostream>
using namespace std;


Tokenizer::Tokenizer(const string& sql)
    : input(sql), position(0) {}

bool Tokenizer::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Tokenizer::isLetter(char c) {
    return isalpha(c) || c == '_';
}

bool Tokenizer::isDigit(char c) {
    return isdigit(c);
}

bool Tokenizer::isOperator(char c) {
    return c == '=' || c == '>' || c == '<' || c == '!';
}

Token Tokenizer::readString() {
    // current char is starting quote ( ' or " )
    char quote = input[position];
    position++;
    string result;
    while (position < input.length() && input[position] != quote) {
        result += input[position];
        position++;
    }
    if (position < input.length()) {
        position++; // consume closing quote
    }
    return Token(TokenType::STRING, result);
}

Token Tokenizer::readNumber() {
    string result;
    while (position < input.length() && (isDigit(input[position]) || input[position]=='.')) {
        result += input[position];
        position++;
    }
    return Token(TokenType::NUMBER, result);
}

Token Tokenizer::readIdentifierOrKeyword() {
    string result;
    while (position < input.length() && (isLetter(input[position]) || isDigit(input[position]))) {
        result += input[position];
        position++;
    }
    string lower = Utils::toLower(result);
    if (isKeyword(lower)) {
        return Token(TokenType::KEYWORD, lower);
    }
    return Token(TokenType::IDENTIFIER, Utils::toLower(result)); // normalize identifiers to lower (optional)
}

vector<Token> Tokenizer::tokenize() {
    vector<Token> tokens;

    while (position < input.length()) {
        char current = input[position];

        if (isWhitespace(current)) {
            position++;
            continue;
        }

        if (current == '"' || current == '\'') {
            tokens.push_back(readString());
            continue;
        }

        if (isDigit(current)) {
            tokens.push_back(readNumber());
            continue;
        }

        if (isLetter(current)) {
            tokens.push_back(readIdentifierOrKeyword());
            continue;
        }

        if (isOperator(current)) {
            string op;
            op += current;
            position++;
            if ((current == '=' || current == '!' || current == '>' || current == '<') &&
                position < input.length() && input[position] == '=') {
                op += input[position];
                position++;
            }
            tokens.push_back(Token(TokenType::OPERATOR, op));
            continue;
        }

        if (current == '(' || current == ')' || current == ',' || current == ';' || current == '*') {
            tokens.push_back(Token(TokenType::PUNCTUATION, string(1, current)));
            position++;
            continue;
        }

        // unknown char -> skip
        position++;
    }

    tokens.push_back(Token(TokenType::END_OF_INPUT, ""));
    return tokens;
}

bool Tokenizer::isKeyword(const string& word) {
    static const vector<string> keywords = {
        // DDL / DML / CONTROL
        "select", "insert", "create", "delete", "update",
        "database", "table", "use", "drop", "alter",

        // clauses / values
        "from", "into", "values", "set", "where",
        "and", "or", "not",

        // ordering / grouping
        "order", "by", "group", "desc", "asc",

        // alter helpers
        "add", "drop", "modify"
    };
    for (const auto& keyword : keywords) {
        if (word == keyword) return true;
    }
    return false;
}

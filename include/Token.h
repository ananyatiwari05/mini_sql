#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
using namespace std;

// Token types for SQL parsing
enum class TokenType {
    KEYWORD,      // SELECT, INSERT, CREATE, DELETE, WHERE, FROM, INTO, VALUES, TABLE
    IDENTIFIER,   // Table names, column names
    NUMBER,       // Integer values
    STRING,       // Quoted string values
    OPERATOR,     // =, ==, >, <, >=, <=, !=
    PUNCTUATION,  // (, ), ,, ;
    UNKNOWN,      // Unknown token
    END_OF_INPUT  // End of input marker
};

// Token structure
struct Token {
    TokenType type;
    string value;

    Token(TokenType t = TokenType::UNKNOWN, string v = "")
        : type(t), value(v) {}
};

#endif // TOKEN_H

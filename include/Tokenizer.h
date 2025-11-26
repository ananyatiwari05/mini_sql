#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "Token.h"
#include <string>
#include <vector>
using namespace std;

class Tokenizer {
private:
    string input;
    size_t position;

    // Helper functions for tokenization
    bool isWhitespace(char c);
    bool isLetter(char c);
    bool isDigit(char c);
    bool isOperator(char c);
    Token readString();
    Token readNumber();
    Token readIdentifierOrKeyword();

public:
    Tokenizer(const string& sql);

    // Main tokenization method
    vector<Token> tokenize();

    // Check if a keyword is recognized
    static bool isKeyword(const string& word);
};

#endif // TOKENIZER_H

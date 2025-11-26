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

    bool isWhitespace(char c);
    bool isLetter(char c);
    bool isDigit(char c);
    bool isOperator(char c);

    Token readString();
    Token readNumber();
    Token readIdentifierOrKeyword();

public:
    Tokenizer(const string& sql);

    vector<Token> tokenize();

    static bool isKeyword(const string& word);
};

#endif // TOKENIZER_H

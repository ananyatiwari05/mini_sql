#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "Token.h"
#include <string>
#include <vector>

class Tokenizer {
private:
    std::string input;
    size_t position;

    bool isWhitespace(char c);
    bool isLetter(char c);
    bool isDigit(char c);
    bool isOperator(char c);

    Token readString();
    Token readNumber();
    Token readIdentifierOrKeyword();

public:
    Tokenizer(const std::string& sql);

    std::vector<Token> tokenize();

    static bool isKeyword(const std::string& word);
};

#endif // TOKENIZER_H

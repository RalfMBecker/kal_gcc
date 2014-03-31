#ifndef LEXER_H_
#define LEXER_H_

#include <string>
#include <cstdio>
#include <cctype>
#include <cstdlib>

// The lexer recognizes 5 tokens, and a catch-all.
// For unknown characters, the lexer returns a token's ascii value
enum Token{
    // eof
    tok_eof = -1,

    // commands
    tok_def = -2, tok_extern = -3,

    // primary
    tok_identifier = -4, tok_number = -5,
};

// it is more logical to make them extern here: they are defined in 
// lexer.cpp translation unit
extern std::string IdentifierStr;  // for tok_identifier
extern double NumVal;       // for tok_number

int gettok(void);

#endif

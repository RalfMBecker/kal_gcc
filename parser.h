#ifndef PARSER_H_
#define PARSER_H_

#include <vector>
#include <map>
#include <string>

#include "error.h"
#include "lexer.h"
#include "ast.h"

extern std::map<char, int> BinopPrecedence;

void BinopPrecedenceCreate();
// make available to driver
int getNextToken();
ExprAST* ParseExpression();
FunctionAST* ParseDefinition(void);
PrototypeAST* ParseExtern(void);
FunctionAST* ParseTopLevelExpr(void);

extern int CurTok;

#endif

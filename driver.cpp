#include <iostream>

#include "parser.h"

void 
HandleDefinition(){
  if ( ParseDefinition() )
    std::cout << "...parsed a function definition.\n";
  else
    getNextToken();
}

void 
HandleExtern(){
  if ( ParseExtern() )
    std::cout << "...parsed an extern statement.\n";
  else
    getNextToken();
}

void 
HandleTopLevelExpression(){
  if ( ParseTopLevelExpr() )
    std::cout << "...parsed a top-level expression.\n";
  else
    getNextToken();
}

void 
MainLoop(){
  for (;;){
    std::cout << "ready> ";
    switch(CurTok){
    case tok_eof:     return;
    case ';':         getNextToken(); break; // ignore at top level
    case tok_def:     HandleDefinition(); break;
    case tok_extern:  HandleExtern(); break;
    default:          HandleTopLevelExpression(); break;
    }
  }
}


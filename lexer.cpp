/********************************************************************
* lexer.cpp - lexer for Kaleidoscope language
* 
********************************************************************/

#include "lexer.h"

std::string IdentifierStr;  // for tok_identifier
double NumVal;              // for tok_number

/// gettok - Return the next token from standard input.
int 
gettok() {
  static int LastChar = ' ';

  // eat ws
  while (isspace(LastChar))
    LastChar = getchar();

  // legal identifier names are of form [al][alnum]*
  if ( isalpha(LastChar) ){ // found an identifier
    IdentifierStr = LastChar;
    while ( isalnum( LastChar = getchar() ) )
      IdentifierStr += LastChar;
    // was command?
    if (IdentifierStr == "def") return tok_def;
    if (IdentifierStr == "extern") return tok_extern;

    return tok_identifier;
  }

  // process numbers. unary +/- are processed at end of gettoken()
  // we don't allow scientific notation (as in '5e7') and such
  if ( isdigit(LastChar) ) {
    std::string tmpStr;
    // do-while as opposed to while-do forced by initial 'if'
    do{
      tmpStr += LastChar;
      LastChar = getchar();
    } while ( isdigit(LastChar) );
    // if you handle the '.' above (eg, ' || LastChar == '.'), you will
    // read strings like 45.343.343. as valid
    if ( LastChar == '.' ) { 
      tmpStr += LastChar;
      LastChar = getchar();
    }
    while ( isdigit(LastChar) ){
      tmpStr += LastChar;
      LastChar = getchar();
    }

    NumVal = strtod(tmpStr.c_str(), 0);
    return tok_number;
  } // end 'if number' scope

  // eat comments
  if ( LastChar == '#' ){
    while ( isspace(LastChar = getchar()) )
      ;
    if (LastChar != EOF)
      return gettok(); // re-throw when done with comment line
  } 

  // did we hit EOF?
  if ( LastChar == EOF )
    return tok_eof;

  // if we come here, we have found no token etc, and return the char we found
  // note that we also need to ready the next LastChar
  int tmpChar = LastChar;
  LastChar = getchar();
  return tmpChar;

}

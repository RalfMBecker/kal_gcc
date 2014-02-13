/****************************************************************
* parser.cpp -    core parser routines                          *
*                                                               *
* Note:           - a function declaration needs to be followed *
*                   by its definition (implementation)          *
*                 - prototype and usage is inconsistent; eg:    *
*                   def text(x y) x+text(x-1, y+1);             *
*                                                               * 
****************************************************************/

#include "parser.h"
#include "ast.h"

// binary precedence table, and initialization routine
// as the map is extendible, declaring it non-const seems just right
std::map<char, int> BinopPrecedence;

// intuition: table determines if a sequence of binary expressions
// associates left-to-right, or right-to-left; in a context of left-to-right
// parsing of lines of the source code (see example before function
// ParseBinOpRHS)
void
BinopPrecedenceCreate(){
  BinopPrecedence['<'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 20;
  BinopPrecedence['*'] = 40;
}

int CurTok; // current token being parsed
int 
getNextToken() { return (CurTok = gettok()); }


/*****************************************************
*      *** parsing "Primary Expressions ***          *
*****************************************************/
 
// process number
ExprAST* 
ParseNumberExpr(){
  ExprAST* res = new NumberExprAST(NumVal);
  getNextToken();
  return res;
}

// process parentheses
ExprAST* 
ParseParenExpr(){
  getNextToken(); // eat '('

  ExprAST* V = ParseExpression(); // mutually recursive call
  if (!V) return 0; // returning 0 allows the toy program to keep running
                      // after error

  if ( CurTok != ')' )
    throw( ExprAST_Error( "near )" ) );

  getNextToken();
  return V;
}

// process IdentifierExpr
//     -> identifier
//     -> identifier ( expression [,expression]* )
ExprAST* 
ParseIdentifierExpr(){
  std::string name = IdentifierStr;
  getNextToken();

  // case -> identifier
  if ( CurTok != '(' )
    return new VariableExprAST(name);

  // case function call -> identifier ( expression [, expression]* )
  getNextToken(); // if we get here, CurToken was '(' prior to this call
  std::vector<ExprAST*> args;
  if ( CurTok != ')' ){
    for (;;){ // keep reading args until error or ')'
      ExprAST* arg = ParseExpression();
      if (!arg) return 0; // ****** check ********
      args.push_back(arg);

      if ( CurTok == ')' ) break;

      if ( CurTok != ',' )
	throw( ExprAST_Error( "near )" ) );
      getNextToken();
    }
  } // end 'if'

  getNextToken();

  return new CallExprAST(name, args);;
} // end ParseIdentifierExpr
     
// *** this really should be a factory function ***
ExprAST* 
ParsePrimary(){
  switch (CurTok){
  case tok_identifier: return ParseIdentifierExpr();
  case tok_number:     return ParseNumberExpr();
  case '(':            return ParseParenExpr();
  default:             throw( ExprAST_Error( "in expression" ) );
  }
} // end switch cases

/***********************************************
*     *** binary expression parsing ***        *
***********************************************/

// helper function to consult table BinopPrecedence
int 
GetTokPrecedence(){
  if ( !isascii(CurTok) )
    return -1; // no binary operator

    // subscripting operation inserts if key not currently present
    // so before using it to retrieve its key-value, need to check
    // if key present
  typedef std::map<char, int>::iterator Iter;
  if ( BinopPrecedence.find(CurTok) == BinopPrecedence.end() )
    return -1; // not in table
  return BinopPrecedence[CurTok]; // by prior lines, no need
                                    // to check validity
}

ExprAST* ParseBinOpRHS(int, ExprAST*); // break dependency cycle. Function not 
                   // in parser.h header file (not part of interface)

// process expression
//       -> primary [binopRHS]*
//       -> epsilon
ExprAST* 
ParseExpression(){

  ExprAST* LHS = ParsePrimary(); // see above
  // case epsilon
  if (!LHS) return 0;

  // case primary [binaopRHS]* (we already have the primary == LHS)
  return ParseBinOpRHS(0, LHS); // entering with only an expression,
                               // so no precedence restrictions
}

// the ops of an expression (tying together the primary expressions)
// are compared left-to right, as in:
// a + b * c: (1) get a, [+b]
//            (2) cmp + and *'s precedence: 20 < 40 => binds right-to-left
//            (3) so next get binexp for b, [*c], then, finally
//            (4) combine as a, [+, (b * c) ], yielding a + (b * c)
//                (this is the outer (LHS op RHS))
ExprAST* 
ParseBinOpRHS(int ExprPrec, ExprAST* LHS){
  // recall the CurTok "points ahead". The three precedence compares are:
  // ExprPrec (expr) TokPrec (expr) NextPrec (0 (expr) 20 (expr) 40)
  for (;;){
    int TokPrec = GetTokPrecedence(); // for (0, a), this is +
    if ( TokPrec < ExprPrec ) // this LHS is finished; return it
      return LHS;
    // next: to determine if we process as
    //         a, [+, b], [*, c] (left-to-right), or
    //         a, [+, (b, [*, c])] (right-to-left)
    // this depends on precedence value comparison between +/*; so get it
    // keep evaluating left-to-right until a token of lower precence found
    int BinOp = CurTok; // store it
    getNextToken();
    ExprAST* RHS = ParsePrimary();
    if (!RHS) return 0; // ***** should be an error, not just 0  ****

    // we now have access to a, +, b. Next: compare + and *
    int NextPrec = GetTokPrecedence();
    // the next lines amount to: if a + b + c, evaluate as (a + b) + c;
    // aka, at identical precedence level, evaluate left-to-right 
    if (TokPrec < NextPrec){ // go right-to-left, if
      RHS = ParseBinOpRHS(TokPrec + 1, RHS); // "+1": stay on right until
                // fully done. Note that after flipping, we go left-to-right
                // again on the future RHS
      if (RHS == 0) return 0; // ***** again, this should be error *****
      }
    // in all cases, we still have a LHS and RHS to merge
    LHS = new BinaryExprAST(BinOp, LHS, RHS);
    } // end for(;;) loop   
 // ***** POSSIBLE ISSUE: adding +1 is only harmless if we do not deal with
 // ***** deeply recursive situations (if we dip 21 deep, we are in trouble)

}

/*************************************************************
*      *** parsing of fct prototypes; ftcs; externs ***      *
* Note: top-level expressions are handled as a f() type      *
*************************************************************/

// support fct prototypes of form f( [id]* [,id]* )
PrototypeAST* 
ParsePrototype(){
  if (CurTok != tok_identifier)
    throw( PrototypeAST_Error("- expected function") );

  std::string name = IdentifierStr;
  getNextToken();
  if ( CurTok != '(' )
    throw( PrototypeAST_Error("- expected '('") );

  std::vector<std::string> args;
  while ( getNextToken() == tok_identifier)
    args.push_back(IdentifierStr);
  if ( CurTok != ')' )
    throw( PrototypeAST_Error("- expected ')'") );
  getNextToken();

  return new PrototypeAST(name, args);
}

// support "def" keyword
FunctionAST* 
ParseDefinition(void){
  getNextToken(); // discard "def"

  PrototypeAST* Proto = ParsePrototype();
  if (Proto == 0) return 0;   // **** as always, should be error ****

  if ( ExprAST* Expr = ParseExpression() )
    return new FunctionAST(Proto, Expr);
  return 0; // something went wrong; see comment above
}

// support "extern" keyword
PrototypeAST* 
ParseExtern(void){
  getNextToken(); // discard "extern"
  return ParsePrototype();
}

// finally, wrap any top-level expression into a defined function - 
// using a token (anonymous) prototype, and a function body
FunctionAST* 
ParseTopLevelExpr(void){
  if (ExprAST* Expr = ParseExpression() ){
    PrototypeAST* Proto = new PrototypeAST( "", std::vector<std::string>() );
    return new FunctionAST(Proto, Expr);
  }
  return 0; // the usual: keep alive, as opposed to error
}

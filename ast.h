/*************************************************************
* ast.cpp - Abstract Syntax Tree for Kaleidoscope            *
*                                                            *
*************************************************************/

#ifndef AST_H_
#define AST_H_

// *********** consider relocation to some header ************
#include <string>
#include <vector>

// abstract base
// as only one type accepted (double), doesn't need a type field
class ExprAST {
public:
  virtual ~ExprAST() {} // virtual destructor for polym. safety
};

// numbers (double)
class NumberExprAST: public ExprAST{
public: 
  NumberExprAST(double val)
  : Val(val) {}
private:
  NumberExprAST() {}; // ***** confirm/add ************
  double Val;
};

// variables
class VariableExprAST: public ExprAST{
public:
  VariableExprAST(const std::string& name)
  : Name(name) {}
private:
  std::string Name;
};

// basic binary expression
class BinaryExprAST: public ExprAST{
public:
  BinaryExprAST(char op, ExprAST* lhs, ExprAST* rhs)
    : Op(op),
      LHS(lhs),
      RHS(rhs) {}
private:
  char Op;
  ExprAST *LHS, *RHS;
};

// function calls
class CallExprAST: public ExprAST{
public:
  // ********* should 2nd arg be handed on as 'const' ***********
  CallExprAST(const std::string& callee, std::vector<ExprAST*> &args)
    : Callee(callee), 
      Args(args) {}
private:
  std::string Callee;
  std::vector<ExprAST*> Args;
}; 

// function prototype
class PrototypeAST{
public:
  PrototypeAST(const std::string& name, const std::vector<std::string>& args)
    : Name(name),
      Args(args) {}
private:
  std::string Name;
  std::vector<std::string> Args;
};

class FunctionAST{
public:
  FunctionAST(PrototypeAST* proto, ExprAST* body)
    : Proto(proto),
      Body(body) {}
private:
  PrototypeAST* Proto;
  ExprAST* Body;
};

#endif

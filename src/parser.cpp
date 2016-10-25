//
// Created by joe on 10/19/16.
//

#include "calcc/parser.h"
#include "calcc/error.h"

using namespace calcc::ast;
using namespace calcc::parser;
using namespace calcc;
using namespace std;

char lc;

void skipComments(istream &sin) {
  char lc = '#';
  while (lc != '\n' && sin.good())
    sin.get(lc);
}

int nextChar(istream &sin, char &c){
  sin.get(c);
  if (!sin.good()) return 0;
  if (isblank(c) || c == '\n') {
    lc = c;
    return 0;
  }
  if (c == '(' || c == ')') {
    sin.unget();
    return 0;
  }
  lc = c;
  return 1;
}

void skipBlank(istream &sin) {
  char c;
  do {
    sin.get(c);
    if (c=='#' && lc == '\n') skipComments(sin);
    else if (!isblank(c) && c!='\n')
      break;
    else
      lc = c;
    if (!sin.good())
      return;
  } while (1);
  sin.unget();
}

string getToken(istream &sin) {
  char c;
  string res = "";
  // Skip leading non-code
  skipBlank(sin);
  // Return NULL if at EOF
  if (!sin.good()) return res;
  // Check if is braces
  nextChar(sin,c);
  res += c;
  if (c=='(' || c == ')') {
    sin.get(c);
    return res;
  }
  while (nextChar(sin, c))
    res += c;
  // Ignore remaining blanks
  return res;
}

void expect(istream &sin, string expected) {
  string token = getToken(sin);
  if (token!=expected)
    throw error::parser("Invalid syntax, expected \"" + expected + "\", got \"" + token + "\"");
}

// Parse Token
llvm::APInt parseInt(const string &str) {
  long long res;
  try {
    res = stoll(str);
  } catch (exception &e) {
    throw error::parser(e.what());
  }
  uint64_t ures = res;
  return llvm::APInt(64,ures,/*isSigned=*/true);
}

// Parse OP
calcc::ast::BINOP_TYPE parseOp(const string &str) {
  if (str=="+") return calcc::ast::BINOP_PLUS;
  if (str=="-") return calcc::ast::BINOP_MINUS;
  if (str=="*") return calcc::ast::BINOP_MULT;
  if (str=="/") return calcc::ast::BINOP_DIV;
  if (str=="%") return calcc::ast::BINOP_MOD;

  if (str=="==") return calcc::ast::BINOP_EQ;
  if (str=="!=") return calcc::ast::BINOP_NE;
  if (str=="<") return calcc::ast::BINOP_LT;
  if (str=="<=") return calcc::ast::BINOP_LE;
  if (str==">") return calcc::ast::BINOP_GT;
  if (str==">=") return calcc::ast::BINOP_GE;
  throw error::parser("Unknown operator: "+str);
}

Expr* parseExpr(istream &sin, bool paren) {
  string token;
  token = getToken(sin);
  // Empty life
  if (token == "") throw error::parser("Expect expression, got EOF");
  // Parentheses
  if (token == ")") throw error::parser("Expect expression, got \")\"");
  if (token == "(") {
    Expr* res;
    res = parseExpr(sin, true);
    expect(sin, ")");
    return res;
  }
  // Int literal
  if ((token[0] == '-' && token.length()>0)|| isdigit(token[0])) {
    llvm::APInt val = parseInt(token);
    return new IntLiteral(val,VAL_INT);
  }
  // Reserved word & identifier
  if (isalpha(token[0])) {
    // If node
    if (token == "if") {
      if (!paren) throw error::parser("Missing parentheses over if statement: " + token);
      Expr *cnd = parseExpr(sin,false);
      Expr *thn = parseExpr(sin,false);
      Expr *els = parseExpr(sin,false);
      return new If(cnd,thn,els);
    }
    // Seq Node
    if (token == "seq") {
      if (!paren) throw error::parser("Missing parentheses over seq statement: " + token);
      Expr* lhs = parseExpr(sin, false);
      Expr* rhs = parseExpr(sin, false);
      return new Seq(lhs,rhs);
    }
    // While Node
    if (token == "while") {
      if (!paren) throw error::parser("Missing parentheses over while statement: " + token);
      Expr* cnd = parseExpr(sin, false);
      Expr* body = parseExpr(sin, false);
      return new While(cnd, body);
    }
    // Set Node
    if (token == "set") {
      if (!paren) throw error::parser("Missing parentheses over set statement: " + token);
      Expr* e = parseExpr(sin, false);
      Expr* ref = parseExpr(sin, false);
      return new Set(e, ref);
    }
    // Bool literal
    if (token == "true") {
      llvm::APInt val(1,1);
      return new IntLiteral(val,VAL_BOOL);
    }
    if (token == "false") {
      llvm::APInt val(1,0);
      return new IntLiteral(val,VAL_BOOL);
    }
    return new Ref(token);
  } else {
    // Binary Operator
    if (!paren) throw error::parser("Missing parentheses over binary operator: " + token);
    BINOP_TYPE op = parseOp(token);
    Expr *lhs = parseExpr(sin, false);
    Expr *rhs = parseExpr(sin, false);
    return new BinaryOp(op,lhs,rhs);
  }
  throw error::parser("Fall-through in parseExpr");
  return NULL;
}

// Parse the main file
Expr* calcc::parser::parse(istream &sin) {
  Expr* res = NULL;
  lc = '\n';
  res = parseExpr(sin, false);
  expect(sin, "");
  std::vector<VDecl*> params;
  for (int i=0;i<6;++i)
    params.push_back(new VDecl("a"+to_string(i),VAL_INT, true));
  Expr* func = new FDecl("f",VAL_INT,params, res);
  return func;
}

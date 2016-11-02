//
// Created by joe on 10/19/16.
//

#include "calcc/parser.h"
#include "calcc/error.h"

using namespace calcc::ast;
using namespace calcc::parser;
using namespace calcc;
using namespace std;

namespace {

  char lc;

  void skipComments(istream &sin) {
    char lc = '#';
    while (lc != '\n' && sin.good())
      sin.get(lc);
  }

  int nextChar(istream &sin, char &c) {
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
      if (c == '#' && lc == '\n') skipComments(sin);
      else if (!isblank(c) && c != '\n')
        break;
      else
        lc = c;
      if (!sin.good())
        return;
    } while (1);
    sin.unget();
  }

  string getToken(istream &sin, int &pos) {
    char c;
    string res = "";
    // Skip leading non-code
    skipBlank(sin);
    // Return NULL if at EOF
    if (!sin.good()) return res;
    // Check if is braces
    pos = (int)sin.tellg();
    nextChar(sin, c);
    res += c;
    if (c == '(' || c == ')') {
      sin.get(c);
      return res;
    }
    while (nextChar(sin, c))
      res += c;
    // Ignore remaining blanks
    return res;
  }

  void expect(istream &sin, string expected) {
    int pos;
    string token = getToken(sin, pos);
    if (token != expected)
      throw error::parser("Invalid syntax, expected \"" + expected + "\", got \"" + token + "\" at " + to_string(pos));
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
    return llvm::APInt(64, ures,/*isSigned=*/true);
  }

// Parse OP
  calcc::ast::BINOP_TYPE parseOp(const string &str) {
    if (str == "+") return calcc::ast::BINOP_PLUS;
    if (str == "-") return calcc::ast::BINOP_MINUS;
    if (str == "*") return calcc::ast::BINOP_MULT;
    if (str == "/") return calcc::ast::BINOP_DIV;
    if (str == "%") return calcc::ast::BINOP_MOD;

    if (str == "==") return calcc::ast::BINOP_EQ;
    if (str == "!=") return calcc::ast::BINOP_NE;
    if (str == "<") return calcc::ast::BINOP_LT;
    if (str == "<=") return calcc::ast::BINOP_LE;
    if (str == ">") return calcc::ast::BINOP_GT;
    if (str == ">=") return calcc::ast::BINOP_GE;
    throw error::parser("Unknown operator: " + str);
  }

  Expr *parseExpr(istream &sin, bool paren) {
    string token;
    int tokpos;
    token = getToken(sin, tokpos);
    // Empty life
    if (token == "") throw error::parser("Expect expression, got EOF");
    // Parentheses
    if (token == ")") throw error::parser("Expect expression, got \")\" at " + to_string(tokpos));
    if (token == "(") {
      if (paren) throw error::parser("Extra parenthesis over parenthesis at " + to_string(tokpos));
      Expr *res;
      res = parseExpr(sin, true);
      expect(sin, ")");
      return res;
    }
    // Int literal
    if ((token[0] == '-' && token.length() > 1) || isdigit(token[0])) {
      if (paren) throw error::parser("Extra parenthesis over token: " + token + ", at " + to_string(tokpos));
      llvm::APInt val = parseInt(token);
      return (new IntLiteral(val, VAL_INT))->setPos(tokpos);
    }
    // Reserved word & identifier
    if (isalpha(token[0])) {
      // If node
      if (token == "if") {
        if (!paren) throw error::parser("Missing parentheses over if statement: " + token + ", at " + to_string(tokpos));
        Expr *cnd = parseExpr(sin, false);
        Expr *thn = parseExpr(sin, false);
        Expr *els = parseExpr(sin, false);
        return (new If(cnd, thn, els))->setPos(tokpos);
      }
      // Seq Node
      if (token == "seq") {
        if (!paren) throw error::parser("Missing parentheses over seq statement: " + token + ", at " + to_string(tokpos));
        Expr *lhs = parseExpr(sin, false);
        Expr *rhs = parseExpr(sin, false);
        return (new Seq(lhs, rhs))->setPos(tokpos);
      }
      // While Node
      if (token == "while") {
        if (!paren) throw error::parser("Missing parentheses over while statement: " + token + ", at " + to_string(tokpos));
        Expr *cnd = parseExpr(sin, false);
        Expr *body = parseExpr(sin, false);
        return (new While(cnd, body))->setPos(tokpos);
      }
      // Set Node
      if (token == "set") {
        if (!paren) throw error::parser("Missing parentheses over set statement: " + token + ", at " + to_string(tokpos));
        Expr *e = parseExpr(sin, false);
        Expr *ref = parseExpr(sin, false);
        return (new Set(e, ref))->setPos(tokpos);
      }
      // Bool literal
      if (token == "true") {
        if (paren) throw error::parser("Extra parenthesis over token: " + token + ", at " + to_string(tokpos));
        llvm::APInt val(1, 1);
        return (new IntLiteral(val, VAL_BOOL))->setPos(tokpos);
      }
      if (token == "false") {
        if (paren) throw error::parser("Extra parenthesis over token: " + token + ", at " + to_string(tokpos));
        llvm::APInt val(1, 0);
        return (new IntLiteral(val, VAL_BOOL))->setPos(tokpos);
      }
      if (paren) throw error::parser("Extra parenthesis over token: " + token + ", at " + to_string(tokpos));
      return (new Ref(token))->setPos(tokpos);
    } else {
      // Binary Operator
      if (!paren) throw error::parser("Missing parentheses over binary operator: " + token + ", at " + to_string(tokpos));
      BINOP_TYPE op = parseOp(token);
      Expr *lhs = parseExpr(sin, false);
      Expr *rhs = parseExpr(sin, false);
      return (new BinaryOp(op, lhs, rhs))->setPos(tokpos);
    }
    throw error::parser("Fall-through in parseExpr");
    return NULL;
  }
}

// Parse the main file
Expr* calcc::parser::parse(istream &sin) {
  Expr *bdy = NULL;
  lc = '\n';
  bdy = parseExpr(sin, false);
  expect(sin, "");
  std::vector<VDecl *> params;
  for (int i = 0; i < 6; ++i)
    params.push_back(new VDecl("a" + to_string(i), VAL_INT, true));
  for (int i = 0; i < 10; ++i) {
    VDecl* m = new VDecl("m" + to_string(i), VAL_INT, false);
    bdy = new VScope(m, bdy);
  }
  Expr* func = new FDecl("f",VAL_INT,params, bdy);
  return func;
}

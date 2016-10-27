//
// Created by joe on 10/21/16.
//

#include "calcc/tools/checker.h"

using namespace calcc::ast;
using namespace calcc::tools;
using namespace calcc;
using namespace std;

Expr *Checker::scan(BinaryOp *e, int &out) {
  Scanner::scan(e, out);
  if (e->getOP() >= BINOP_EQ) {
    if (e->getLHS()->getValType() != VAL_INT || e->getRHS()->getValType() != VAL_INT)
      throw error::scanner("Comparison is only valid between two Int values");
  } else {
    if (e->getLHS()->getValType() != VAL_INT || e->getRHS()->getValType() != VAL_INT)
      throw error::scanner("Math operation is only valid between two Int values");
  }
  return e;
}

Expr *Checker::scan(FDecl *e, int &out) {
  Scanner::scan(e, out);
  if (e->getBody()->getValType() != e->getValType())
    throw error::scanner("Function body not returning value of type Int");
  return e;
}

Expr *Checker::scan(If *e, int &out) {
  Scanner::scan(e, out);
  if (e->getCnd()->getValType() != VAL_BOOL)
    throw error::scanner("Condition for If statement is not of type Bool");
  if (e->getThn()->getValType() != e->getEls()->getValType())
    throw error::scanner("Then and Else part of If statements return different types");
  return e;
}

ast::Expr *Checker::scan(ast::Set *e, int &out) {
  Scanner::scan(e, out);
  if (e->getRef()->getExprType() != EXPR_REF)
    throw error::scanner("Right hand side of Set is not an reference");
  else {
    ast::Ref *r = (ast::Ref *) e->getRef();
    if (r->getDecl()->getExprType() != EXPR_VDECL)
      throw error::scanner("Right hand side of Set is not an variable: " + r->getName());
    else {
      ast::VDecl *d = (ast::VDecl *) r->getDecl();
      if (d->isConst())
        throw error::scanner("Right hand side of Set is an immutable variable: " + d->getName());
    }
  }
  return e;
}
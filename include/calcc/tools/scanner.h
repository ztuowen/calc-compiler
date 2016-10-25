//
// Created by joe on 10/19/16.
//

#ifndef CALCCOMPILER_CALCASTSCANNER_H
#define CALCCOMPILER_CALCASTSCANNER_H

#include "calcc/AST.h"
#include "calcc/error.h"

namespace calcc {
  namespace tools {
    //! Generic AST Scanner
    template<class T>
    class Scanner {
    protected:
      virtual ast::Expr* scan(ast::IntLiteral* e, T &out) { return e; }
      virtual ast::Expr* scan(ast::Ref* e, T &out) { return e; }
      virtual ast::Expr* scan(ast::VDecl* e, T &out) { return e; }
      virtual ast::Expr* scan(ast::FDecl* e, T &out) {
        run(e->getBody(), out);
        return e;
      }
      virtual ast::Expr* scan(ast::If* e, T &out) {
        run(e->getCnd(), out);
        run(e->getThn(), out);
        run(e->getEls(), out);
        return e;
      }
      virtual ast::Expr* scan(ast::BinaryOp* e, T &out) {
        run(e->getLHS(), out);
        run(e->getRHS(), out);
        return e;
      }
      virtual ast::Expr* scan(ast::ValPtr* e, T &out) { return e; }
      virtual ast::Expr* scan(ast::Set* e, T &out) {
        run(e->getExpr(), out);
        run(e->getRef(), out);
        return e; }
      virtual ast::Expr* scan(ast::Seq* e, T &out) {
        run(e->getLHS(), out);
        run(e->getRHS(), out);
        return e; }
      virtual ast::Expr* scan(ast::While* e, T &out) {
        run(e->getCnd(), out);
        run(e->getBdy(), out);
        return e;
      }
    public:
      virtual ast::Expr* run(ast::Expr* e, T &out) {
        if (!e) throw error::scanner("Null expression!");
        switch (e->getExprType()) {
          case ast::EXPR_INT: return scan(static_cast<ast::IntLiteral*>(e), out);
          case ast::EXPR_REF: return scan(static_cast<ast::Ref*>(e), out);
          case ast::EXPR_IF: return scan(static_cast<ast::If*>(e), out);
          case ast::EXPR_BINOP: return scan(static_cast<ast::BinaryOp*>(e), out);
          case ast::EXPR_VDECL: return scan(static_cast<ast::VDecl*>(e), out);
          case ast::EXPR_FDECL: return scan(static_cast<ast::FDecl*>(e), out);
          case ast::EXPR_VALP: return scan(static_cast<ast::ValPtr*>(e), out);
          case ast::EXPR_SET: return scan(static_cast<ast::Set*>(e), out);
          case ast::EXPR_SEQ: return scan(static_cast<ast::Seq*>(e), out);
          case ast::EXPR_WHILE: return scan(static_cast<ast::While*>(e), out);
          default: throw error::scanner("Unknown node type");
        }
      }
    };
  }
}

#endif //CALCCOMPILER_CALCASTSCANNER_H

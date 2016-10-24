//
// Created by joe on 10/19/16.
//

#ifndef CALCCOMPILER_CALCASTSCANNER_H
#define CALCCOMPILER_CALCASTSCANNER_H

#include "calcc/AST.h"
#include "calcc/error.h"

namespace calcc {
  namespace ast {
    template<class T>
    class Scanner {
    protected:
      virtual Expr* scan(IntLiteral* e, T &out) { return e; }
      virtual Expr* scan(Ref* e, T &out) { return e; }
      virtual Expr* scan(VDecl* e, T &out) { return e; }
      virtual Expr* scan(FDecl* e, T &out) {
        run(e->getBody(), out);
        return e;
      }
      virtual Expr* scan(If* e, T &out) {
        run(e->getCnd(), out);
        run(e->getThn(), out);
        run(e->getEls(), out);
        return e;
      }
      virtual Expr* scan(BinaryOp* e, T &out) {
        run(e->getLHS(), out);
        run(e->getRHS(), out);
        return e;
      }
      virtual Expr* scan(ValPtr* e, T &out) { return e; }
    public:
      virtual Expr* run(Expr* e, T &out) {
        if (!e) throw error::scanner("Null expression!");
        switch (e->getExprType()) {
          case EXPR_INT: return scan(static_cast<IntLiteral*>(e), out);
          case EXPR_REF: return scan(static_cast<Ref*>(e), out);
          case EXPR_IF: return scan(static_cast<If*>(e), out);
          case EXPR_BINOP: return scan(static_cast<BinaryOp*>(e), out);
          case EXPR_VDECL: return scan(static_cast<VDecl*>(e), out);
          case EXPR_FDECL: return scan(static_cast<FDecl*>(e), out);
          case EXPR_VALP: return scan(static_cast<ValPtr*>(e), out);
          default: throw error::scanner("Unknown node type");
        }
      }
    };
  }
}

#endif //CALCCOMPILER_CALCASTSCANNER_H

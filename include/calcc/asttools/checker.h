//
// Created by joe on 10/21/16.
//

#ifndef CALCCOMPILER_CHECKER_H
#define CALCCOMPILER_CHECKER_H

#include "calcc/AST.h"
#include "calcc/asttools/scanner.h"

namespace calcc {
  namespace ast {
    class Checker : public Scanner< int > {
    protected:
      virtual Expr* scan(FDecl* e, int &out);
      virtual Expr* scan(If* e, int &out);
      virtual Expr* scan(BinaryOp* e, int &out);
    };
  }
}
#endif //CALCCOMPILER_CHECKER_H

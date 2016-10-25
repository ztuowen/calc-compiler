//
// Created by joe on 10/21/16.
//

#ifndef CALCCOMPILER_CHECKER_H
#define CALCCOMPILER_CHECKER_H

#include "calcc/AST.h"
#include "calcc/tools/scanner.h"

namespace calcc {
  namespace tools {
    //! Basic type checking
    class Checker : public Scanner< int > {
    protected:
      virtual ast::Expr* scan(ast::FDecl* e, int &out);
      virtual ast::Expr* scan(ast::If* e, int &out);
      virtual ast::Expr* scan(ast::BinaryOp* e, int &out);
      virtual ast::Expr* scan(ast::Set* e, int &out);
    };
  }
}
#endif //CALCCOMPILER_CHECKER_H

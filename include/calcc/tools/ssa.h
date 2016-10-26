//
// Created by ztuowen on 10/25/16.
//

#ifndef CALCCOMPILER_SSA_H
#define CALCCOMPILER_SSA_H

#include "calcc/tools/scanner.h"

namespace calcc {
  namespace tools {
    //! Crude SSA resolution
    class SSAfier : public Scanner< ast::vset > {
    protected:
      virtual ast::Expr* scan(ast::FDecl* e, ast::vset &out);
      virtual ast::Expr* scan(ast::If* e, ast::vset &out);
      virtual ast::Expr* scan(ast::While* e, ast::vset &out);
      virtual ast::Expr* scan(ast::Set* e, ast::vset &out);
    };
  }
}

#endif //CALCCOMPILER_SSA_H

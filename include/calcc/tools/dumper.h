//
// Created by joe on 10/21/16.
//

#ifndef CALCCOMPILER_DUMPER_H
#define CALCCOMPILER_DUMPER_H

#include <ostream>
#include "calcc/AST.h"
#include "calcc/tools/scanner.h"

namespace calcc {
  namespace tools {
    //! Basic AST dump
    class Dumper : public Scanner< std::ostream > {
    protected:
      virtual ast::Expr* scan(ast::IntLiteral* e, std::ostream &out);
      virtual ast::Expr* scan(ast::Ref* e, std::ostream &out);
      virtual ast::Expr* scan(ast::VDecl* e, std::ostream &out);
      virtual ast::Expr* scan(ast::FDecl* e, std::ostream &out);
      virtual ast::Expr* scan(ast::If* e, std::ostream &out);
      virtual ast::Expr* scan(ast::BinaryOp* e, std::ostream &out);
    public:
      virtual ast::Expr* run(ast::Expr* e, std::ostream &out) {
        ast::Expr *ret = Scanner::run(e,out);
        out.flush();
        return ret;
      }
    };
  }
}

#endif //CALCCOMPILER_DUMPER_H

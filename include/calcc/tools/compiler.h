//
// Created by joe on 10/21/16.
//

#ifndef CALCCOMPILER_COMPILER_H
#define CALCCOMPILER_COMPILER_H

#include <map>
#include "calcc/AST.h"
#include "calcc/tools/scanner.h"

namespace calcc {
  namespace tools {
    typedef std::map<std::string, ast::ValPtr*> valmap;
    //! Compile AST into LLVM IR
    class Compiler : public Scanner< valmap > {
    protected:
      virtual ast::Expr* scan(ast::IntLiteral* e, valmap &out);
      virtual ast::Expr* scan(ast::Ref* e, valmap &out);
      virtual ast::Expr* scan(ast::VDecl* e, valmap &out);
      virtual ast::Expr* scan(ast::FDecl* e, valmap &out);
      virtual ast::Expr* scan(ast::If* e, valmap &out);
      virtual ast::Expr* scan(ast::BinaryOp* e, valmap &out);
      virtual ast::Expr* scan(ast::ValPtr* e, valmap &out);
    };
  }
}

#endif //CALCCOMPILER_COMPILER_H

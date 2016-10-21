//
// Created by joe on 10/21/16.
//

#ifndef CALCCOMPILER_COMPILER_H
#define CALCCOMPILER_COMPILER_H

#include <map>
#include "calcc/AST.h"
#include "calcc/asttools/scanner.h"

namespace calcc {
  namespace ast {
    typedef std::map<std::string, ValPtr*> valmap;
    class Compiler : public Scanner< valmap > {
    protected:
      virtual Expr* scan(IntLiteral* e, valmap &out);
      virtual Expr* scan(Ref* e, valmap &out);
      virtual Expr* scan(VDecl* e, valmap &out);
      virtual Expr* scan(FDecl* e, valmap &out);
      virtual Expr* scan(If* e, valmap &out);
      virtual Expr* scan(BinaryOp* e, valmap &out);
      virtual Expr* scan(ValPtr* e, valmap &out);
    };
  }
}

#endif //CALCCOMPILER_COMPILER_H

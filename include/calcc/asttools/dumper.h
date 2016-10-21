//
// Created by joe on 10/21/16.
//

#ifndef CALCCOMPILER_DUMPER_H
#define CALCCOMPILER_DUMPER_H

#include <ostream>
#include "calcc/AST.h"
#include "calcc/asttools/scanner.h"

namespace calcc {
  namespace ast {
    class Dumper : public Scanner< std::ostream > {
    protected:
      virtual Expr* scan(IntLiteral* e, std::ostream &out);
      virtual Expr* scan(Ref* e, std::ostream &out);
      virtual Expr* scan(VDecl* e, std::ostream &out);
      virtual Expr* scan(FDecl* e, std::ostream &out);
      virtual Expr* scan(If* e, std::ostream &out);
      virtual Expr* scan(BinaryOp* e, std::ostream &out);
    public:
      virtual Expr* run(Expr* e, std::ostream &out) {
        Expr *ret = Scanner::run(e,out);
        out.flush();
        return ret;
      }
    };
  }
}

#endif //CALCCOMPILER_DUMPER_H

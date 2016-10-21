//
// Created by joe on 10/21/16.
//

#ifndef CALCCOMPILER_REFERRER_H
#define CALCCOMPILER_REFERRER_H

#include <map>
#include "calcc/AST.h"
#include "calcc/asttools/scanner.h"

namespace calcc {
  namespace ast {
    typedef std::map<std::string, Decl*> declmap;
    class Referrer : public Scanner< declmap > {
    protected:
      virtual Expr* scan(Ref* e, declmap &out);
      virtual Expr* scan(FDecl* e, declmap &out);
    };
  }
}

#endif //CALCCOMPILER_REFERRER_H

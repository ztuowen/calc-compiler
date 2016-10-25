//
// Created by joe on 10/21/16.
//

#ifndef CALCCOMPILER_REFERRER_H
#define CALCCOMPILER_REFERRER_H

#include <map>
#include "calcc/AST.h"
#include "calcc/tools/scanner.h"

namespace calcc {
  namespace tools {
    typedef std::map<std::string, ast::Decl*> declmap;
    //! Link reference to corresponding declaration
    class Referrer : public Scanner< declmap > {
    protected:
      virtual ast::Expr* scan(ast::Ref* e, declmap &out);
      virtual ast::Expr* scan(ast::FDecl* e, declmap &out);
    };
  }
}

#endif //CALCCOMPILER_REFERRER_H

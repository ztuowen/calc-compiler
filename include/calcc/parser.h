//
// Created by joe on 10/19/16.
//

#ifndef CALCCOMPILER_PARSER_H
#define CALCCOMPILER_PARSER_H

#include "AST.h"
#include <string>

namespace calcc {
  namespace parser {
    ast::Expr* parse(std::istream &sin);
  }
}

#endif //CALCCOMPILER_PARSER_H

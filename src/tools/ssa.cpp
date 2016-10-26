//
// Created by ztuowen on 10/25/16.
//

#include "calcc/tools/ssa.h"

using namespace calcc::ast;
using namespace calcc::tools;
using namespace calcc;
using namespace std;

ast::Expr* SSAfier::scan(ast::FDecl *e, ast::vset &out) {
  run(e->getBody(), e->getMod());
  return e;
}

ast::Expr* SSAfier::scan(ast::If *e, ast::vset &out) {
  run(e->getCnd(), out);
  vset thn;
  run(e->getThn(), thn);
  vset els;
  run(e->getEls(), els);
  e->getMod().insert(thn.begin(),thn.end());
  e->getMod().insert(els.begin(),els.end());
  out.insert(e->getMod().begin(),e->getMod().end());
  return e;
}

ast::Expr* SSAfier::scan(ast::Set *e, ast::vset &out) {
  run(e->getExpr(), out);
  out.insert(static_cast<VDecl*>(static_cast<Ref*>(e->getRef())->getDecl()));
  return e;
}

ast::Expr* SSAfier::scan(ast::While *e, ast::vset &out) {
  run(e->getCnd(), out);
  vset bdy;
  run(e->getBdy(), bdy);
  e->getMod().insert(bdy.begin(),bdy.end());
  out.insert(bdy.begin(),bdy.end());
  return e;
}

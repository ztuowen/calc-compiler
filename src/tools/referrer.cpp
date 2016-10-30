//
// Created by joe on 10/21/16.
//

#include "calcc/tools/referrer.h"

using namespace calcc::ast;
using namespace calcc::tools;
using namespace calcc;
using namespace std;

Expr *Referrer::scan(FDecl *e, declmap &out) {
  vector<VDecl *> p = e->getParams();
  vector<Decl *> oldp;
  out[e->getName()] = e;
  for (auto par = p.begin(); par != p.end(); ++par) {
    auto it = out.find((*par)->getName());
    if (it != out.end()) {
      oldp.push_back(it->second);
      out.erase(it);
    }
    out[(*par)->getName()] = *par;
  }
  // This is only needed because we don't want it to refer to other things
  Expr *ret = Scanner<declmap>::scan(e, out);
  // Need explicit clean up
  for (auto par = p.begin(); par != p.end(); ++par)
    out.erase((*par)->getName());
  for (auto i = oldp.begin(); i != oldp.end(); ++i)
    out[(*i)->getName()] = *i;
  return ret;
}

Expr *Referrer::scan(Ref *e, declmap &out) {
  auto it = out.find(e->getName());
  if (it == out.end())
    throw error::scanner("Unknown identifier: " + e->getName());
  e->setDecl(it->second);
  return e;
}

ast::Expr *Referrer::scan(ast::VScope *e, declmap &out) {
  Decl *oldd = NULL;
  if (out.find(e->getDecl()->getName()) != out.end()) {
    oldd = out[e->getDecl()->getName()];
  }
  out[e->getDecl()->getName()] = e->getDecl();
  run(e->getExpr(), out);
  out.erase(e->getDecl()->getName());
  if (oldd)
    out[e->getDecl()->getName()] = oldd;
  return e;
}

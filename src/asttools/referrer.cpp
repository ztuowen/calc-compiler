//
// Created by joe on 10/21/16.
//

#include "calcc/asttools/referrer.h"
#include "calcc/error.h"

using namespace calcc::ast;
using namespace calcc;
using namespace std;

Expr* Referrer::scan(FDecl *e, declmap &out) {
  vector<VDecl*> p = e->getParams();
  vector<Decl*> oldp;
  out[e->getName()] = e;
  for (int i=0;i<p.size();++i) {
    auto it = out.find(p[i]->getName());
    if (it!=out.end()) {
      oldp.push_back(it->second);
      out.erase(it);
    }
    out[p[i]->getName()] = p[i];
  }
  Expr *ret = Scanner<declmap>::scan(e,out);
  // Need explicit clean up
  for (int i=0;i<p.size();++i)
    out.erase(p[i]->getName());
  for (int i=0;i<oldp.size();++i)
    out[oldp[i]->getName()] = oldp[i];
  return ret;
}

Expr* Referrer::scan(Ref *e, declmap &out) {
  auto it = out.find(e->getName());
  if (it == out.end())
    throw error::scanner("Unknown identifier: "+e->getName());
  e->setDecl(it->second);
  return e;
}
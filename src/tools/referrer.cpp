//
// Created by joe on 10/21/16.
//

#include "calcc/tools/referrer.h"

using namespace calcc::ast;
using namespace calcc::tools;
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
  // This is only needed because we don't want it to refer to other things
  for (int i=0;i<10;++i) {
    string name = "m"+to_string(i);
    VDecl* m = new VDecl(name, VAL_INT, false);
    out[name] = m;
  }
  Expr *ret = Scanner<declmap>::scan(e,out);
  // Need explicit clean up
  for (int i=0;i<p.size();++i)
    out.erase(p[i]->getName());
  for (int i=0;i<10;++i) {
    string name = "m" + to_string(i);
    out.erase(name);
  }
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
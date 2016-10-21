//
// Created by joe on 10/21/16.
//

#include "calcc/asttools/dumper.h"
#include "calcc/error.h"

using namespace calcc::ast;
using namespace calcc;
using namespace std;

Expr* Dumper::scan(IntLiteral *e, std::ostream &out) {
  out << "(Int "<< e->getValue().toString(10,true)<<": "<< e->getValType()<<")";
  return e;
}

Expr* Dumper::scan(BinaryOp *e, std::ostream &out) {
  out << "(Op "<< e->getOP()<< ": ";
  run(e->getLHS(),out);
  run(e->getRHS(),out);
  return e;
}

Expr* Dumper::scan(FDecl *e, std::ostream &out) {
  out << "(FDecl "<< e->getName()<< ": " << e->getValType() << " (";
  vector<VDecl*> p = e->getParams();
  for (int i = 0; i<p.size();++i) {
    run(p[i], out);
    out << " ";
  }
  out<<") ";
  run(e->getBody(),out);
  out <<")";
  return e;
}

Expr* Dumper::scan(If *e, std::ostream &out) {
  out << "(If ";
  run(e->getCnd(), out);
  run(e->getThn(), out);
  run(e->getEls(), out);
  out << ")";
  return e;
}

Expr* Dumper::scan(Ref *e, std::ostream &out) {
  out<<"(Ref "<< e->getName()<< ": "<< e->getValType() <<")";
  return e;
}

Expr* Dumper::scan(VDecl *e, std::ostream &out) {
  out<<"(VDecl "<< e->getName()<< ": "<< e->getValType()<<")";
  return e;
}

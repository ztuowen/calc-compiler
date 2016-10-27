//
// Created by joe on 10/21/16.
//

#include "calcc/tools/dumper.h"

using namespace calcc::ast;
using namespace calcc::tools;
using namespace calcc;
using namespace std;

Expr *Dumper::scan(IntLiteral *e, std::ostream &out) {
  out << "(Int " << e->getValue().toString(10, true) << ": " << e->getValType() << ")";
  return e;
}

Expr *Dumper::scan(BinaryOp *e, std::ostream &out) {
  out << "(Op " << e->getOP() << ": ";
  run(e->getLHS(), out);
  out << " ";
  run(e->getRHS(), out);
  return e;
}

Expr *Dumper::scan(FDecl *e, std::ostream &out) {
  out << "(FDecl " << e->getName() << ": " << e->getValType() << " (";
  vector<VDecl *> p = e->getParams();
  for (int i = 0; i < p.size(); ++i) {
    run(p[i], out);
    out << " ";
  }
  out << ") ";
  indent_inc();
  indent(out);
  run(e->getBody(), out);
  indent_dec();
  out << ")\n";
  return e;
}

Expr *Dumper::scan(If *e, std::ostream &out) {
  out << "(If ";
  run(e->getCnd(), out);
  indent_inc();
  indent(out);
  run(e->getThn(), out);
  indent(out);
  run(e->getEls(), out);
  indent_dec();
  out << ")";
  return e;
}

Expr *Dumper::scan(Ref *e, std::ostream &out) {
  out << "(Ref " << e->getName() << ": " << e->getValType() << ")";
  return e;
}

Expr *Dumper::scan(VDecl *e, std::ostream &out) {
  out << "(VDecl " << e->getName() << ": " << e->getValType() << ")";
  return e;
}

ast::Expr *Dumper::scan(ast::Seq *e, std::ostream &out) {
  out << "(Seq :";
  run(e->getLHS(), out);
  indent(out);
  run(e->getRHS(), out);
  out << ")";
  return e;
}

ast::Expr *Dumper::scan(ast::Set *e, std::ostream &out) {
  out << "(Set :";
  run(e->getExpr(), out);
  indent(out);
  run(e->getRef(), out);
  out << ")";
  return e;
}

ast::Expr *Dumper::scan(ast::While *e, std::ostream &out) {
  out << "(While :";
  run(e->getCnd(), out);
  indent_inc();
  indent(out);
  run(e->getBdy(), out);
  indent_dec();
  out << ")";
  return e;
}

ast::Expr *Dumper::scan(ast::VScope *e, std::ostream &out) {
  out << "(VScope :";
  run(e->getDecl(), out);
  indent(out);
  run(e->getExpr(), out);
  out << ")";
  return e;
}

void Dumper::indent(std::ostream &out) {
  out << endl << id;
}

void Dumper::indent_inc() {
  id += "  ";
}

void Dumper::indent_dec() {
  id = id.substr(2);
}
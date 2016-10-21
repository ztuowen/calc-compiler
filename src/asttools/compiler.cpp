//
// Created by joe on 10/21/16.
//

#include "calcc/asttools/compiler.h"
#include "calcc/global_llvm.h"

using namespace calcc::ast;
using namespace calcc;
using namespace std;
using namespace llvm;

static llvm::BasicBlock* BB;

Type *toType(VAL_TYPE vt) {
  switch (vt) {
    case VAL_INT: return Type::getInt64Ty(calcc::C);
    case VAL_BOOL: return Type::getInt1Ty(calcc::C);
    default: throw error::scanner("Unknown type error");
  }
}

Expr* Compiler::scan(FDecl *e, valmap &out) {
  // Setup Parameters
  std::vector<Type *> params;
  vector<VDecl*> p = e->getParams();
  for (int i = 0; i<p.size(); ++i)
    params.push_back(toType(p[i]->getValType()));
  FunctionType *FT = FunctionType::get(toType(e->getValType()), params, false);
  Function *F = Function::Create(FT, Function::ExternalLinkage, "f", &*calcc::M);

  BB = BasicBlock::Create(calcc::C, "entry", F);

  auto it = F->arg_begin();
  for (int i = 0; i<p.size(); ++i, ++it)
    p[i]->setVPtr(new ValPtr(&*it, NULL)); // What to do for arguments' basic block

  calcc::Builder.SetInsertPoint(BB);

  ValPtr* ret = (ValPtr*)Scanner::run(e->getBody(), out);

  // Setup Return
  calcc::Builder.CreateRet(ret->getValue());

  return e;
}

Expr* Compiler::scan(IntLiteral *e, valmap &out) {
  return new ValPtr(llvm::ConstantInt::get(C, e->getValue()), BB);
}

Expr* Compiler::scan(BinaryOp *e, valmap &out) {
  ValPtr* lhs = (ValPtr*)Scanner::run(e->getLHS(), out);
  ValPtr* rhs = (ValPtr*)Scanner::run(e->getRHS(), out);
  switch (e->getOP()) {
    case BINOP_PLUS: return new ValPtr(Builder.CreateAdd(lhs->getValue(),rhs->getValue()),BB);
    case BINOP_MINUS: return new ValPtr(Builder.CreateSub(lhs->getValue(),rhs->getValue()),BB);
    case BINOP_MULT: return new ValPtr(Builder.CreateMul(lhs->getValue(),rhs->getValue()),BB);
    case BINOP_DIV: return new ValPtr(Builder.CreateSDiv(lhs->getValue(),rhs->getValue()),BB);
    case BINOP_MOD: return new ValPtr(Builder.CreateSRem(lhs->getValue(),rhs->getValue()),BB);

    case BINOP_EQ: return new ValPtr(Builder.CreateICmpEQ(lhs->getValue(),rhs->getValue()),BB);
    case BINOP_LT: return new ValPtr(Builder.CreateICmpSLT(lhs->getValue(),rhs->getValue()),BB);
    case BINOP_LE: return new ValPtr(Builder.CreateICmpSLE(lhs->getValue(),rhs->getValue()),BB);
    case BINOP_GT: return new ValPtr(Builder.CreateICmpSGT(lhs->getValue(),rhs->getValue()),BB);
    case BINOP_GE: return new ValPtr(Builder.CreateICmpSGE(lhs->getValue(),rhs->getValue()),BB);
    default:
      throw error::scanner("Unknown binary op type");
  }
  return NULL;
}

Expr* Compiler::scan(If *e, valmap &out) {
  ValPtr* cnd = (ValPtr*)Scanner::run(e->getCnd(), out);
  ValPtr* thn = (ValPtr*)Scanner::run(e->getThn(), out);
  ValPtr* els = (ValPtr*)Scanner::run(e->getEls(), out);
  return new ValPtr(Builder.CreateSelect(cnd->getValue(),thn->getValue(),els->getValue()),BB);
}

Expr* Compiler::scan(Ref *e, valmap &out) {
  Decl* d = e->getDecl();
  if (d->getExprType() == EXPR_FDECL)
    throw error::scanner("Can't refer to a FDecl yet");
  VDecl* vd = (VDecl*) d;
  return vd->getVPtr();
}

Expr* Compiler::scan(ValPtr *e, valmap &out) {
  return e;
}

Expr* Compiler::scan(VDecl *e, valmap &out) {
  throw error::scanner("Error: trying to compile a VDecl");
  return e;
}

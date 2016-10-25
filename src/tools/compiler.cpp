//
// Created by joe on 10/21/16.
//

#include "calcc/tools/compiler.h"
#include "calcc/global_llvm.h"

using namespace calcc::ast;
using namespace calcc::tools;
using namespace calcc;
using namespace std;
using namespace llvm;

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
  // Create function & binding
  Function *F = Function::Create(FT, Function::ExternalLinkage, "f", &*calcc::M);
  e->setVPtr(new ValPtr(F));
  // Setup function parameters
  auto it = F->arg_begin();
  for (int i = 0; i<p.size(); ++i, ++it)
    p[i]->setVPtr(new ValPtr(&*it));
  // Function body
  Builder.SetInsertPoint(BasicBlock::Create(calcc::C, "entry", F));
  ValPtr* ret = (ValPtr*)Scanner::run(e->getBody(), out);
  // Setup Return
  calcc::Builder.CreateRet(ret->getValue());
  return e->getVPtr();
}

Expr* Compiler::scan(IntLiteral *e, valmap &out) {
  return new ValPtr(llvm::ConstantInt::get(C, e->getValue()));
}

Expr* Compiler::scan(BinaryOp *e, valmap &out) {
  ValPtr* lhs = (ValPtr*)Scanner::run(e->getLHS(), out);
  ValPtr* rhs = (ValPtr*)Scanner::run(e->getRHS(), out);
  switch (e->getOP()) {
    case BINOP_PLUS: return new ValPtr(Builder.CreateAdd(lhs->getValue(),rhs->getValue()));
    case BINOP_MINUS: return new ValPtr(Builder.CreateSub(lhs->getValue(),rhs->getValue()));
    case BINOP_MULT: return new ValPtr(Builder.CreateMul(lhs->getValue(),rhs->getValue()));
    case BINOP_DIV: return new ValPtr(Builder.CreateSDiv(lhs->getValue(),rhs->getValue()));
    case BINOP_MOD: return new ValPtr(Builder.CreateSRem(lhs->getValue(),rhs->getValue()));

    case BINOP_EQ: return new ValPtr(Builder.CreateICmpEQ(lhs->getValue(),rhs->getValue()));
    case BINOP_NE: return new ValPtr(Builder.CreateICmpNE(lhs->getValue(),rhs->getValue()));
    case BINOP_LT: return new ValPtr(Builder.CreateICmpSLT(lhs->getValue(),rhs->getValue()));
    case BINOP_LE: return new ValPtr(Builder.CreateICmpSLE(lhs->getValue(),rhs->getValue()));
    case BINOP_GT: return new ValPtr(Builder.CreateICmpSGT(lhs->getValue(),rhs->getValue()));
    case BINOP_GE: return new ValPtr(Builder.CreateICmpSGE(lhs->getValue(),rhs->getValue()));
    default:
      throw error::scanner("Unknown binary op type");
  }
  return NULL;
}

Expr* Compiler::scan(If *e, valmap &out) {
  // Calculate condition
  ValPtr* cnd = (ValPtr*)Scanner::run(e->getCnd(), out);
  // Setup blocks
  Function *F = Builder.GetInsertBlock()->getParent();
  BasicBlock* thnB = BasicBlock::Create(calcc::C, "thenIf", F);
  BasicBlock* elsB = BasicBlock::Create(calcc::C, "elseIf", F);
  BasicBlock* aftB = BasicBlock::Create(calcc::C, "afterIf", F);
  // Create conditional branch
  Builder.CreateCondBr(cnd->getValue(),thnB,elsB);
  // Then
  Builder.SetInsertPoint(thnB);
  ValPtr* thn = (ValPtr*)Scanner::run(e->getThn(), out);
  Builder.CreateBr(aftB);
  thnB = Builder.GetInsertBlock();
  // Else
  Builder.SetInsertPoint(elsB);
  ValPtr* els = (ValPtr*)Scanner::run(e->getEls(), out);
  Builder.CreateBr(aftB);
  elsB = Builder.GetInsertBlock();
  // After with PHI node
  Builder.SetInsertPoint(aftB);
  PHINode* ret = Builder.CreatePHI(toType(e->getValType()),2);
  ret->addIncoming(thn->getValue(), thnB);
  ret->addIncoming(els->getValue(), elsB);
  return new ValPtr(ret);
}

Expr* Compiler::scan(Ref *e, valmap &out) {
  Decl* d = e->getDecl();
  if (d->getExprType() == EXPR_FDECL)
    throw error::scanner("Can't refer to a FDecl yet, Function name: " + e->getName());
  VDecl* vd = (VDecl*) d;
  return vd->getVPtr();
}

Expr* Compiler::scan(ValPtr *e, valmap &out) {
  return e;
}

Expr* Compiler::scan(VDecl *e, valmap &out) {
  throw error::scanner("Error: trying to compile a VDecl: " + e->getName());
  return e;
}

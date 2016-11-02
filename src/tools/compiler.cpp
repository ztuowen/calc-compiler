//
// Created by joe on 10/21/16.
//

#include <llvm/Support/raw_ostream.h>
#include "calcc/tools/compiler.h"
#include "calcc/global_llvm.h"

using namespace calcc::ast;
using namespace calcc::tools;
using namespace calcc;
using namespace std;
using namespace llvm;

namespace {
  Function *sadd, *ssub, *smul, *trap;
  Type *toType(VAL_TYPE vt) {
    switch (vt) {
      case VAL_INT:
        return Type::getInt64Ty(calcc::C);
      case VAL_BOOL:
        return Type::getInt1Ty(calcc::C);
      default:
        throw error::scanner("Unknown type error");
    }
  }
  void makeTrap(int pos) {
    auto p = ConstantInt::get(toType(VAL_INT),(uint64_t)pos,true);
    Builder.CreateCall(trap, vector<Value*>(1,p));
    Builder.CreateUnreachable();
  }
  ValPtr* makeWrappedChecks(Function *f, ValPtr* lhs, ValPtr* rhs, int pos) {
    vector<Value*> parms;
    parms.push_back(lhs->getValue());
    parms.push_back(rhs->getValue());
    Value* ret = Builder.CreateCall(f, parms);
    Value* res = Builder.CreateExtractValue(ret, vector<unsigned int>(1,0));
    Value* cnd = Builder.CreateExtractValue(ret, vector<unsigned int>(1,1));
    Function *F = Builder.GetInsertBlock()->getParent();
    BasicBlock *over = BasicBlock::Create(calcc::C, "over", F);
    BasicBlock *safe = BasicBlock::Create(calcc::C, "safe", F);
    Builder.CreateCondBr(cnd, over, safe);
    Builder.SetInsertPoint(over);
    makeTrap(pos);
    Builder.SetInsertPoint(safe);
    return new ValPtr(res);
  }
  ValPtr* makeDivChecks(ValPtr *lhs, ValPtr *rhs, int pos) {
    Value* zero = ConstantInt::get(toType(VAL_INT), 0);
    Value* none = ConstantInt::get(C, APInt(64,(uint64_t)-1,true));
    Value* nmax = ConstantInt::get(C, APInt::getSignedMinValue(64));
    Value* cnd1 = Builder.CreateICmpEQ(rhs->getValue(),zero);
    Value* cnd2 = Builder.CreateAnd(Builder.CreateICmpEQ(lhs->getValue(),nmax),Builder.CreateICmpEQ(rhs->getValue(),none));
    Value* cnd = Builder.CreateOr(cnd1,cnd2);
    Function *F = Builder.GetInsertBlock()->getParent();
    BasicBlock *over = BasicBlock::Create(calcc::C, "over", F);
    BasicBlock *safe = BasicBlock::Create(calcc::C, "safe", F);
    Builder.CreateCondBr(cnd, over, safe);
    Builder.SetInsertPoint(over);
    makeTrap(pos);
    Builder.SetInsertPoint(safe);
    return new ValPtr(Builder.CreateSDiv(lhs->getValue(),rhs->getValue()));
  }
  ValPtr* makeModChecks(ValPtr *lhs, ValPtr *rhs, int pos) {
    Value* zero = ConstantInt::get(toType(VAL_INT), 0);
    Value* cnd = Builder.CreateICmpEQ(rhs->getValue(), zero);
    Function *F = Builder.GetInsertBlock()->getParent();
    BasicBlock *over = BasicBlock::Create(calcc::C, "over", F);
    BasicBlock *safe = BasicBlock::Create(calcc::C, "safe", F);
    Builder.CreateCondBr(cnd, over, safe);
    Builder.SetInsertPoint(over);
    makeTrap(pos);
    Builder.SetInsertPoint(safe);
    return new ValPtr(Builder.CreateSRem(lhs->getValue(),rhs->getValue()));
  }
}

Expr *Compiler::scan(FDecl *e, valmap &out) {
  // Setup Parameters
  std::vector<Type *> params;
  vector<VDecl *> p = e->getParams();
  for (auto par = p.begin(); par != p.end(); ++par)
    params.push_back(toType((*par)->getValType()));
  FunctionType *FT = FunctionType::get(toType(e->getValType()), params, false);
  // Create function & binding
  Function *F = Function::Create(FT, Function::ExternalLinkage, "f", &*calcc::M);
  e->setVPtr(new ValPtr(F));
  // Setup function parameters
  auto it = F->arg_begin();
  for (auto par = p.begin(); par != p.end(); ++par, ++it)
    (*par)->setVPtr(new ValPtr(&*it));
  // Hacky trick so that uninitialized variable has initial value of 0
  Value *zero = llvm::ConstantInt::get(Type::getInt64Ty(C), 0);
  for (auto it = e->getMod().begin(); it != e->getMod().end(); ++it)
    (*it)->setVPtr(new ValPtr(zero));
  // Function body
  Builder.SetInsertPoint(BasicBlock::Create(calcc::C, "entry", F));
  ValPtr *ret = (ValPtr *) Scanner::run(e->getBody(), out);
  // Setup Return
  calcc::Builder.CreateRet(ret->getValue());
  return e->getVPtr();
}

Expr *Compiler::scan(IntLiteral *e, valmap &out) {
  return new ValPtr(llvm::ConstantInt::get(C, e->getValue()));
}

Expr *Compiler::scan(BinaryOp *e, valmap &out) {
  ValPtr *lhs = (ValPtr *) Scanner::run(e->getLHS(), out);
  ValPtr *rhs = (ValPtr *) Scanner::run(e->getRHS(), out);
  switch (e->getOP()) {
    case BINOP_PLUS:
      if (checks) return makeWrappedChecks(sadd, lhs, rhs, e->getPos());
      return new ValPtr(Builder.CreateAdd(lhs->getValue(), rhs->getValue()));
    case BINOP_MINUS:
      if (checks) return makeWrappedChecks(ssub, lhs, rhs, e->getPos());
      return new ValPtr(Builder.CreateSub(lhs->getValue(), rhs->getValue()));
    case BINOP_MULT:
      if (checks) return makeWrappedChecks(smul, lhs, rhs, e->getPos());
      return new ValPtr(Builder.CreateMul(lhs->getValue(), rhs->getValue()));
    case BINOP_DIV:
      if (checks) return makeDivChecks(lhs,rhs,e->getPos());
      return new ValPtr(Builder.CreateSDiv(lhs->getValue(), rhs->getValue()));
    case BINOP_MOD:
      if (checks) return makeModChecks(lhs,rhs,e->getPos());
      return new ValPtr(Builder.CreateSRem(lhs->getValue(), rhs->getValue()));

    case BINOP_EQ:
      return new ValPtr(Builder.CreateICmpEQ(lhs->getValue(), rhs->getValue()));
    case BINOP_NE:
      return new ValPtr(Builder.CreateICmpNE(lhs->getValue(), rhs->getValue()));
    case BINOP_LT:
      return new ValPtr(Builder.CreateICmpSLT(lhs->getValue(), rhs->getValue()));
    case BINOP_LE:
      return new ValPtr(Builder.CreateICmpSLE(lhs->getValue(), rhs->getValue()));
    case BINOP_GT:
      return new ValPtr(Builder.CreateICmpSGT(lhs->getValue(), rhs->getValue()));
    case BINOP_GE:
      return new ValPtr(Builder.CreateICmpSGE(lhs->getValue(), rhs->getValue()));
    default:
      throw error::scanner("Unknown binary op type");
  }
  return NULL;
}

Expr *Compiler::scan(If *e, valmap &out) {
  // Calculate condition
  ValPtr *cnd = (ValPtr *) Scanner::run(e->getCnd(), out);
  // Setup blocks
  Function *F = Builder.GetInsertBlock()->getParent();
  BasicBlock *thnB = BasicBlock::Create(calcc::C, "thenIf", F);
  BasicBlock *elsB = BasicBlock::Create(calcc::C, "elseIf", F);
  BasicBlock *aftB = BasicBlock::Create(calcc::C, "afterIf", F);
  // Create conditional branch
  Builder.CreateCondBr(cnd->getValue(), thnB, elsB);
  // Create PHIs
  Builder.SetInsertPoint(aftB);
  std::map<VDecl *, PHINode *> phimap;
  std::map<VDecl *, ValPtr *> orig;
  for (auto it = e->getMod().begin(); it != e->getMod().end(); ++it) {
    PHINode *phi = Builder.CreatePHI(toType((*it)->getValType()), 2);
    phimap[*it] = phi;
    orig[*it] = (*it)->getVPtr();
  }
  PHINode *ret = Builder.CreatePHI(toType(e->getValType()), 2);
  // Then
  Builder.SetInsertPoint(thnB);
  ValPtr *thn = (ValPtr *) Scanner::run(e->getThn(), out);
  Builder.CreateBr(aftB);
  thnB = Builder.GetInsertBlock();
  for (auto it = e->getMod().begin(); it != e->getMod().end(); ++it) {
    phimap[*it]->addIncoming((*it)->getVPtr()->getValue(), thnB);
    (*it)->setVPtr(orig[*it]);
  }
  ret->addIncoming(thn->getValue(), thnB);
  // Else
  Builder.SetInsertPoint(elsB);
  ValPtr *els = (ValPtr *) Scanner::run(e->getEls(), out);
  Builder.CreateBr(aftB);
  elsB = Builder.GetInsertBlock();
  for (auto it = e->getMod().begin(); it != e->getMod().end(); ++it) {
    phimap[*it]->addIncoming((*it)->getVPtr()->getValue(), elsB);
    (*it)->setVPtr(new ValPtr(phimap[*it]));
  }
  ret->addIncoming(els->getValue(), elsB);
  // Set insert point to after
  Builder.SetInsertPoint(aftB);
  return new ValPtr(ret);
}

Expr *Compiler::scan(Ref *e, valmap &out) {
  Decl *d = e->getDecl();
  if (d->getExprType() == EXPR_FDECL)
    throw error::scanner("Can't refer to a FDecl yet, Function name: " + e->getName());
  VDecl *vd = (VDecl *) d;
  return vd->getVPtr();
}

Expr *Compiler::scan(ValPtr *e, valmap &out) {
  return e;
}

Expr *Compiler::scan(VDecl *e, valmap &out) {
  throw error::scanner("Error: trying to compile a VDecl: " + e->getName());
  return e;
}

ast::Expr *Compiler::scan(ast::Seq *e, valmap &out) {
  Scanner::run(e->getLHS(), out);
  ValPtr *v = (ValPtr *) Scanner::run(e->getRHS(), out);
  return v;
}

ast::Expr *Compiler::scan(ast::Set *e, valmap &out) {
  ValPtr *v = (ValPtr *) Scanner::run(e->getExpr(), out);
  VDecl *d = static_cast<VDecl *>(static_cast<Ref *>(e->getRef())->getDecl());
  d->setVPtr(v);
  return v;
}

ast::Expr *Compiler::scan(ast::While *e, valmap &out) {
  Function *F = Builder.GetInsertBlock()->getParent();
  BasicBlock *entB = Builder.GetInsertBlock();
  BasicBlock *cndB = BasicBlock::Create(calcc::C, "CondW", F);
  BasicBlock *bdyB = BasicBlock::Create(calcc::C, "BodyW", F);
  BasicBlock *aftB = BasicBlock::Create(calcc::C, "AfterW", F);
  Builder.CreateBr(cndB);
  // Cond block
  Builder.SetInsertPoint(cndB);
  // PHIs from entry
  std::map<VDecl *, PHINode *> phimap;
  for (auto it = e->getMod().begin(); it != e->getMod().end(); ++it) {
    PHINode *phi = Builder.CreatePHI(toType((*it)->getValType()), 2);
    phi->addIncoming((*it)->getVPtr()->getValue(), entB);
    (*it)->setVPtr(new ValPtr(phi));
    phimap[*it] = phi;
  }
  // return value
  PHINode *ret = Builder.CreatePHI(toType(e->getValType()), 2);
  ret->addIncoming(llvm::ConstantInt::get(toType(e->getValType()), 0), entB);
  // Condition
  ValPtr *cnd = (ValPtr *) Scanner::run(e->getCnd(), out);
  std::map<VDecl *, ValPtr *> orig;
  Builder.CreateCondBr(cnd->getValue(), bdyB, aftB);
  for (auto it = e->getMod().begin(); it != e->getMod().end(); ++it) {
    orig[*it] = (*it)->getVPtr();
  }
  // Body
  Builder.SetInsertPoint(bdyB);
  ValPtr *bdy = (ValPtr *) Scanner::run(e->getBdy(), out);
  Builder.CreateBr(cndB);
  bdyB = Builder.GetInsertBlock();
  // PHIs from body
  for (auto it = e->getMod().begin(); it != e->getMod().end(); ++it) {
    phimap[*it]->addIncoming((*it)->getVPtr()->getValue(), bdyB);
    (*it)->setVPtr(orig[*it]);
  }
  // return value
  ret->addIncoming(bdy->getValue(), bdyB);
  // Set insert point to after
  Builder.SetInsertPoint(aftB);
  return new ValPtr(ret);
}

ast::Expr *Compiler::scan(ast::VScope *e, valmap &out) {
  e->getDecl()->setVPtr(new ValPtr(llvm::ConstantInt::get(toType(e->getDecl()->getValType()), 0)));
  Expr *ret = run(e->getExpr(), out);
  return ret;
}

void Compiler::run_init(ast::Expr *e) {
  calcc::tools::valmap vmap;
  if (checks) {
    std::vector<Type*> retFields;
    retFields.push_back(toType(VAL_INT));
    retFields.push_back(toType(VAL_BOOL));
    auto retType = StructType::get(C,retFields);
    std::vector<Type*> args;
    args.push_back(toType(VAL_INT));
    args.push_back(toType(VAL_INT));
    FunctionType *ft = FunctionType::get(retType, args, false);
    FunctionType *tt = FunctionType::get(Type::getVoidTy(C), vector<Type*>(1, toType(VAL_INT)), false);
    sadd = Function::Create(ft, Function::ExternalLinkage, "llvm.sadd.with.overflow.i64", &*calcc::M);
    ssub = Function::Create(ft, Function::ExternalLinkage, "llvm.ssub.with.overflow.i64", &*calcc::M);
    smul = Function::Create(ft, Function::ExternalLinkage, "llvm.smul.with.overflow.i64", &*calcc::M);
    trap = Function::Create(tt, Function::ExternalLinkage, "overflow_fail", &*calcc::M);
  }
  run(e, vmap);
}
//
// Created by joe on 10/19/16.
//

#ifndef CALCCOMPILER_CLACAST_H
#define CALCCOMPILER_CLACAST_H

#include <string>
#include <vector>
#include "llvm/ADT/APInt.h"
#include "llvm/IR/Value.h"

namespace calcc {
  namespace ast {
    enum EXPR_TYPE {
      EXPR_FDECL,
      EXPR_VDECL,
      EXPR_BINOP,
      EXPR_IF,
      EXPR_REF,
      EXPR_INT,
      EXPR_VALP
    };
    enum VAL_TYPE {
      VAL_UNKNOWN,
      VAL_INT,
      VAL_BOOL
    };
    enum BINOP_TYPE {
      BINOP_PLUS,
      BINOP_MINUS,
      BINOP_MULT,
      BINOP_DIV,
      BINOP_MOD,
      // COMPARISON OPS
      BINOP_EQ = 16,
      BINOP_NE,
      BINOP_LT,
      BINOP_LE,
      BINOP_GT,
      BINOP_GE
    };
    class Expr {
    public:
      virtual EXPR_TYPE getExprType() = 0;
      virtual VAL_TYPE getValType() = 0;
    };
    class ValPtr : public Expr {
    private:
      llvm::Value* val;
    public:
      ValPtr(llvm::Value* ival) : val(ival) { }
      virtual EXPR_TYPE getExprType() {return EXPR_VALP;}
      virtual VAL_TYPE getValType() {return VAL_UNKNOWN;}
      llvm::Value* getValue() {return val;}
    };
    class Decl : public Expr {
    private:
      std::string name;
      VAL_TYPE vt;
    public:
      Decl(std::string iname, VAL_TYPE ivt) : name(iname), vt(ivt) { }
      virtual VAL_TYPE getValType() {return vt;}
      std::string getName() {return name;}
    };
    class VDecl : public Decl {
    private:
      ValPtr *vptr;
    public:
      VDecl(std::string iname, VAL_TYPE ivt) : Decl(iname,ivt) { }
      virtual EXPR_TYPE getExprType() {return EXPR_VDECL;}
      void setVPtr(ValPtr* vp) { vptr = vp; }
      ValPtr* getVPtr() { return vptr; }
    };
    class FDecl : public Decl {
    private:
      std::vector<VDecl*> params;
      Expr* body;
    public:
      FDecl(std::string iname, VAL_TYPE ivt, std::vector<VDecl*> &iparams, Expr* ibody) : Decl(iname,ivt), params(iparams), body(ibody) { }
      virtual EXPR_TYPE getExprType() {return EXPR_FDECL;}
      Expr* getBody() { return body; }
      std::vector<VDecl*> getParams() {return params;}
    };
    class BinaryOp : public Expr {
    private:
      VAL_TYPE vt;
      BINOP_TYPE op;
      Expr *lhs, *rhs;
    public:
      BinaryOp(BINOP_TYPE iop, Expr *ilhs, Expr *irhs) : op(iop), lhs(ilhs), rhs(irhs) { vt=VAL_UNKNOWN; }
      virtual EXPR_TYPE getExprType() {return EXPR_BINOP;}
      virtual VAL_TYPE getValType();
      BINOP_TYPE getOP() {return op;}
      Expr* getLHS() {return lhs;}
      Expr* getRHS() {return rhs;}
    };
    class If : public Expr {
    private:
      VAL_TYPE vt;
      Expr *cnd, *thn, *els;
    public:
      If(Expr *icnd, Expr *ithn, Expr *iels) : cnd(icnd), thn(ithn), els(iels) { vt=VAL_UNKNOWN; }
      virtual EXPR_TYPE getExprType() {return EXPR_IF;}
      virtual VAL_TYPE getValType();
      Expr* getCnd() {return cnd;}
      Expr* getThn() {return thn;}
      Expr* getEls() {return els;}
    };
    class Ref : public Expr {
    private:
      std::string name;
      Decl* decl;
    public:
      Ref(std::string val) : name(val) { }
      virtual EXPR_TYPE getExprType() {return EXPR_REF;}
      virtual VAL_TYPE getValType() { return decl->getValType();};
      std::string getName() {return name;}
      void setDecl(Decl *d) { decl = d; }
      Decl* getDecl() { return decl; }
    };
    class IntLiteral : public Expr {
    private:
      llvm::APInt value;
      VAL_TYPE vt;
    public:
      IntLiteral(const llvm::APInt &val, VAL_TYPE ivt) : value(val), vt(ivt) { }
      virtual EXPR_TYPE getExprType() {return EXPR_INT;}
      virtual VAL_TYPE getValType() {return vt;}
      llvm::APInt getValue() {return value;}
    };
  }
}

#endif //CALCCOMPILER_CLACAST_H

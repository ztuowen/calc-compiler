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
    //! Expression(Node) type
    enum EXPR_TYPE {
      EXPR_FDECL,
      EXPR_VDECL,
      EXPR_BINOP,
      EXPR_IF,
      EXPR_REF,
      EXPR_INT,
      EXPR_VALP
    };
    //! Type of the expression's value
    enum VAL_TYPE {
      VAL_UNKNOWN,
      VAL_INT,
      VAL_BOOL
    };
    //! Binary operator's type
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
    //! Generic expression node that returns a value
    class Expr {
    public:
      virtual EXPR_TYPE getExprType() = 0;
      virtual VAL_TYPE getValType() = 0;
    };
    //! Pointer to an LLVM value
    class ValPtr : public Expr {
    private:
      //! the llvm value, only known when compiling
      llvm::Value* val;
    public:
      ValPtr(llvm::Value* ival) : val(ival) { }
      virtual EXPR_TYPE getExprType() {return EXPR_VALP;}
      virtual VAL_TYPE getValType() {return VAL_UNKNOWN;}
      llvm::Value* getValue() {return val;}
    };
    //! A declaration
    class Decl : public Expr {
    private:
      //! llvm value, binds when compiling
      ValPtr *vptr;
      std::string name;
      VAL_TYPE vt;
    public:
      Decl(std::string iname, VAL_TYPE ivt) : name(iname), vt(ivt) { }
      virtual VAL_TYPE getValType() {return vt;}
      std::string getName() {return name;}
      void setVPtr(ValPtr* vp) { vptr = vp; }
      ValPtr* getVPtr() { return vptr; }
    };
    //! A variable declaration
    class VDecl : public Decl {
    public:
      VDecl(std::string iname, VAL_TYPE ivt) : Decl(iname,ivt) { }
      virtual EXPR_TYPE getExprType() {return EXPR_VDECL;}
    };
    //! A function declaration
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
    //! Binary operator expression
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
    //! If expression
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
    //! A declaration reference pointer
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
    //! An integer/bool literal
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

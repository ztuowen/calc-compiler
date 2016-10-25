//
// Created by joe on 10/19/16.
//

#ifndef CALCCOMPILER_CLACAST_H
#define CALCCOMPILER_CLACAST_H

#include <string>
#include <vector>
#include <set>
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
      EXPR_VALP,
      EXPR_SET,
      EXPR_WHILE,
      EXPR_SEQ
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
    private:
      bool cnst;
    public:
      VDecl(std::string iname, VAL_TYPE ivt, bool icnst) : Decl(iname,ivt), cnst(icnst) { }
      virtual EXPR_TYPE getExprType() {return EXPR_VDECL;}
      bool isConst() { return cnst; }
    };
    //! A set of variables
    typedef std::set<ast::VDecl*> vset;
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
    class Set : public Expr {
    private:
      VAL_TYPE vt;
      Expr *e, *ref;
    public:
      Set(Expr *ie, Expr *iref) : e(ie), ref(iref) { vt = VAL_UNKNOWN; }
      virtual EXPR_TYPE getExprType() {return EXPR_SET;}
      virtual VAL_TYPE getValType();
      Expr* getExpr() { return e; }
      Expr* getRef() { return ref; }
    };
    class While : public Expr {
    private:
      VAL_TYPE vt;
      Expr *cnd, *bdy;
    public:
      While(Expr* icnd, Expr* ibdy) : cnd(icnd), bdy(ibdy) { vt = VAL_UNKNOWN; }
      virtual EXPR_TYPE getExprType() {return EXPR_WHILE;}
      virtual VAL_TYPE getValType();
      Expr* getCnd() { return cnd; }
      Expr* getBdy() { return bdy; }
    };
    class Seq : public Expr {
    private:
      VAL_TYPE vt;
      Expr *lhs, *rhs;
    public:
      Seq(Expr* l, Expr* r) : lhs(l), rhs(r) { vt = VAL_UNKNOWN; }
      virtual EXPR_TYPE getExprType() {return EXPR_SEQ;}
      virtual VAL_TYPE getValType();
      Expr* getLHS() { return lhs; }
      Expr* getRHS() { return rhs; }
    };
  }
}

#endif //CALCCOMPILER_CLACAST_H

//
// Created by joe on 10/20/16.
//

#include "calcc/AST.h"

using namespace calcc::ast;

VAL_TYPE If::getValType() {
  if (vt == VAL_UNKNOWN)
    vt = getThn()->getValType(); // No need to check correctness
  return vt;
}

VAL_TYPE BinaryOp::getValType() {
  if (vt == VAL_UNKNOWN) {
    if (op >= BINOP_EQ) vt = VAL_BOOL;
    else vt = VAL_INT;
  }
  return vt;
}

VAL_TYPE Set::getValType() {
  if (vt == VAL_UNKNOWN)
    vt = getExpr()->getValType();
  return vt;
}

VAL_TYPE While::getValType() {
  if (vt == VAL_UNKNOWN)
    vt = getBdy()->getValType();
  return vt;
}

VAL_TYPE Seq::getValType() {
  if (vt == VAL_UNKNOWN)
    vt = getRHS()->getValType();
  return vt;
}

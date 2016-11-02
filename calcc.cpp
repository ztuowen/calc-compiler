#include "llvm/ADT/APInt.h"
#include "llvm/IR/ConstantRange.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "calcc/AST.h"
#include "calcc/parser.h"
#include "calcc/error.h"
#include "calcc/tools/referrer.h"
#include "calcc/tools/checker.h"
#include "calcc/tools/dumper.h"
#include "calcc/tools/ssa.h"
#include "calcc/global_llvm.h"
#include "calcc/tools/compiler.h"

using namespace llvm;
using namespace std;

namespace calcc {
  LLVMContext C;
  IRBuilder<NoFolder> Builder(C);
  std::unique_ptr<Module> M = llvm::make_unique<Module>("calc", C);
  bool checks = false;
}

static int compile() {
  calcc::ast::Expr *astp;
  // parse the source program
  try {
    astp = calcc::parser::parse(cin);
  } catch (calcc::error::parser &e) {
    cout << e.what() << endl;
    return 1;
  }
  // Link identifier to definition
  try {
    calcc::tools::declmap d;
    calcc::tools::Referrer ref;
    ref.run(astp,d);
  } catch  (calcc::error::scanner &e) {
    cout << e.what() << endl;
    return 1;
  }
  // AST dump
  try {
    calcc::tools::Dumper dump;
    dump.run(astp, cout);
    cout.flush();
  } catch (calcc::error::scanner &e) {
    cout << e.what() << endl;
    return 1;
  }
  // Basic Type checking
  try {
    int d;
    calcc::tools::Checker chk;
    chk.run(astp,d);
  } catch  (calcc::error::scanner &e) {
    cout << e.what() << endl;
    return 1;
  }
  // SSA
  try {
    calcc::ast::vset v;
    calcc::tools::SSAfier ssa;
    ssa.run(astp,v);
  } catch  (calcc::error::scanner &e) {
    cout << e.what() << endl;
    return 1;
  }
  // From AST generate LLVM code
  calcc::M->setTargetTriple(llvm::sys::getProcessTriple());
  {
    calcc::tools::Compiler comp;
    comp.run_init(astp);
  }
  assert(!verifyModule(*calcc::M,&llvm::outs()));
  calcc::M->dump();
  return 0;
}

int main(int argc, char** argv) {
  // Parse command line
  if (argc > 1) {
    if (!strcmp(argv[1],"-check")) calcc::checks = true;
    else return 1;
    if (argc > 2) return 1;
  }
  return compile();
}

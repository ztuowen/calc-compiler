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
#include "calcc/asttools/referrer.h"
#include "calcc/asttools/checker.h"
#include "calcc/asttools/dumper.h"
#include "calcc/global_llvm.h"
#include "calcc/asttools/compiler.h"

using namespace llvm;
using namespace std;

namespace calcc {
  LLVMContext C;
  IRBuilder<true, NoFolder> Builder(C);
  std::unique_ptr<Module> M = llvm::make_unique<Module>("calc", C);
}

static int compile(char* fname) {

  // parse the source program
  calcc::ast::Expr *astp;
  try {
    ifstream fin;
    fin.open(fname);
    astp = calcc::parser::parse(fin);
    fin.close();
  } catch (calcc::error::parser &e) {
    cout << e.what() << endl;
    return 1;
  }
  // Link identifier to definition
  try {
    calcc::ast::declmap d;
    calcc::ast::Referrer ref;
    ref.run(astp,d);
  } catch  (calcc::error::scanner &e) {
    cout << e.what() << endl;
    return 1;
  }
  // AST dump
  try {
    calcc::ast::Dumper dump;
    dump.run(astp, cout);
    cout.flush();
  } catch (calcc::error::scanner &e) {
    cout << e.what() << endl;
    return 1;
  }
  // Basic Type checking
  try {
    int d;
    calcc::ast::Checker chk;
    chk.run(astp,d);
  } catch  (calcc::error::scanner &e) {
    cout << e.what() << endl;
    return 1;
  }
  // From AST generate LLVM code
  calcc::M->setTargetTriple(llvm::sys::getProcessTriple());
  {
    calcc::ast::valmap vmap;
    calcc::ast::Compiler comp;
    comp.run(astp,vmap);
  }
  assert(!verifyModule(*calcc::M,&llvm::outs()));
  calcc::M->dump();
  return 0;
}

int main(int argc, char** argv) {
    if (argc<2) {
        cout<<"Usage: "<<argv[0]<<" <Filename>"<<endl;
        return 1;
    }
    return compile(argv[1]);
}

//
// Created by joe on 10/21/16.
//

#ifndef CALCCOMPILER_GLOBAL_LLVM_H
#define CALCCOMPILER_GLOBAL_LLVM_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/NoFolder.h"

namespace calcc {
  extern llvm::LLVMContext C;
  extern llvm::IRBuilder<llvm::NoFolder> Builder;
  extern std::unique_ptr<llvm::Module> M;
}

#endif //CALCCOMPILER_GLOBAL_LLVM_H

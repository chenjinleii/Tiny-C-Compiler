//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_CODE_GEN_H
#define TINY_C_COMPILER_CODE_GEN_H

#include "ast.h"

#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

using SymbolTable=std::unordered_map<std::string, llvm::Value *>;

//class CodeGenBlock

class CodeGenContext {
public:
    llvm::LLVMContext the_context_;
    llvm::IRBuilder<> builder_;
    std::unique_ptr<llvm::Module> the_module_;
    SymbolTable global_vars_;
    //TypeSystem typeSystem;

    void GenerateCode(Block &root);

private:
    //std::vector<CodeGenBlock*> blockStack;
};

#endif //TINY_C_COMPILER_CODE_GEN_H
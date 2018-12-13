//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_CODE_GEN_H
#define TINY_C_COMPILER_CODE_GEN_H

#include "ast.h"
#include "type_system.h"

#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/LegacyPassManager.h>

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

namespace tcc {

using SymbolTable=std::unordered_map<std::string, llvm::AllocaInst *>;

struct CodeGenBlock {
    llvm::BasicBlock *block;
    llvm::Value *return_value;
    SymbolTable locals;
    std::unordered_map<std::string, std::shared_ptr<Type>> types;
    std::unordered_map<std::string, bool> is_func_arg;
};

class CodeGenContext {
public:
    CodeGenContext();

    void InitializeModuleAndPassManager();
    llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *parent, llvm::Type *type,
                                             const std::string &name);

    // 拥有许多核心LLVM数据结构,例如类型和常量值表
    llvm::LLVMContext the_context_;
    // 该实例跟踪插入指令的当前位置,并具有创建新指令的函数
    llvm::IRBuilder<> builder_;
    // 包含函数和全局变量,拥有我们生成的所有IR的内存
    std::unique_ptr<llvm::Module> the_module_;

    std::unique_ptr<llvm::legacy::FunctionPassManager> the_FPM_;
    SymbolTable global_vars_;
    TypeSystem type_system_;

    void GenerateCode(CompoundStatement &root);
    llvm::Value *GetSymbolValue(const std::string &name) const;
    std::shared_ptr<Type> GetSymbolType(const std::string &name) const;
    bool IsFuncArg(const std::string &name);
    void SetSymbolValue(const std::string &name, llvm::AllocaInst *value);
    void SetSymbolType(const std::string &name, std::shared_ptr<Type> type);
    void SetFuncArg(const std::string &name, bool value);
    llvm::BasicBlock *CurrentBlock() const;
    void PushBlock(llvm::BasicBlock *block);
    void PopBlock();
    void SetCurrentReturnValue(llvm::Value *value);
    llvm::Value *GetCurrentReturnValue();
private:
    std::vector<CodeGenBlock *> block_stack_;
};

}

#endif //TINY_C_COMPILER_CODE_GEN_H

//
// Created by kaiser on 18-12-8.
//

#include "code_gen.h"

#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>
#include <boost/range/adaptor/reversed.hpp>

#include <iostream>
#include <cstdlib>
#include <system_error>

namespace tcc {

void CodeGenContext::GenerateCode(CompoundStatement &root) {
    std::vector<llvm::Type *> system_args;
    auto main_func_type{llvm::FunctionType::get(llvm::Type::getVoidTy(the_context_),
                                                system_args, false)};

    llvm::Function::Create(main_func_type, llvm::Function::ExternalLinkage);

    auto block{llvm::BasicBlock::Create(the_context_, "entry")};
    PushBlock(block);
    root.CodeGen(*this);
    PopBlock();
}

llvm::Value *CodeGenContext::GetSymbolValue(const std::string &name) const {
    for (const auto &ele:block_stack_ | boost::adaptors::reversed) {
        if (ele->locals.find(name) != std::end(ele->locals)) {
            return ele->locals[name];
        }
    }
    return nullptr;
}

std::shared_ptr<Type> CodeGenContext::GetSymbolType(const std::string &name) const {
    for (const auto &ele : block_stack_ | boost::adaptors::reversed) {
        if (ele->types.find(name) != std::end(ele->types)) {
            return ele->types[name];
        }
    }
    return nullptr;
}

bool CodeGenContext::IsFuncArg(const std::string &name) {
    for (const auto &ele : block_stack_ | boost::adaptors::reversed) {
        if (ele->is_func_arg.find(name) != std::end(ele->is_func_arg)) {
            return ele->is_func_arg[name];
        }
    }
    return false;
}

void CodeGenContext::SetSymbolValue(const std::string &name, llvm::AllocaInst *value) {
    block_stack_.back()->locals[name] = value;
}

void CodeGenContext::SetSymbolType(const std::string &name, std::shared_ptr<Type> type) {
    block_stack_.back()->types[name] = type;
}

void CodeGenContext::SetFuncArg(const std::string &name, bool value) {
    block_stack_.back()->is_func_arg[name] = value;
}

llvm::BasicBlock *CodeGenContext::CurrentBlock() const {
    return block_stack_.back()->block;
}

void CodeGenContext::PushBlock(llvm::BasicBlock *block) {
    auto code_gen_block{new CodeGenBlock()};
    code_gen_block->block = block;
    code_gen_block->return_value = nullptr;
    block_stack_.push_back(code_gen_block);
}

void CodeGenContext::PopBlock() {
    auto code_gen_block{block_stack_.back()};
    block_stack_.pop_back();
    delete code_gen_block;
}

void CodeGenContext::SetCurrentReturnValue(llvm::Value *value) {
    block_stack_.back()->return_value = value;
}

llvm::Value *CodeGenContext::GetCurrentReturnValue() {
    return block_stack_.back()->return_value;
}

CodeGenContext::CodeGenContext() :
        builder_{the_context_}, type_system_{the_context_} {
    InitializeModuleAndPassManager();
}

void CodeGenContext::InitializeModuleAndPassManager() {
    the_module_ = std::make_unique<llvm::Module>("main", the_context_);
    the_FPM_ = std::make_unique<llvm::legacy::FunctionPassManager>(the_module_.get());

    // 优化
    the_FPM_->add(llvm::createInstructionCombiningPass());
    the_FPM_->add(llvm::createReassociatePass());
    the_FPM_->add(llvm::createGVNPass());
    the_FPM_->add(llvm::createCFGSimplificationPass());
    the_FPM_->add(llvm::createPromoteMemoryToRegisterPass());

    the_FPM_->doInitialization();
}

llvm::AllocaInst *CodeGenContext::CreateEntryBlockAlloca(llvm::Function *parent, llvm::Type *type,
                                                         const std::string &name) {
    // 在栈中分配内存,确保在函数的入口块处创建 alloca
    // 调用 begin() 表示指向入口块的第一条指令
    llvm::IRBuilder<> temp{&parent->getEntryBlock(),
                           parent->getEntryBlock().begin()};
    return temp.CreateAlloca(type, nullptr, name);

}

void CodeGenContext::Debug(CompoundStatement &root, const std::string &file_name) {
    std::vector<llvm::Type *> system_args;
    auto main_func_type{llvm::FunctionType::get(llvm::Type::getVoidTy(the_context_),
                                                system_args, false)};

    llvm::Function::Create(main_func_type, llvm::Function::ExternalLinkage);

    auto block{llvm::BasicBlock::Create(the_context_, "entry")};
    PushBlock(block);
    root.CodeGen(*this);
    PopBlock();

    std::error_code error;
    llvm::raw_fd_ostream fuck{file_name, error};
    if (error) {
        std::cerr << "can not open ir file\n";
        std::exit(EXIT_FAILURE);
    }
    fuck << *the_module_;
}

}

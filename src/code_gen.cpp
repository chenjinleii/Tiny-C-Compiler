//
// Created by kaiser on 18-12-8.
//

#include "code_gen.h"

#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/Support/raw_ostream.h>
#include <boost/range/adaptor/reversed.hpp>

namespace tcc {

void CodeGenContext::GenerateCode(CompoundStatement &root) {
    std::vector<llvm::Type *> system_args;
    auto main_func_type{llvm::FunctionType::get(llvm::Type::getVoidTy(the_context_),
                                                llvm::makeArrayRef(system_args), false)};

    llvm::Function::Create(main_func_type, llvm::Function::ExternalLinkage);

    auto block{llvm::BasicBlock::Create(the_context_, "entry")};
    PushBlock(block);
    root.CodeGen(*this);
    PopBlock();

    llvm::legacy::PassManager pass;
    pass.add(llvm::createPrintModulePass(llvm::outs()));
    pass.run(*the_module_);
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

void CodeGenContext::SetSymbolValue(const std::string &name, llvm::Value *value) {
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

}

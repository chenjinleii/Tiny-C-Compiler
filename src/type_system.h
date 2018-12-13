//
// Created by kaiser on 18-12-12.
//

#ifndef TCC_TYPE_SYSTEM_H
#define TCC_TYPE_SYSTEM_H

#include "ast.h"
#include "token.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/BasicBlock.h>

#include <string>
#include <unordered_map>

namespace tcc {

class TypeSystem {
public:
    explicit TypeSystem(llvm::LLVMContext &context);
    llvm::Value *GetDefaultValue(TokenValue type, llvm::LLVMContext &context);
    llvm::Value *Cast(llvm::Value *value, llvm::Type *type, llvm::BasicBlock *block);
    llvm::Type *GetVarType(const Type &type);
    llvm::LLVMContext &the_context_;
    llvm::Type *void_type_ = llvm::Type::getVoidTy(the_context_);
    llvm::Type *char_type_ = llvm::Type::getInt8Ty(the_context_);
    llvm::Type *int32_type_ = llvm::Type::getInt32Ty(the_context_);
    llvm::Type *double_type_ = llvm::Type::getDoubleTy(the_context_);
    llvm::Type *string_type_ = llvm::Type::getInt8PtrTy(the_context_);
    llvm::Type *bool_type_ = llvm::Type::getInt1Ty(the_context_);
private:
    void AddCast(llvm::Type *from, llvm::Type *to, llvm::CastInst::CastOps op);
    std::unordered_map<llvm::Type *, std::unordered_map<llvm::Type *, llvm::CastInst::CastOps>> cast_table_;

};

}

#endif //TCC_TYPE_SYSTEM_H

//
// Created by kaiser on 18-12-12.
//

#include "type_system.h"
#include "error.h"

#include <llvm/IR/Constants.h>

namespace tcc {

TypeSystem::TypeSystem(llvm::LLVMContext &context) : the_context_{context} {
    AddCast(int32_type_, double_type_, llvm::CastInst::SIToFP);
    AddCast(double_type_, int32_type_, llvm::CastInst::FPToSI);
    AddCast(bool_type_, double_type_, llvm::CastInst::SIToFP);
}

llvm::Value *TypeSystem::GetDefaultValue(TokenValue type, llvm::LLVMContext &context) {
    if (type == TokenValue::kInt) {
        llvm::ConstantInt::get(context, llvm::APInt(32, 0));
    } else if (type == TokenValue::kDouble) {
        llvm::ConstantFP::get(context, llvm::APFloat(0.0));
    }
    return nullptr;
}

llvm::Value *TypeSystem::Cast(llvm::Value *value, llvm::Type *type, llvm::BasicBlock *block) {
    auto from{value->getType()};
    if (from == type) {
        return value;
    }

    if (cast_table_.find(from) == std::end(cast_table_)) {
        ErrorReportAndExit("Type has no cast");
        return value;
    }

    if (cast_table_[from].find(type) == std::end(cast_table_[from])) {
        ErrorReportAndExit("Unable to cast");
        return value;
    }

    return llvm::CastInst::Create(cast_table_[from][type], value, type, "cast", block);
}

void TypeSystem::AddCast(llvm::Type *from, llvm::Type *to, llvm::CastInst::CastOps op) {
    cast_table_[from][to] = op;
}

llvm::Type *TypeSystem::GetVarType(const Type &type) {
    switch (type.type_) {
        case TokenValue::kInt:return int32_type_;
        case TokenValue::kChar:return char_type_;
        case TokenValue::kDouble:return double_type_;
        case TokenValue::kVoid:return void_type_;
        case TokenValue::kStringLiteral:return string_type_;
        default:return nullptr;
    }
}

}

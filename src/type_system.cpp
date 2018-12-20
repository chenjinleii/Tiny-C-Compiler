//
// Created by kaiser on 18-12-12.
//

#include "type_system.h"
#include "code_gen.h"
#include "error.h"

#include <llvm/IR/Constants.h>

namespace tcc {

TypeSystem::TypeSystem(llvm::LLVMContext &context) : the_context_{context} {
  AddCast(int32_type_, double_type_, llvm::CastInst::SIToFP);
  AddCast(double_type_, int32_type_, llvm::CastInst::FPToSI);
  AddCast(bool_type_, double_type_, llvm::CastInst::SIToFP);
}

llvm::Value *TypeSystem::Cast(llvm::Value *value, llvm::Type *type,
                              llvm::BasicBlock *block) {
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

  return llvm::CastInst::Create(cast_table_[from][type], value, type, "cast",
                                block);
}

llvm::Type *TypeSystem::GetType(const Type &type) {
  switch (type.type_) {
    case TokenValue::kInt:return int32_type_;
    case TokenValue::kChar:return char_type_;
    case TokenValue::kDouble:return double_type_;
    case TokenValue::kVoid:return void_type_;
    case TokenValue::kStringLiteral:return string_type_;
    default:return nullptr;
  }
}

llvm::Value *TypeSystem::CastToBool(CodeGenContext &context,
                                    llvm::Value *condition_value) {
  {
    if (condition_value->getType()->getTypeID() == llvm::Type::IntegerTyID) {
      condition_value = context.builder_.CreateIntCast(
          condition_value, llvm::Type::getInt1Ty(context.the_context_), true);
      return context.builder_.CreateICmpNE(
          condition_value,
          llvm::ConstantInt::get(llvm::Type::getInt1Ty(context.the_context_), 0,
                                 true));
    } else if (condition_value->getType()->getTypeID() ==
        llvm::Type::DoubleTyID) {
      return context.builder_.CreateFCmpONE(
          condition_value,
          llvm::ConstantFP::get(context.the_context_, llvm::APFloat(0.0)));
    } else {
      return condition_value;
    }
  }
}

void TypeSystem::AddCast(llvm::Type *from, llvm::Type *to,
                         llvm::CastInst::CastOps op) {
  cast_table_[from][to] = op;
}

}  // namespace tcc

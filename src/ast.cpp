//
// Created by kaiser on 18-12-8.
//

#include "ast.h"

llvm::Value *Double::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *Integer::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *String::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *IdentifierOrType::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *FunctionCall::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *BinaryOpExpression::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *Assignment::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *Block::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *ExpressionStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *VariableDeclaration::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *FunctionDeclaration::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *FunctionDefinition::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *IfStatenment::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *ForStatenment::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *ReturnStatenment::CodeGen(CodeGenContext &context) {
    return nullptr;
}
//
// Created by kaiser on 18-12-8.
//

#include "ast.h"

llvm::Value *ASTNode::CodeGen(CodeGenContext &context) {
    return nullptr;
}

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

llvm::Value *UnaryOpExpression::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *BinaryOpExpression::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *AssignmentExpression::CodeGen(CodeGenContext &context) {
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

llvm::Value *IfStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *WhileStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *ForStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *ReturnStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}
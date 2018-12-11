//
// Created by kaiser on 18-12-8.
//

#include "ast.h"

namespace tcc {

llvm::Value *ASTNode::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *DoubleConstant::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *IntConstant::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *CharConstant::CodeGen(CodeGenContext &context) {
    return ASTNode::CodeGen(context);
}

llvm::Value *StringLiteral::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *Identifier::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *FunctionCall::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *UnaryOpExpression::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *PostfixExpression::CodeGen(CodeGenContext &context) {
    return ASTNode::CodeGen(context);
}

llvm::Value *BinaryOpExpression::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *TernaryOpExpression::CodeGen(CodeGenContext &context) {
    return ASTNode::CodeGen(context);
}

llvm::Value *CompoundStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *ExpressionStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *Declaration::CodeGen(CodeGenContext &context) {
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

llvm::Value *EmptyStatement::CodeGen(CodeGenContext &context) {
    return ASTNode::CodeGen(context);
}

}

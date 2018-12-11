//
// Created by kaiser on 18-12-8.
//

#include "ast.h"

namespace tcc {

std::string ASTNodeTypes::ToString(ASTNodeTypes::Type type) {
    return QMetaEnum::fromType<ASTNodeTypes::Type>().valueToKey(type);
}

llvm::Value *ASTNode::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value Type::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    return root;
}

Json::Value PrimitiveType::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind()) +
            ' ' + TokenValues::ToString(type_);
    return root;
}

Json::Value Statement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    return root;
}

Json::Value CompoundStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    for (const auto &it : *statements_) {
        root["children"].append(it->JsonGen());
    }
    return root;
}

llvm::Value *CompoundStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value ExpressionStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    root["children"].append(expression_->JsonGen());
    return root;
}

llvm::Value *ExpressionStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value IfStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    root["children"].append(condition_->JsonGen());
    root["children"].append(then_block_->JsonGen());
    if (else_block_) {
        root["children"].append(else_block_->JsonGen());
    }
    return root;
}

llvm::Value *IfStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value WhileStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    root["children"].append(condition_->JsonGen());
    root["children"].append(block_->JsonGen());
    return root;
}

llvm::Value *WhileStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value ForStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());

    if (initial_) {
        root["children"].append(initial_->JsonGen());
    }
    if (condition_) {
        root["children"].append(condition_->JsonGen());
    }
    if (increment_) {
        root["children"].append(increment_->JsonGen());
    }

    return root;
}

llvm::Value *ForStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value ReturnStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    if (expression_) {
        root["children"].append(expression_->JsonGen());
    }

    return root;
}

llvm::Value *ReturnStatement::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value Declaration::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    if (type_) {
        root["children"].append(type_->JsonGen());
    }
    if (variable_name_) {
        root["children"].append(variable_name_->JsonGen());
    }

    if (initialization_expression_) {
        root["children"].append(initialization_expression_->JsonGen());
    }
    return root;
}

llvm::Value *Declaration::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value Expression::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    return root;
}

Json::Value UnaryOpExpression::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind()) + " "
            + TokenValues::ToString(op_);

    root["children"].append(object_->JsonGen());

    return root;
}

llvm::Value *UnaryOpExpression::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value PostfixExpression::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind()) + " "
            + TokenValues::ToString(op_);

    root["children"].append(object_->JsonGen());

    return root;
}

llvm::Value *PostfixExpression::CodeGen(CodeGenContext &context) {
    return ASTNode::CodeGen(context);
}

Json::Value BinaryOpExpression::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind()) + " "
            + TokenValues::ToString(op_);

    root["children"].append(lhs_->JsonGen());
    root["children"].append(rhs_->JsonGen());

    return root;
}

llvm::Value *BinaryOpExpression::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value Identifier::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind()) + " " + name_;
    return root;
}

llvm::Value *Identifier::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value FunctionCall::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    root["children"].append(function_name_->JsonGen());
    for (const auto &it : *args_) {
        root["children"].append(it->JsonGen());
    }
    return root;
}

llvm::Value *FunctionCall::CodeGen(CodeGenContext &context) {
    return nullptr;
}

ASTNodeType FunctionDeclaration::Kind() const {
    if (has_body_) {
        return ASTNodeType::kFunctionDefinition;
    } else {
        return ASTNodeType::kFunctionDeclaration;
    }
}

Json::Value FunctionDeclaration::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    root["children"].append(return_type_->JsonGen());
    root["children"].append(function_name_->JsonGen());

    if (args_) {
        for (auto &it : *args_) {
            root["children"].append(it->JsonGen());
        }
    }

    if (body_) {
        root["children"].append(body_->JsonGen());
    }

    return root;
}

llvm::Value *FunctionDeclaration::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value CharConstant::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind()) + " " + value_;
    return root;
}

llvm::Value *CharConstant::CodeGen(CodeGenContext &context) {
    return ASTNode::CodeGen(context);
}

Json::Value Int32Constant::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind())
            + " " + std::to_string(value_);

    return root;
}

llvm::Value *Int32Constant::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value DoubleConstant::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind())
            + " " + std::to_string(value_);
    return root;
}

llvm::Value *DoubleConstant::CodeGen(CodeGenContext &context) {
    return nullptr;
}

Json::Value StringLiteral::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind())
            + " " + value_;
    return root;
}

llvm::Value *StringLiteral::CodeGen(CodeGenContext &context) {
    return nullptr;
}

}

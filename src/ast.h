//
// Created by kaiser on 18-12-8.
//

//TODO enum struct array point switch以及其他内置类型

#ifndef TINY_C_COMPILER_AST_H
#define TINY_C_COMPILER_AST_H

#include "location.h"
#include "token.h"

#include <llvm/IR/Value.h>
#include <json/json.h>
#include <QMetaEnum>

#include <vector>
#include <memory>
#include <string>
#include <cstdint>

namespace tcc {

class CodeGenContext;
class Expression;
class Statement;
class Declaration;

using ExpressionList=std::vector<std::shared_ptr<Expression>>;
using StatementList=std::vector<std::shared_ptr<Statement>>;
using DeclarationList=std::vector<std::shared_ptr<Declaration>>;

enum class LinkAttribute {
    kNone,
    kExternal,
    kInternal,
};

class ASTNodeTypes : public QObject {
Q_OBJECT
public:
    enum Type {
        kASTNode,

        kStatement,
        kEmptyStatement,
        kExpressionStatement,
        kLabelStatement,
        kJumpStatement,
        kIfStatement,
        kWhileStatement,
        kForStatement,
        kReturnStatement,
        kCompoundStatement,
        kDeclaration,

        kExpression,
        kUnaryOpExpression,
        kPostfixExpression,
        kBinaryOpExpression,
        kAssignmentExpression,
        kTernaryOpExpression,
        kFunctionCall,

        kIdentifier,
        kFunctionDefinition,
        kFunctionDeclaration,

        kCharConstant,
        kIntConstant,
        kDoubleConstant,
        kStringLiteral,

        kType,
        kPrimitiveType,
    };
    Q_ENUM(Type)

    static std::string ToString(Type type) {
        return QMetaEnum::fromType<ASTNodeTypes::Type>().valueToKey(type);
    }
};

using ASTNodeType=ASTNodeTypes::Type;

class ASTNode {
public:
    ASTNode() = default;
    ASTNode(const std::string &name) : name_{name} {}
    virtual ~ASTNode() = default;
    virtual ASTNodeType Kind() const { return ASTNodeType::kASTNode; }
    virtual llvm::Value *CodeGen(CodeGenContext &context);
    virtual Json::Value jsonGen() const { return Json::Value(); }

    SourceLocation location_;
    std::string name_;
};

class Statement : public ASTNode {
public:
    virtual ASTNodeType Kind() const override { return ASTNodeType::kStatement; }
    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind());
        return root;
    }

};

class Expression : public ASTNode {
public:
    Expression() = default;
    Expression(const std::string &name) : ASTNode{name} {}
    virtual ASTNodeType Kind() const override { return ASTNodeType::kExpression; }
    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind());
        return root;
    }
};

class EmptyStatement : public Statement {
public:
    ASTNodeType Kind() const override { return ASTNodeType::kEmptyStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;
    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind());
        return root;
    }
};

class CompoundStatement : public Statement {
public:
    CompoundStatement() = default;
    explicit CompoundStatement(std::shared_ptr<StatementList> statements) :
            statements_{std::move(statements)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kCompoundStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<StatementList> statements_{std::make_unique<StatementList>()};
    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind());
        for (auto it = statements_->begin(); it != statements_->end(); it++) {
            root["children"].append((*it)->jsonGen());
        }
        return root;
    }
};

class IfStatement : public Statement {
public:
    IfStatement() = default;
    IfStatement(std::shared_ptr<Expression> condition_,
                std::shared_ptr<CompoundStatement> then_block,
                std::shared_ptr<CompoundStatement> else_block)
            : condition_{std::move(condition_)}, then_block_{std::move(then_block)},
              else_block_{std::move(else_block)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kIfStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> condition_;
    std::shared_ptr<CompoundStatement> then_block_;
    std::shared_ptr<CompoundStatement> else_block_;

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind());
        root["children"].append(condition_->jsonGen());
        root["children"].append(then_block_->jsonGen());
        if (else_block_) {
            root["children"].append(else_block_->jsonGen());
        }
        return root;
    }
};

class WhileStatement : public Statement {
public:
    WhileStatement() = default;
    WhileStatement(std::shared_ptr<Expression>
                   condition, std::shared_ptr<CompoundStatement> block) :
            condition_{std::move(condition)},
            block_{std::move(block)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kWhileStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> condition_;
    std::shared_ptr<CompoundStatement> block_;

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind());

        if (condition_) {
            root["children"].append(condition_->jsonGen());
        }
        return root;
    }
};

class ForStatement : public Statement {
public:
    ForStatement() = default;
    ForStatement(std::shared_ptr<Expression> initial,
                 std::shared_ptr<Expression> condition,
                 std::shared_ptr<Expression> increment,
                 std::shared_ptr<CompoundStatement> block) :
            initial_{std::move(initial)},
            condition_{std::move(condition)},
            increment_{std::move(increment)}, block_{std::move(block)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kForStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> initial_, condition_, increment_;
    std::shared_ptr<CompoundStatement> block_;

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind());

        if (initial_) {
            root["children"].append(initial_->jsonGen());
        }
        if (condition_) {
            root["children"].append(condition_->jsonGen());
        }
        if (increment_) {
            root["children"].append(increment_->jsonGen());
        }

        return root;
    }
};

class LabelStatement : public Statement {
public:
    LabelStatement() : label_{value_++} {}

    ASTNodeType Kind() const override { return ASTNodeType::kLabelStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    inline static std::int32_t value_{};
    std::int32_t label_{};
};

class JumpStatement : public Statement {
public:
    JumpStatement() = default;
    JumpStatement(std::shared_ptr<LabelStatement> label) :
            label_{std::move(label)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kJumpStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<LabelStatement> label_;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement() = default;
    explicit ReturnStatement(std::shared_ptr<Expression> expression) :
            expression_{std::move(expression)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kReturnStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> expression_;

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind());
        root["children"].append(expression_->jsonGen());
        return root;
    }
};

class Identifier : public Expression {
public:
    Identifier() = default;
    explicit Identifier(const std::string &name) : Expression{name} {}

    ASTNodeType Kind() const override { return ASTNodeType::kIdentifier; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind()) + " "
                + name_;
        return root;
    }
};

class Type : public ASTNode {
public:
    virtual bool IsPrimitive() const { return false; }
    virtual bool IsPoint() const { return false; }
    virtual ASTNodeType Kind() const override { return ASTNodeType::kType; }
};

class PrimitiveType : public Type {
public:
    PrimitiveType() = default;
    explicit PrimitiveType(TokenValue type) : type_{type} {}

    bool IsPrimitive() const override { return true; }
    ASTNodeType Kind() const override { return ASTNodeType::kPrimitiveType; }

    TokenValue type_{TokenValue::kNone};
};

class Declaration : public Statement {
public:
    Declaration() = default;
    Declaration(std::shared_ptr<Type> type,
                std::shared_ptr<Identifier> variable_name,
                std::shared_ptr<Expression> initialization_expression = nullptr) :
            type_{std::move(type)}, variable_name_{std::move(variable_name)},
            initialization_expression_{std::move(initialization_expression)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kDeclaration; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Type> type_;
    std::shared_ptr<Identifier> variable_name_;
    std::shared_ptr<Expression> initialization_expression_;

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind());
        root["children"].append(type_->jsonGen());
        root["children"].append(variable_name_->jsonGen());
        if (initialization_expression_ != nullptr) {
            root["children"].append(initialization_expression_->jsonGen());
        }
        return root;
    }
};

class DoubleConstant : public Expression {
public:
    DoubleConstant() = default;
    explicit DoubleConstant(double value) : value_{value} {}

    ASTNodeType Kind() const override { return ASTNodeType::kDoubleConstant; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    double value_{};

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind())
                + " " + std::to_string(value_);

        return root;
    }
};

class IntConstant : public Expression {
public:
    IntConstant() = default;
    explicit IntConstant(std::int32_t value) : value_{value} {}

    ASTNodeType Kind() const override { return ASTNodeType::kIntConstant; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::int32_t value_{};

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind())
                + " " + std::to_string(value_);

        return root;
    }
};

class CharConstant : public Expression {
public:
    CharConstant() = default;
    explicit CharConstant(char value) : value_{value} {}

    ASTNodeType Kind() const override { return ASTNodeType::kCharConstant; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    char value_{};

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind())
                + " " + value_;

        return root;
    }
};

class StringLiteral : public Expression {
public:
    StringLiteral() = default;
    explicit StringLiteral(const std::string &value) : value_{value} {}

    ASTNodeType Kind() const override { return ASTNodeType::kStringLiteral; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::string value_;

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind())
                + " " + value_;

        return root;
    }
};

class FunctionCall : public Expression {
public:
    FunctionCall() = default;
    explicit FunctionCall(std::shared_ptr<Identifier> function_name,
                          std::shared_ptr<ExpressionList> args = nullptr) :
            function_name_{std::move(function_name)}, args_{std::move(args)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kFunctionCall; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Identifier> function_name_;
    std::shared_ptr<ExpressionList> args_{std::make_unique<ExpressionList>()};

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind());
        root["children"].append(function_name_->jsonGen());
        for (auto it = args_->begin(); it != args_->end(); it++) {
            root["children"].append((*it)->jsonGen());
        }
        return root;
    }
};

class UnaryOpExpression : public Expression {
public:
    UnaryOpExpression() = default;
    UnaryOpExpression(std::shared_ptr<Expression> object, TokenValue op) :
            object_{std::move(object)}, op_{op} {}

    ASTNodeType Kind() const override { return ASTNodeType::kUnaryOpExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> object_;
    TokenValue op_{};
};

class PostfixExpression : public Expression {
public:
    PostfixExpression() = default;
    PostfixExpression(std::shared_ptr<Expression> object, TokenValue op) :
            object_{std::move(object)}, op_{op} {}

    ASTNodeType Kind() const override { return ASTNodeType::kPostfixExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> object_;
    TokenValue op_{};
};

class BinaryOpExpression : public Expression {
public:
    BinaryOpExpression() = default;
    BinaryOpExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs,
                       TokenValue op) : lhs_{std::move(lhs)}, rhs_{std::move(rhs)}, op_{op} {}

    ASTNodeType Kind() const override { return ASTNodeType::kBinaryOpExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> lhs_;
    std::shared_ptr<Expression> rhs_;
    TokenValue op_{};

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind()) + " "
                + TokenValues::ToString(op_);

        root["children"].append(lhs_->jsonGen());
        root["children"].append(rhs_->jsonGen());

        return root;
    }
};

class TernaryOpExpression : public Expression {
public:
    TernaryOpExpression() = default;
    TernaryOpExpression(std::shared_ptr<Expression> condition,
                        std::shared_ptr<Expression> lhs,
                        std::shared_ptr<Expression> rhs) :
            condition_{std::move(condition)}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kTernaryOpExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> condition_;
    std::shared_ptr<Expression> lhs_;
    std::shared_ptr<Expression> rhs_;
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement() = default;
    explicit ExpressionStatement(std::shared_ptr<Expression> expression) :
            expression_{std::move(expression)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kExpressionStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> expression_;

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind());
        root["children"].append(expression_->jsonGen());
        return root;
    }
};

class FunctionDeclaration : public Statement {
public:
    FunctionDeclaration() = default;
    FunctionDeclaration(std::shared_ptr<Type> return_type,
                        std::shared_ptr<Identifier> function_name,
                        std::shared_ptr<DeclarationList> args,
                        bool has_body,
                        std::shared_ptr<CompoundStatement> body)
            : return_type_{std::move(return_type)}, function_name_{std::move(function_name)},
              args_{std::move(args)}, has_body_{has_body},
              body_{std::move(body)} {}

    ASTNodeType Kind() const override {
        if (has_body_) {
            return ASTNodeType::kFunctionDefinition;
        } else {
            return ASTNodeType::kFunctionDeclaration;
        }
    }

    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Type> return_type_;
    std::shared_ptr<Identifier> function_name_;
    std::shared_ptr<DeclarationList> args_{std::shared_ptr<DeclarationList>()};
    bool has_body_{false};
    std::shared_ptr<CompoundStatement> body_;

    Json::Value jsonGen() const override {
        Json::Value root;
        root["name"] = ASTNodeTypes::ToString(Kind());
        root["children"].append(return_type_->jsonGen());
        root["children"].append(function_name_->jsonGen());

        for (auto it = args_->begin(); it != args_->end(); it++) {
            root["children"].append((*it)->jsonGen());
        }

        if (body_) {
            root["children"].append(body_->jsonGen());
        }

        return root;
    }
};

}

#endif //TINY_C_COMPILER_AST_H

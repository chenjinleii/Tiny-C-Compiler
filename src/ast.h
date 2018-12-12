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

class ASTNodeTypes : public QObject {
Q_OBJECT
public:
    enum Type {
        kASTNode,

        kType,
        kPrimitiveType,

        kStatement,
        kCompoundStatement,
        kExpressionStatement,
        kIfStatement,
        kWhileStatement,
        kForStatement,
        kReturnStatement,
        kDeclaration,

        kExpression,
        kUnaryOpExpression,
        kPostfixExpression,
        kBinaryOpExpression,
        kIdentifier,
        kFunctionCall,
        kFunctionDefinition,
        kFunctionDeclaration,

        kCharConstant,
        kInt32Constant,
        kDoubleConstant,
        kStringLiteral,
    };
    Q_ENUM(Type)

    static std::string ToString(Type type);
};

using ASTNodeType=ASTNodeTypes::Type;

class ASTNode {
public:
    ASTNode() = default;
    virtual ~ASTNode() = default;
    virtual ASTNodeType Kind() const { return ASTNodeType::kASTNode; }
    virtual Json::Value JsonGen() const { return Json::Value(); }
    // 该方法表示为该AST节点生成IR所依赖的所有内容
    // llvm::Value 是用于表示LLVM中SSA值的类
    // SSA内容见 https://en.wikipedia.org/wiki/Static_single_assignment_form
    virtual llvm::Value *CodeGen(CodeGenContext &);

    SourceLocation location_;
};

class Type : public ASTNode {
public:
    explicit Type(TokenValue type) : type_{type} {}
    virtual bool IsPrimitive() const { return false; }
    virtual bool IsPoint() const { return false; }
    ASTNodeType Kind() const override { return ASTNodeType::kType; }
    Json::Value JsonGen() const override;

    TokenValue type_{TokenValue::kNone};
};

class PrimitiveType : public Type {
public:
    PrimitiveType() = default;
    explicit PrimitiveType(TokenValue type) : Type{type} {}

    bool IsPrimitive() const override { return true; }
    ASTNodeType Kind() const override { return ASTNodeType::kPrimitiveType; }
    Json::Value JsonGen() const override;
};

class Statement : public ASTNode {
public:
    ASTNodeType Kind() const override { return ASTNodeType::kStatement; }
    Json::Value JsonGen() const override;
};

class CompoundStatement : public Statement {
public:
    CompoundStatement() = default;
    explicit CompoundStatement(std::shared_ptr<StatementList> statements) :
            statements_{std::move(statements)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kCompoundStatement; }
    void AddStatement(std::shared_ptr<Statement> statement) { statements_->push_back(std::move(statement)); }

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<StatementList> statements_{std::make_shared<StatementList>()};
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement() = default;
    explicit ExpressionStatement(std::shared_ptr<Expression> expression) :
            expression_{std::move(expression)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kExpressionStatement; }

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> expression_;
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

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> condition_;
    std::shared_ptr<CompoundStatement> then_block_;
    std::shared_ptr<CompoundStatement> else_block_;
};

class WhileStatement : public Statement {
public:
    WhileStatement() = default;
    WhileStatement(std::shared_ptr<Expression>
                   condition, std::shared_ptr<CompoundStatement> block) :
            condition_{std::move(condition)},
            block_{std::move(block)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kWhileStatement; }

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> condition_;
    std::shared_ptr<CompoundStatement> block_;
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

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> initial_, condition_, increment_;
    std::shared_ptr<CompoundStatement> block_;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement() = default;
    explicit ReturnStatement(std::shared_ptr<Expression> expression) :
            expression_{std::move(expression)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kReturnStatement; }

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> expression_;
};

class Identifier;

class Declaration : public Statement {
public:
    Declaration() = default;
    Declaration(std::shared_ptr<PrimitiveType> type,
                std::shared_ptr<Identifier> variable_name,
                std::shared_ptr<Expression> initialization_expression = nullptr) :
            type_{std::move(type)}, variable_name_{std::move(variable_name)},
            initialization_expression_{std::move(initialization_expression)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kDeclaration; }

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Type> type_;
    std::shared_ptr<Identifier> variable_name_;
    std::shared_ptr<Expression> initialization_expression_;
};

class Expression : public ASTNode {
public:
    ASTNodeType Kind() const override { return ASTNodeType::kExpression; }
    Json::Value JsonGen() const override;
};

class UnaryOpExpression : public Expression {
public:
    UnaryOpExpression() = default;
    UnaryOpExpression(std::shared_ptr<Expression> object, TokenValue op) :
            object_{std::move(object)}, op_{op} {}

    ASTNodeType Kind() const override { return ASTNodeType::kUnaryOpExpression; }

    Json::Value JsonGen() const override;
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

    Json::Value JsonGen() const override;
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

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Expression> lhs_;
    std::shared_ptr<Expression> rhs_;
    TokenValue op_{};
};

class Identifier : public Expression {
public:
    Identifier() = default;
    explicit Identifier(const std::string &name) : name_{name} {}

    ASTNodeType Kind() const override { return ASTNodeType::kIdentifier; }

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::string name_;
};

class FunctionCall : public Expression {
public:
    FunctionCall() = default;
    explicit FunctionCall(std::shared_ptr<Identifier> function_name,
                          std::shared_ptr<ExpressionList> args = nullptr) :
            function_name_{std::move(function_name)}, args_{std::move(args)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kFunctionCall; }
    void AddArg(std::shared_ptr<Expression> arg) { args_->push_back(std::move(arg)); }

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Identifier> function_name_;
    std::shared_ptr<ExpressionList> args_;
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

    ASTNodeType Kind() const override;
    void AddArg(std::shared_ptr<Declaration> arg) { args_->push_back(arg); }

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::shared_ptr<Type> return_type_;
    std::shared_ptr<Identifier> function_name_;
    std::shared_ptr<DeclarationList> args_;
    bool has_body_{false};
    std::shared_ptr<CompoundStatement> body_;
};

class CharConstant : public Expression {
public:
    CharConstant() = default;
    explicit CharConstant(char value) : value_{value} {}

    ASTNodeType Kind() const override { return ASTNodeType::kCharConstant; }

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    char value_{};
};

class Int32Constant : public Expression {
public:
    Int32Constant() = default;
    explicit Int32Constant(std::int32_t value) : value_{value} {}

    ASTNodeType Kind() const override { return ASTNodeType::kInt32Constant; }

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::int32_t value_{};
};

class DoubleConstant : public Expression {
public:
    DoubleConstant() = default;
    explicit DoubleConstant(double value) : value_{value} {}

    ASTNodeType Kind() const override { return ASTNodeType::kDoubleConstant; }

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    double value_{};
};

class StringLiteral : public Expression {
public:
    StringLiteral() = default;
    explicit StringLiteral(const std::string &value) : value_{value} {}

    ASTNodeType Kind() const override { return ASTNodeType::kStringLiteral; }

    Json::Value JsonGen() const override;
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::string value_;
};

}

#endif //TINY_C_COMPILER_AST_H

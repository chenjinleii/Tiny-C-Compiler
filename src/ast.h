//
// Created by kaiser on 18-12-8.
//

//TODO enum struct array point switch以及其他内置类型

#ifndef TINY_C_COMPILER_AST_H
#define TINY_C_COMPILER_AST_H

#include "location.h"
#include "token.h"

#include <llvm/IR/Value.h>

#include <vector>
#include <memory>
#include <string>
#include <cstdint>

namespace tcc {

class CodeGenContext;
class Expression;
class Statement;

using ExpressionList=std::vector<std::unique_ptr<Expression>>;
using StatementList=std::vector<std::unique_ptr<Statement>>;

enum class ASTNodeType {
    kASTNode,
    kExpression,
    kStatement,
    kType,

    kPrimitiveType,
    kPointerType,

    kCharConstant,
    kIntConstant,
    kDoubleConstant,
    kStringLiteral,

    kIdentifier,
    kFunctionCall,
    kCastExpression,
    kUnaryOpExpression,
    kPostfixExpression,
    kBinaryOpExpression,
    kTernaryOpExpression,
    kAssignmentExpression,

    kBlock,
    kExpressionStatement,
    kVariableDeclaration,
    kFunctionDeclaration,
    kFunctionDefinition,
    kIfStatement,
    kWhileStatement,
    kForStatement,
    kReturnStatement,
    kEmptyStatement
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual ASTNodeType Kind() const { return ASTNodeType::kASTNode; }
    virtual llvm::Value *CodeGen(CodeGenContext &context);

    SourceLocation location_;
};

class Expression : public ASTNode {
public:
    virtual ASTNodeType Kind() const override { return ASTNodeType::kExpression; }
};

class Statement : public ASTNode {
public:
    virtual ASTNodeType Kind() const override { return ASTNodeType::kStatement; }
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

class PointerType : public Type {
public:
    PointerType() = default;
    explicit PointerType(std::unique_ptr<Type> point_to) :
            point_to_{std::move(point_to)} {}

    bool IsPoint() const override { return true; }
    ASTNodeType Kind() const override { return ASTNodeType::kPointerType; }

    std::unique_ptr<Type> point_to_;
};

class DoubleConstant : public Expression {
public:
    DoubleConstant() = default;
    explicit DoubleConstant(double value) : value_{value} {}

    ASTNodeType Kind() const override { return ASTNodeType::kDoubleConstant; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    double value_{};
};

class IntConstant : public Expression {
public:
    IntConstant() = default;
    explicit IntConstant(std::int32_t value) : value_{value} {}

    ASTNodeType Kind() const override { return ASTNodeType::kIntConstant; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::int32_t value_{};
};

class CharConstant : public Expression {
public:
    CharConstant() = default;
    explicit CharConstant(char value) : value_{value} {}

    ASTNodeType Kind() const override { return ASTNodeType::kCharConstant; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    char value_{};
};

class StringLiteral : public Expression {
public:
    StringLiteral() = default;
    explicit StringLiteral(const std::string &value) : value_{value} {}

    ASTNodeType Kind() const override { return ASTNodeType::kStringLiteral; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::string value_;
};

class Identifier : public Expression {
public:
    Identifier() = default;
    explicit Identifier(const std::string &name) : name_{name} {}

    ASTNodeType Kind() const override { return ASTNodeType::kIdentifier; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::string name_;
};

class FunctionCall : public Expression {
public:
    FunctionCall() = default;
    explicit FunctionCall(std::unique_ptr<Identifier> function_name,
                          std::unique_ptr<ExpressionList> args = nullptr) :
            function_name_{std::move(function_name)}, args_{std::move(args)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kFunctionCall; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Identifier> function_name_;
    std::unique_ptr<ExpressionList> args_{std::make_unique<ExpressionList>()};
};

class CastExpression : public Expression {
public:
    CastExpression() = default;
    CastExpression(std::unique_ptr<Type> type,
                   std::unique_ptr<Identifier> name) :
            type_{std::move(type)}, name_{std::move(name)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kCastExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Type> type_;
    std::unique_ptr<Identifier> name_;
};

class UnaryOpExpression : public Expression {
public:
    UnaryOpExpression() = default;
    UnaryOpExpression(std::unique_ptr<Expression> object, TokenValue op) :
            object_{std::move(object)}, op_{op} {}

    ASTNodeType Kind() const override { return ASTNodeType::kUnaryOpExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> object_;
    TokenValue op_{};
};

class PostfixExpression : public Expression {
public:
    PostfixExpression() = default;
    PostfixExpression(std::unique_ptr<Expression> object, TokenValue op) :
            object_{std::move(object)}, op_{op} {}

    ASTNodeType Kind() const override { return ASTNodeType::kPostfixExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> object_;
    TokenValue op_{};
};

class BinaryOpExpression : public Expression {
public:
    BinaryOpExpression() = default;
    BinaryOpExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs,
                       TokenValue op) : lhs_{std::move(lhs)}, rhs_{std::move(rhs)}, op_{op} {}

    ASTNodeType Kind() const override { return ASTNodeType::kBinaryOpExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
    TokenValue op_{};
};

class TernaryOpExpression : public Expression {
public:
    TernaryOpExpression() = default;
    TernaryOpExpression(std::unique_ptr<Expression> condition,
                        std::unique_ptr<Expression> lhs,
                        std::unique_ptr<Expression> rhs) :
            condition_{std::move(condition)}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kTernaryOpExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
};

class AssignmentExpression : public Expression {
public:
    AssignmentExpression() = default;
    AssignmentExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) :
            lhs_{std::move(lhs)}, rhs_{std::move(lhs)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kAssignmentExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
};

class Block : public Expression {
public:
    Block() = default;
    explicit Block(std::unique_ptr<StatementList> statements) :
            statements_{std::move(statements)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kBlock; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<StatementList> statements_{std::make_unique<StatementList>()};
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement() = default;
    explicit ExpressionStatement(std::unique_ptr<Expression> expression) :
            expression_{std::move(expression)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kExpressionStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> expression_;
};

class VariableDeclaration : public Statement {
public:
    VariableDeclaration() = default;
    VariableDeclaration(std::unique_ptr<Type> type,
                        std::unique_ptr<Identifier> variable_name,
                        std::unique_ptr<Expression> initialization_expression = nullptr) :
            type_{std::move(type)}, variable_name_{std::move(variable_name)},
            initialization_expression_{std::move(initialization_expression)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kVariableDeclaration; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Type> type_;
    std::unique_ptr<Identifier> variable_name_;
    std::unique_ptr<Expression> initialization_expression_;
};

class VariableDeclarationList : public Statement {
public:
    VariableDeclarationList() = default;
    VariableDeclarationList(std::unique_ptr<std::vector<VariableDeclaration>> var_declarations) :
            var_declarations_{std::move(var_declarations)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kVariableDeclaration; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<std::vector<VariableDeclaration>> var_declarations_{
            std::make_unique<std::vector<VariableDeclaration>>()
    };
};

class FunctionDeclaration : public Statement {
public:
    FunctionDeclaration() = default;
    FunctionDeclaration(std::unique_ptr<Type> return_type,
                        std::unique_ptr<Identifier> function_name,
                        std::unique_ptr<VariableDeclarationList> args,
                        bool has_body,
                        std::unique_ptr<Block> body)
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

    std::unique_ptr<Type> return_type_;
    std::unique_ptr<Identifier> function_name_;
    std::unique_ptr<VariableDeclarationList> args_{std::unique_ptr<VariableDeclarationList>()};
    bool has_body_{false};
    std::unique_ptr<Block> body_;
};

class IfStatement : public Statement {
public:
    IfStatement() = default;
    IfStatement(std::unique_ptr<Expression> condition_,
                std::unique_ptr<Block> then_block,
                std::unique_ptr<Block> else_block)
            : condition_{std::move(condition_)}, then_block_{std::move(then_block)},
              else_block_{std::move(else_block)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kIfStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Block> then_block_;
    std::unique_ptr<Block> else_block_;
};

class WhileStatement : public Statement {
public:
    WhileStatement() = default;
    WhileStatement(std::unique_ptr<Expression>
                   condition, std::unique_ptr<Block> block) :
            condition_{std::move(condition)},
            block_{std::move(block)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kWhileStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Block> block_;
};

class ForStatement : public Statement {
public:
    ForStatement() = default;
    ForStatement(std::unique_ptr<Expression> initial,
                 std::unique_ptr<Expression> condition,
                 std::unique_ptr<Expression> increment,
                 std::unique_ptr<Block> block) :
            initial_{std::move(initial)},
            condition_{std::move(condition)},
            increment_{std::move(increment)}, block_{std::move(block)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kForStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> initial_, condition_, increment_;
    std::unique_ptr<Block> block_;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement() = default;
    explicit ReturnStatement(std::unique_ptr<Expression> expression) :
            expression_{std::move(expression)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kReturnStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> expression_;
};

class EmptyStatement : public Statement {
public:
    ASTNodeType Kind() const override { return ASTNodeType::kEmptyStatement; }
    llvm::Value *CodeGen(CodeGenContext &context) override;
};

}

#endif //TINY_C_COMPILER_AST_H

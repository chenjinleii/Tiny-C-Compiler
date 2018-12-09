//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_AST_H
#define TINY_C_COMPILER_AST_H

#include <llvm/IR/Value.h>

#include <vector>
#include <memory>
#include <string>
#include <cstdint>

class CodeGenContext;
class Expression;
class Statement;
class VariableDeclaration;

enum class ASTNodeType {
    kExpression,
    kStatement,
    kDouble,
    kInteger,
    kString,
    kIdentifierOrType,
    kFunctionCall,
    kBinaryOpExpression,
    kAssignment,
    kBlock,
    kExpressionStatement,
    kVariableDeclaration,
    kFunctionDeclaration,
    kFunctionDefinition,
    kIfStatenment,
    kForStatenmen,
    kReturnStatenment
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual ASTNodeType Kind() const = 0;
    virtual llvm::Value *CodeGen(CodeGenContext &context) = 0;
};

class Expression : public ASTNode {
public:
    ASTNodeType Kind() const override { return ASTNodeType::kExpression; }
};

class Statement : public ASTNode {
public:
    ASTNodeType Kind() const override { return ASTNodeType::kStatement; }
};

class Double : public Expression {
public:
    Double() = default;
    explicit Double(double value) : value_{value} {}
    ASTNodeType Kind() const override { return ASTNodeType::kDouble; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    double value_{};
};

class Integer : public Expression {
public:
    Integer() = default;
    explicit Integer(std::int32_t value) : value_{value} {}
    ASTNodeType Kind() const override { return ASTNodeType::kInteger; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::int32_t value_{};
};

class String : public Expression {
public:
    String() = default;
    explicit String(const std::string &value) : value_{value} {}
    ASTNodeType Kind() const override { return ASTNodeType::kString; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::string value_;
};

class IdentifierOrType : public Expression {
public:
    IdentifierOrType() = default;
    explicit IdentifierOrType(const std::string &name, bool is_type) :
            name_{name}, is_type_{is_type} {}
    ASTNodeType Kind() const override { return ASTNodeType::kIdentifierOrType; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::string name_;
    bool is_type_{false};
};

class FunctionCall : public Expression {
public:
    using ExpressionList=std::vector<std::unique_ptr<Expression>>;
    FunctionCall() = default;
    explicit FunctionCall(std::unique_ptr<IdentifierOrType> function_name,
                          std::unique_ptr<ExpressionList> args = nullptr) :
            function_name_{std::move(function_name)}, args_{std::move(args)} {}
    ASTNodeType Kind() const override { return ASTNodeType::kFunctionCall; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<IdentifierOrType> function_name_;
    std::unique_ptr<ExpressionList> args_;
};

class BinaryOpExpression : public Expression {
public:
    BinaryOpExpression() = default;
    BinaryOpExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs,
                       char op) : lhs_{std::move(lhs)}, rhs_{std::move(lhs)}, op_{op} {}
    ASTNodeType Kind() const override { return ASTNodeType::kBinaryOpExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
    char op_{};
};

class Assignment : public Expression {
public:
    Assignment() = default;
    Assignment(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) :
            lhs_{std::move(lhs)}, rhs_{std::move(lhs)} {}
    ASTNodeType Kind() const override { return ASTNodeType::kAssignment; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
};

class Block : public Expression {
public:
    using StatementList=std::vector<std::unique_ptr<Statement>>;
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
    VariableDeclaration(std::unique_ptr<IdentifierOrType> type,
                        std::unique_ptr<IdentifierOrType> variable_name,
                        std::unique_ptr<Expression> initialization_expression = nullptr) :
            type_{std::move(type)}, variable_name_{std::move(variable_name)},
            initialization_expression_{std::move(initialization_expression)} {}
    ASTNodeType Kind() const override { return ASTNodeType::kVariableDeclaration; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<IdentifierOrType> type_;
    std::unique_ptr<IdentifierOrType> variable_name_;
    std::unique_ptr<Expression> initialization_expression_;
};

class FunctionDeclaration : public Statement {
public:
    using VariableDeclarationList=std::vector<std::unique_ptr<VariableDeclaration>>;
    FunctionDeclaration() = default;
    FunctionDeclaration(std::unique_ptr<IdentifierOrType> return_type,
                        std::unique_ptr<IdentifierOrType> function_name,
                        std::unique_ptr<VariableDeclarationList> args)
            : return_type_{std::move(return_type)}, function_name_{std::move(function_name)},
              args_{std::move(args)} {}
    ASTNodeType Kind() const override { return ASTNodeType::kFunctionDeclaration; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<IdentifierOrType> return_type_;
    std::unique_ptr<IdentifierOrType> function_name_;
    std::unique_ptr<VariableDeclarationList> args_{std::unique_ptr<VariableDeclarationList>()};
    bool is_extern_{false};
};

class FunctionDefinition : public Statement {
public:
    using VariableDeclarationList=std::vector<std::unique_ptr<VariableDeclaration>>;
    FunctionDefinition() = default;
    FunctionDefinition(std::unique_ptr<IdentifierOrType> return_type,
                       std::unique_ptr<IdentifierOrType> function_name,
                       std::unique_ptr<VariableDeclarationList> args,
                       std::unique_ptr<Block> body)
            : return_type_{std::move(return_type)}, function_name_{std::move(function_name)},
              args_{std::move(args)}, body_{std::move(body)} {}
    ASTNodeType Kind() const override { return ASTNodeType::kFunctionDefinition; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<IdentifierOrType> return_type_;
    std::unique_ptr<IdentifierOrType> function_name_;
    std::unique_ptr<VariableDeclarationList> args_{std::unique_ptr<VariableDeclarationList>()};
    std::unique_ptr<Block> body_;
    bool is_extern_{false};
};

class IfStatenment : public Statement {
public:
    IfStatenment() = default;
    IfStatenment(std::unique_ptr<Expression> condition_,
                 std::unique_ptr<Block> then_block,
                 std::unique_ptr<Block> else_block)
            : condition_{std::move(condition_)}, then_block_{std::move(then_block)},
              else_block_{std::move(else_block)} {}
    ASTNodeType Kind() const override { return ASTNodeType::kIfStatenment; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Block> then_block_;
    std::unique_ptr<Block> else_block_;
};

class ForStatenment : public Statement {
public:
    ForStatenment() = default;
    ForStatenment(std::unique_ptr<Expression> initial,
                  std::unique_ptr<Expression> condition,
                  std::unique_ptr<Expression> increment,
                  std::unique_ptr<Block> block) :
            initial_{std::move(initial)},
            condition_{std::move(condition)},
            increment_{std::move(increment)}, block_{std::move(block)} {}
    ASTNodeType Kind() const override { return ASTNodeType::kForStatenmen; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> initial_, condition_, increment_;
    std::unique_ptr<Block> block_;
};

class ReturnStatenment : public Statement {
public:
    ReturnStatenment() = default;
    explicit ReturnStatenment(std::unique_ptr<Expression> expression) :
            expression_{std::move(expression)} {}
    ASTNodeType Kind() const override { return ASTNodeType::kReturnStatenment; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> expression_;
};

//TODO struct array enum while以及其他内置类型

#endif //TINY_C_COMPILER_AST_H
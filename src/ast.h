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

using ExpressionList=std::vector<std::unique_ptr<Expression>>;
using StatementList=std::vector<std::unique_ptr<Statement>>;
using VariableDeclarationList=std::vector<std::unique_ptr<VariableDeclaration>>;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual llvm::Value *CodeGen(CodeGenContext &context) = 0;
};

class Expression : public ASTNode {};

class Statement : public ASTNode {};

class Double : public Expression {
public:
    explicit Double(double value) : value_{value} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    double value_;
};

class Integer : public Expression {
public:
    explicit Integer(std::int32_t value) : value_{value} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::int32_t value_;
};

class String : public Expression {
public:
    explicit String(const std::string &value) : value_{value} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::string value_;
};

class IdentifierOrType : public Expression {
public:
    explicit IdentifierOrType(const std::string &name) : name_{name} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::string name_;
    bool is_type_{false};
};

class FunctionCall : public Expression {
public:
    explicit FunctionCall(std::unique_ptr<IdentifierOrType> function_name,
                          std::unique_ptr<ExpressionList> args = nullptr) :
            function_name_{std::move(function_name)}, args_{std::move(args)} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<IdentifierOrType> function_name_;
    std::unique_ptr<ExpressionList> args_;
};

class BinaryOpExpression : public Expression {
public:
    BinaryOpExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs,
                       char op) : lhs_{std::move(lhs)}, rhs_{std::move(lhs)}, op_{op} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
    char op_;
};

class Assignment : public BinaryOpExpression {
public:
    Assignment(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) :
            BinaryOpExpression{std::move(lhs), std::move(rhs), '='} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;
};

class Block : public Statement {
public:
    explicit Block(std::unique_ptr<StatementList> statements) :
            statements_{std::move(statements)} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<StatementList> statements_;
};

class ExpressionStatement : public Statement {
public:
    explicit ExpressionStatement(std::unique_ptr<Expression> expression) :
            expression_{std::move(expression)} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> expression_;
};

class VariableDeclaration : public Statement {
public:
    VariableDeclaration(std::unique_ptr<IdentifierOrType> type,
                        std::unique_ptr<IdentifierOrType> variable_name,
                        std::unique_ptr<Expression> initialization_expression = nullptr) :
            type_{std::move(type)}, variable_name_{std::move(variable_name)},
            initialization_expression_{std::move(initialization_expression)} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<IdentifierOrType> type_;
    std::unique_ptr<IdentifierOrType> variable_name_;
    std::unique_ptr<Expression> initialization_expression_;
};

class FunctionDeclaration : public Statement {
public:
    FunctionDeclaration(std::unique_ptr<IdentifierOrType> return_type,
                        std::unique_ptr<IdentifierOrType> function_name,
                        std::unique_ptr<VariableDeclarationList> args,
                        std::unique_ptr<Block> body)
            : return_type_{std::move(return_type)}, function_name_{std::move(function_name)},
              args_{std::move(args)}, body_{std::move(body)} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<IdentifierOrType> return_type_;
    std::unique_ptr<IdentifierOrType> function_name_;
    std::unique_ptr<VariableDeclarationList> args_;
    std::unique_ptr<Block> body_;
    bool is_extern_{false};
};

class IfStatenment : public Statement {
public:
    IfStatenment(std::unique_ptr<Expression> condition_,
                 std::unique_ptr<Block> then_block,
                 std::unique_ptr<Block> else_block)
            : condition_{std::move(condition_)}, then_block_{std::move(then_block)},
              else_block_{std::move(else_block)} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Block> then_block_;
    std::unique_ptr<Block> else_block_;
};

class ForStatenment : public Statement {
public:
    ForStatenment(std::unique_ptr<Expression> initial,
                  std::unique_ptr<Expression> condition,
                  std::unique_ptr<Expression> increment,
                  std::unique_ptr<Block> block) :
            initial_{std::move(initial)},
            condition_{std::move(condition)},
            increment_{std::move(increment)}, block_{std::move(block)} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> initial_, condition_, increment_;
    std::unique_ptr<Block> block_;
};

class ReturnStatenment : public Statement {
public:
    explicit ReturnStatenment(std::unique_ptr<Expression> expression) :
            expression_{std::move(expression)} {}
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> expression_;
};

//TODO struct array enum以及其他内置类型

#endif //TINY_C_COMPILER_AST_H
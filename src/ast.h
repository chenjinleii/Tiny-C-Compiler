//
// Created by kaiser on 18-12-8.
//

//TODO enum switch以及其他内置类型

#ifndef TINY_C_COMPILER_AST_H
#define TINY_C_COMPILER_AST_H

#include "location.h"

#include <llvm/IR/Value.h>

#include <vector>
#include <memory>
#include <string>
#include <cstdint>

namespace tcc {

class CodeGenContext;
class Expression;
class Statement;
class Identifier;
class VariableDeclaration;

using ExpressionList=std::vector<std::unique_ptr<Expression>>;
using StatementList=std::vector<std::unique_ptr<Statement>>;
using IdentifierList=std::vector<std::unique_ptr<Identifier>>;
using VariableDeclarationList=std::vector<std::unique_ptr<VariableDeclaration>>;

enum class ASTNodeType {
    kASTNode,
    kExpression,
    kStatement,
    kType,

    kPrimitiveType,
    kPointerType,
    kArrayType,

    kDouble,
    kInteger,
    kString,
    kIdentifier,
    kFunctionCall,
    kCastExpression,
    kUnaryOpExpression,
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

    kStructDeclaration,
    kStructMemberAccess,
    kStructMemberAssignment,
    kEnumDeclaration
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
    Type() = default;
    explicit Type(const std::string &type) : type_{type} {}

    virtual bool IsPrimitive() const { return false; }
    virtual bool IsPointer() const { return false; }
    virtual bool IsArray() const { return false; }
    virtual ASTNodeType Kind() const override { return ASTNodeType::kType; }

    std::string type_;
};

class PrimitiveType : public Type {
public:
    PrimitiveType() = default;
    explicit PrimitiveType(const std::string &type) : Type{type} {}

    bool IsPrimitive() const override { return true; }
    ASTNodeType Kind() const override { return ASTNodeType::kPrimitiveType; }
};

class PointerType : public Type {
public:
    PointerType() = default;
    explicit PointerType(std::unique_ptr<Type> point_to_type) :
            point_to_type_{std::move(point_to_type)} {}

    bool IsPointer() const override { return true; }
    ASTNodeType Kind() const override { return ASTNodeType::kPointerType; }

    std::unique_ptr<Type> point_to_type_;
};

class ArrayType : public Type {
public:
    ArrayType() = default;
    ArrayType(std::unique_ptr<Type> array_type, std::size_t size) :
            array_type_{std::move(array_type)}, size_{size} {}

    bool IsArray() const override { return true; }
    ASTNodeType Kind() const override { return ASTNodeType::kArrayType; }

    std::unique_ptr<Type> array_type_;
    std::size_t size_{};
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
    UnaryOpExpression(std::unique_ptr<Expression> object, char op) :
            object_{std::move(object)}, op_{op} {}

    ASTNodeType Kind() const override { return ASTNodeType::kUnaryOpExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> object_;
    char op_{};
};

class BinaryOpExpression : public Expression {
public:
    BinaryOpExpression() = default;
    BinaryOpExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs,
                       char op) : lhs_{std::move(lhs)}, rhs_{std::move(rhs)}, op_{op} {}

    ASTNodeType Kind() const override { return ASTNodeType::kBinaryOpExpression; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
    char op_{};
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

class FunctionDeclaration : public Statement {
public:
    FunctionDeclaration() = default;
    FunctionDeclaration(std::unique_ptr<Type> return_type,
                        std::unique_ptr<Identifier> function_name,
                        std::unique_ptr<VariableDeclarationList> args,
                        bool has_body,
                        std::unique_ptr<Block> body,
                        bool is_extern = true)
            : return_type_{std::move(return_type)}, function_name_{std::move(function_name)},
              args_{std::move(args)}, has_body_{has_body},
              body_{std::move(body)}, is_extern_{is_extern} {}

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
    bool is_extern_{true};
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

class StructDeclaration : public Statement {
public:
    StructDeclaration() = default;
    StructDeclaration(std::unique_ptr<Identifier> name,
                      std::unique_ptr<VariableDeclarationList> members) :
            name_{std::move(name)}, members_{std::move(members)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kStructDeclaration; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Identifier> name_;
    std::unique_ptr<VariableDeclarationList> members_{std::make_unique<VariableDeclarationList>()};
};

class StructMemberAccess : public Expression {
public:
    StructMemberAccess() = default;

    StructMemberAccess(std::unique_ptr<Identifier> name,
                       std::unique_ptr<Identifier> member) :
            name_{std::move(name)}, member_{std::move(member)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kStructMemberAccess; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Identifier> name_;
    std::unique_ptr<Identifier> member_;
};

class StructMemberAssignment : public Expression {
public:
    StructMemberAssignment() = default;
    StructMemberAssignment(std::unique_ptr<StructMemberAccess> member,
                           std::unique_ptr<Expression> expression) :
            member_{std::move(member)}, expression_{std::move(expression)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kStructMemberAssignment; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<StructMemberAccess> member_;
    std::unique_ptr<Expression> expression_;
};

class EnumDeclaration : public Statement {
public:
    EnumDeclaration() = default;
    EnumDeclaration(std::unique_ptr<Identifier> name,
         std::unique_ptr<IdentifierList>) :
            name_{std::move(name)}, member_{std::move(member_)} {}

    ASTNodeType Kind() const override { return ASTNodeType::kEnumDeclaration; }
    llvm::Value *CodeGen(CodeGenContext &context) override;

    std::unique_ptr<Identifier> name_;
    std::unique_ptr<IdentifierList> member_;
};

}

#endif //TINY_C_COMPILER_AST_H

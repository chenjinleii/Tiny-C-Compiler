//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_AST_H
#define TINY_C_COMPILER_AST_H

#include "location.h"
#include "token.h"

#include <llvm/IR/Value.h>
#include <QtCore/QMetaEnum>
#include <QtCore/QJsonObject>
#include <QtCore/QString>

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace tcc {

class CodeGenContext;
class Expression;
class Statement;
class Declaration;

using ExpressionList = std::vector<std::shared_ptr<Expression>>;
using StatementList = std::vector<std::shared_ptr<Statement>>;
using DeclarationList = std::vector<std::shared_ptr<Declaration>>;

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

  static QString ToString(Type type);
};

using ASTNodeType = ASTNodeTypes::Type;

class ASTNode {
 public:
  ASTNode() = default;
  virtual ~ASTNode() = default;
  virtual ASTNodeType Kind() const;
  virtual QJsonObject JsonGen() const;
  // 该方法表示为该AST节点生成IR所依赖的所有内容
  // llvm::Value 是用于表示LLVM中SSA值的类
  virtual llvm::Value *CodeGen(CodeGenContext &context);

  SourceLocation location_;
};

class Type : public ASTNode {
 public:
  Type() = default;
  explicit Type(TokenValue type);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;

  TokenValue type_{TokenValue::kNone};
};

class PrimitiveType : public Type {
 public:
  PrimitiveType() = default;
  explicit PrimitiveType(TokenValue type);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
};

class Statement : public ASTNode {
 public:
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
};

class CompoundStatement : public Statement {
 public:
  CompoundStatement() = default;
  explicit CompoundStatement(std::shared_ptr<StatementList> statements);
  ASTNodeType Kind() const override;
  void AddStatement(std::shared_ptr<Statement> statement);
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::shared_ptr<StatementList> statements_;
};

class ExpressionStatement : public Statement {
 public:
  ExpressionStatement() = default;
  explicit ExpressionStatement(std::shared_ptr<Expression> expression);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::shared_ptr<Expression> expression_;
};

class IfStatement : public Statement {
 public:
  IfStatement() = default;
  IfStatement(std::shared_ptr<Expression> cond_,
              std::shared_ptr<Statement> then_block,
              std::shared_ptr<Statement> else_block);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::shared_ptr<Expression> cond_;
  std::shared_ptr<Statement> then_block_;
  std::shared_ptr<Statement> else_block_;
};

class WhileStatement : public Statement {
 public:
  WhileStatement() = default;
  WhileStatement(std::shared_ptr<Expression> cond,
                 std::shared_ptr<Statement> block);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::shared_ptr<Expression> cond_;
  std::shared_ptr<Statement> block_;
};

class ForStatement : public Statement {
 public:
  ForStatement() = default;
  ForStatement(std::shared_ptr<Expression> init,
               std::shared_ptr<Expression> cond,
               std::shared_ptr<Expression> increment,
               std::shared_ptr<Statement> block,
               std::shared_ptr<Declaration> declaration);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::shared_ptr<Expression> init_, cond_, increment_;
  std::shared_ptr<Statement> block_;
  std::shared_ptr<Declaration> declaration_;
};

class ReturnStatement : public Statement {
 public:
  ReturnStatement() = default;
  explicit ReturnStatement(std::shared_ptr<Expression> expression);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::shared_ptr<Expression> expression_;
};

class Identifier;

class Declaration : public Statement {
 public:
  Declaration() = default;
  Declaration(std::shared_ptr<PrimitiveType> type,
              std::shared_ptr<Identifier> name,
              std::shared_ptr<Expression> init = nullptr);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::shared_ptr<Type> type_;
  std::shared_ptr<Identifier> name_;
  std::shared_ptr<Expression> init_;
};

class Expression : public ASTNode {
 public:
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
};

class UnaryOpExpression : public Expression {
 public:
  UnaryOpExpression() = default;
  UnaryOpExpression(std::shared_ptr<Expression> object, TokenValue op);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::shared_ptr<Expression> object_;
  TokenValue op_{};
};

class PostfixExpression : public Expression {
 public:
  PostfixExpression() = default;
  PostfixExpression(std::shared_ptr<Expression> object, TokenValue op);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::shared_ptr<Expression> object_;
  TokenValue op_{};
};

class BinaryOpExpression : public Expression {
 public:
  BinaryOpExpression() = default;
  BinaryOpExpression(std::shared_ptr<Expression> lhs,
                     std::shared_ptr<Expression> rhs, TokenValue op);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::shared_ptr<Expression> lhs_;
  std::shared_ptr<Expression> rhs_;
  TokenValue op_{};
};

class Identifier : public Expression {
 public:
  Identifier() = default;
  explicit Identifier(std::string name);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::string name_;
};

class FunctionCall : public Expression {
 public:
  FunctionCall() = default;
  explicit FunctionCall(std::shared_ptr<Identifier> name,
                        std::shared_ptr<ExpressionList> args = nullptr);
  ASTNodeType Kind() const override;
  void AddArg(std::shared_ptr<Expression> arg);
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::shared_ptr<Identifier> name_;
  std::shared_ptr<ExpressionList> args_;
};

class FunctionDeclaration : public Statement {
 public:
  FunctionDeclaration() = default;
  FunctionDeclaration(std::shared_ptr<Type> return_type,
                      std::shared_ptr<Identifier> name,
                      std::shared_ptr<DeclarationList> args,
                      std::shared_ptr<CompoundStatement> body);

  ASTNodeType Kind() const override;
  void AddArg(std::shared_ptr<Declaration> arg);

  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::shared_ptr<Type> return_type_;
  std::shared_ptr<Identifier> name_;
  std::shared_ptr<DeclarationList> args_;
  std::shared_ptr<Statement> body_;
};

class CharConstant : public Expression {
 public:
  CharConstant() = default;
  explicit CharConstant(char value);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  char value_{};
};

class Int32Constant : public Expression {
 public:
  Int32Constant() = default;
  explicit Int32Constant(std::int32_t value);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::int32_t value_{};
};

class DoubleConstant : public Expression {
 public:
  DoubleConstant() = default;
  explicit DoubleConstant(double value);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  double value_{};
};

class StringLiteral : public Expression {
 public:
  StringLiteral() = default;
  explicit StringLiteral(std::string value);
  ASTNodeType Kind() const override;
  QJsonObject JsonGen() const override;
  llvm::Value *CodeGen(CodeGenContext &context) override;

  std::string value_;
};

}  // namespace tcc

#endif  // TINY_C_COMPILER_AST_H

//
// Created by kaiser on 18-12-8.
//

#include "ast.h"
#include "code_gen.h"
#include "error.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Verifier.h>
#include <QtCore/QJsonArray>

#include <cassert>
#include <cstdint>
#include <vector>

namespace tcc {

QString ASTNodeTypes::ToString(ASTNodeTypes::Type type) {
  return QMetaEnum::fromType<ASTNodeTypes::Type>().valueToKey(type) + 1;
}

ASTNodeType ASTNode::Kind() const {
  return ASTNodeType::kASTNode;
}

QJsonObject ASTNode::JsonGen() const {
  return QJsonObject{};
}

llvm::Value *ASTNode::CodeGen(CodeGenContext &) {
  return nullptr;
}

Type::Type(TokenValue type) : type_{type} {}

ASTNodeType Type::Kind() const {
  return ASTNodeType::kType;
}

QJsonObject Type::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind());
  return root;
}

PrimitiveType::PrimitiveType(TokenValue type) : Type{type} {}

ASTNodeType PrimitiveType::Kind() const {
  return ASTNodeType::kPrimitiveType;
}

QJsonObject PrimitiveType::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind()).append(' ')
      .append(TokenTypes::ToString(type_));
  return root;
}

ASTNodeType Statement::Kind() const {
  return ASTNodeType::kStatement;
}

QJsonObject Statement::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind());
  return root;
}

CompoundStatement::CompoundStatement(std::shared_ptr<StatementList> statements)
    : statements_{std::move(statements)} {}

ASTNodeType CompoundStatement::Kind() const {
  return ASTNodeType::kCompoundStatement;
}

void CompoundStatement::AddStatement(std::shared_ptr<Statement> statement) {
  if (statements_) {
    statements_->push_back(std::move(statement));
  } else {
    statements_ = std::make_shared<StatementList>();
    statements_->push_back(std::move(statement));
  }
}

QJsonObject CompoundStatement::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind());
  QJsonArray children;

  if (statements_) {
    for (const auto &it : *statements_) {
      assert(it != nullptr);
      children.append(it->JsonGen());
    }
  }
  root["children"] = children;

  return root;
}

llvm::Value *CompoundStatement::CodeGen(CodeGenContext &context) {
  llvm::Value *last{};
  if (statements_) {
    for (auto &it : *statements_) {
      if (!it) {
        ErrorReportAndExit(location_, "The statement is empty.");
      }
      last = it->CodeGen(context);
    }
  }

  // 可能返回 nullptr
  return last;
}

ExpressionStatement::ExpressionStatement(std::shared_ptr<Expression> expression)
    : expression_{std::move(expression)} {}

ASTNodeType ExpressionStatement::Kind() const {
  return ASTNodeType::kExpressionStatement;
}

QJsonObject ExpressionStatement::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind());
  QJsonArray children;

  assert(expression_ != nullptr);
  children.append(expression_->JsonGen());
  root["children"] = children;

  return root;
}

llvm::Value *ExpressionStatement::CodeGen(CodeGenContext &context) {
  if (!expression_) {
    ErrorReportAndExit(location_, "The expression is empty.");
  }
  return expression_->CodeGen(context);
}

IfStatement::IfStatement(std::shared_ptr<Expression> cond_,
                         std::shared_ptr<Statement> then_block,
                         std::shared_ptr<Statement> else_block)
    : cond_{std::move(cond_)},
      then_block_{std::move(then_block)},
      else_block_{std::move(else_block)} {}

ASTNodeType IfStatement::Kind() const {
  return ASTNodeType::kIfStatement;
}

QJsonObject IfStatement::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind());
  QJsonArray children;

  assert(cond_ != nullptr);
  children.append(cond_->JsonGen());
  children.append(then_block_->JsonGen());
  if (else_block_) {
    children.append(else_block_->JsonGen());
  }
  root["children"] = children;

  return root;
}

llvm::Value *IfStatement::CodeGen(CodeGenContext &context) {
  if (!cond_) {
    ErrorReportAndExit(location_, "The condition of the if statement cannot be empty.");
  }

  auto cond_value{cond_->CodeGen(context)};
  if (!cond_value) {
    ErrorReportAndExit(location_, "If condition code generation failed.");
    return nullptr;
  }
  cond_value = context.type_system_.CastToBool(context, cond_value);

  auto parent_func{context.builder_.GetInsertBlock()->getParent()};
  auto then_block = llvm::BasicBlock::Create(context.the_context_, "if_then", parent_func);
  auto else_block = llvm::BasicBlock::Create(context.the_context_, "if_else");
  auto after_block = llvm::BasicBlock::Create(context.the_context_, "if_after");

  if (else_block_) {
    context.builder_.CreateCondBr(cond_value, then_block, else_block);
  } else {
    context.builder_.CreateCondBr(cond_value, then_block, after_block);
  }

  context.builder_.SetInsertPoint(then_block);
  context.PushBlock(then_block);

  // 可以为空
  then_block_->CodeGen(context);

  context.PopBlock();
  // 注意需要使用控制流指令例如 return/branch 来终止基本块
  context.builder_.CreateBr(after_block);

  if (else_block_) {
    parent_func->getBasicBlockList().push_back(else_block);
    context.builder_.SetInsertPoint(else_block);
    context.PushBlock(then_block);

    // 可以为空
    else_block_->CodeGen(context);

    context.PopBlock();
    context.builder_.CreateBr(after_block);
  }

  parent_func->getBasicBlockList().push_back(after_block);
  context.builder_.SetInsertPoint(after_block);

  return nullptr;
}

WhileStatement::WhileStatement(std::shared_ptr<Expression> condition, std::shared_ptr<Statement> block)
    : cond_{std::move(condition)}, block_{std::move(block)} {}

ASTNodeType WhileStatement::Kind() const {
  return ASTNodeType::kWhileStatement;
}

QJsonObject WhileStatement::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind());
  QJsonArray children;

  assert(cond_ != nullptr);
  children.append(cond_->JsonGen());
  children.append(block_->JsonGen());
  root["children"] = children;

  return root;
}

llvm::Value *WhileStatement::CodeGen(CodeGenContext &context) {
  if (!cond_) {
    ErrorReportAndExit(location_, "The condition of the while statement cannot be empty.");
  }

  llvm::Value *cond_value{cond_->CodeGen(context)};
  if (!cond_value) {
    ErrorReportAndExit(location_, "While condition code generation failed.");
    return nullptr;
  }
  cond_value = context.type_system_.CastToBool(context, cond_value);

  auto parent_func{context.builder_.GetInsertBlock()->getParent()};
  auto loop_block{llvm::BasicBlock::Create(context.the_context_, "while_loop", parent_func)};
  auto after_block{llvm::BasicBlock::Create(context.the_context_, "while_after")};

  context.builder_.CreateCondBr(cond_value, loop_block, after_block);
  context.builder_.SetInsertPoint(loop_block);

  context.PushBlock(loop_block);

  // 可以为空
  block_->CodeGen(context);

  context.PopBlock();

  cond_value = cond_->CodeGen(context);
  if (!cond_value) {
    ErrorReportAndExit(location_, "While condition code generation failed");
    return nullptr;
  }
  cond_value = context.type_system_.CastToBool(context, cond_value);

  context.builder_.CreateCondBr(cond_value, loop_block, after_block);

  parent_func->getBasicBlockList().push_back(after_block);
  context.builder_.SetInsertPoint(after_block);

  return nullptr;
}

ForStatement::ForStatement(std::shared_ptr<Expression> init,
                           std::shared_ptr<Expression> cond,
                           std::shared_ptr<Expression> increment,
                           std::shared_ptr<Statement> block,
                           std::shared_ptr<Declaration> declaration)
    : init_{std::move(init)},
      cond_{std::move(cond)},
      increment_{std::move(increment)},
      block_{std::move(block)},
      declaration_{std::move(declaration)} {}

ASTNodeType ForStatement::Kind() const {
  return ASTNodeType::kForStatement;
}

QJsonObject ForStatement::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind());
  QJsonArray children;

  if (init_) {
    children.append(init_->JsonGen());
  } else if (declaration_) {
    children.append(declaration_->JsonGen());
  }

  if (cond_) {
    children.append(cond_->JsonGen());
  }
  if (increment_) {
    children.append(increment_->JsonGen());
  }
  children.append(block_->JsonGen());

  root["children"] = children;

  return root;
}

/*
 *  for (1; 1; 1)
 *      putchar('*');
 *
 * 生成的 IR 为:
 *
 * entry:
 * br i1 true, label %for_loop, label %for_after
 *
 * for_loop:
 * %0 = call i32 @putchar(i8 42)
 * br i1 true, label %for_loop, label %for_after
 *
 * for_after:
 *
 */

llvm::Value *ForStatement::CodeGen(CodeGenContext &context) {
  auto parent_func{context.builder_.GetInsertBlock()->getParent()};
  auto loop_block{llvm::BasicBlock::Create(context.the_context_, "for_loop", parent_func)};
  auto after_block{llvm::BasicBlock::Create(context.the_context_, "for_after")};

  context.PushBlock(loop_block);

  // init cond increment 均可以为空
  if (init_) {
    init_->CodeGen(context);
  } else if (declaration_) {
    declaration_->CodeGen(context);
  }

  llvm::Value *cond_value{};
  if (cond_) {
    cond_value = cond_->CodeGen(context);
    cond_value = context.type_system_.CastToBool(context, cond_value);
    context.builder_.CreateCondBr(cond_value, loop_block, after_block);
  } else {
    context.builder_.CreateBr(loop_block);
  }

  context.builder_.SetInsertPoint(loop_block);

  block_->CodeGen(context);

  if (increment_) {
    increment_->CodeGen(context);
  }

  if (cond_) {
    cond_value = cond_->CodeGen(context);
    cond_value = context.type_system_.CastToBool(context, cond_value);
    context.builder_.CreateCondBr(cond_value, loop_block, after_block);
  } else {
    context.builder_.CreateBr(loop_block);
  }

  context.PopBlock();

  parent_func->getBasicBlockList().push_back(after_block);
  context.builder_.SetInsertPoint(after_block);

  return nullptr;
}

ReturnStatement::ReturnStatement(std::shared_ptr<Expression> expression)
    : expression_{std::move(expression)} {}

ASTNodeType ReturnStatement::Kind() const {
  return ASTNodeType::kReturnStatement;
}

QJsonObject ReturnStatement::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind());
  QJsonArray children;

  if (expression_) {
    children.append(expression_->JsonGen());
  }
  root["children"] = children;

  return root;
}

// 目前只支持 return 在末尾的情况
llvm::Value *ReturnStatement::CodeGen(CodeGenContext &context) {
  if (expression_) {
    auto return_value = expression_->CodeGen(context);
    context.SetCurrentReturnValue(return_value);
    return return_value;
  } else {
    context.SetCurrentReturnValue(nullptr);
    return nullptr;
  }
}

Declaration::Declaration(std::shared_ptr<PrimitiveType> type,
                         std::shared_ptr<Identifier> name,
                         std::shared_ptr<Expression> init)
    : type_{std::move(type)},
      name_{std::move(name)},
      init_{std::move(init)} {}

ASTNodeType Declaration::Kind() const {
  return ASTNodeType::kDeclaration;
}

QJsonObject Declaration::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind());
  QJsonArray children;

  assert(type_ != nullptr);
  children.append(type_->JsonGen());

  assert(name_ != nullptr);
  children.append(name_->JsonGen());

  if (init_) {
    children.append(init_->JsonGen());
  }
  root["children"] = children;

  return root;
}

llvm::Value *Declaration::CodeGen(CodeGenContext &context) {
  auto type{context.type_system_.GetType(*type_)};
  auto parent_func{context.builder_.GetInsertBlock()->getParent()};
  auto addr{context.CreateEntryBlockAlloca(parent_func, type, name_->name_)};

  // 默认初始化为 0
  if (type->isIntegerTy(8)) {
    context.builder_.CreateStore(
        llvm::ConstantInt::get(context.the_context_, llvm::APInt(8, 0)), addr);
  } else if (type->isIntegerTy(32)) {
    context.builder_.CreateStore(
        llvm::ConstantInt::get(context.the_context_, llvm::APInt(32, 0)), addr);
  } else if (type->isDoubleTy()) {
    context.builder_.CreateStore(
        llvm::ConstantFP::get(context.the_context_, llvm::APFloat(0.0)), addr);
  }

  context.SetSymbolType(name_->name_, type_);
  context.SetSymbolAddr(name_->name_, addr);

  if (init_) {
    BinaryOpExpression{name_, init_, TokenValue::kAssign}.CodeGen(context);
  }

  return addr;
}

ASTNodeType Expression::Kind() const {
  return ASTNodeType::kExpression;
}

QJsonObject Expression::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind());
  return root;
}

UnaryOpExpression::UnaryOpExpression(std::shared_ptr<Expression> object, TokenValue op)
    : object_{std::move(object)}, op_{op} {}

ASTNodeType UnaryOpExpression::Kind() const {
  return ASTNodeType::kUnaryOpExpression;
}

QJsonObject UnaryOpExpression::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind()).append(' ')
      .append(TokenTypes::ToString(op_));

  QJsonArray children;
  assert(object_ != nullptr);
  children.append(object_->JsonGen());
  root["children"] = children;

  return root;
}

// ++/--/~/!
// TODO 类型转换
llvm::Value *UnaryOpExpression::CodeGen(CodeGenContext &context) {
  if (op_ == TokenValue::kInc || op_ == TokenValue::kDec) {
    return PostfixExpression{object_, op_}.CodeGen(context);
  } else if (op_ == TokenValue::kLogicNot) {
    auto value{context.type_system_.CastToBool(context, object_->CodeGen(context))};
    return context.builder_.CreateICmpNE(
        value,
        llvm::ConstantInt::get(context.the_context_,
                               llvm::APInt(1, true)));
  } else if (op_ == TokenValue::kNot) {
    return context.builder_.CreateNot(object_->CodeGen(context));
  } else {
    ErrorReportAndExit(location_, "Unknown unary prefix operator.");
    return nullptr;
  }
}

PostfixExpression::PostfixExpression(std::shared_ptr<Expression> object, TokenValue op)
    : object_{std::move(object)}, op_{op} {}

ASTNodeType PostfixExpression::Kind() const {
  return ASTNodeType::kPostfixExpression;
}

QJsonObject PostfixExpression::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind()).append(' ')
      .append(TokenTypes::ToString(op_));

  QJsonArray children;
  assert(object_ != nullptr);
  children.append(object_->JsonGen());
  root["children"] = children;

  return root;
}

// ++/--
// TODO 类型转换
llvm::Value *PostfixExpression::CodeGen(CodeGenContext &context) {
  BinaryOpExpression postfix{
      object_,
      std::make_shared<BinaryOpExpression>(
          object_, std::make_shared<Int32Constant>(1),
          op_ == TokenValue::kInc ? TokenValue::kAdd : TokenValue::kSub),
      TokenValue::kAssign};
  return postfix.CodeGen(context);
}

BinaryOpExpression::BinaryOpExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs, TokenValue op)
    : lhs_{std::move(lhs)}, rhs_{std::move(rhs)}, op_{op} {}

ASTNodeType BinaryOpExpression::Kind() const {
  return ASTNodeType::kBinaryOpExpression;
}

QJsonObject BinaryOpExpression::JsonGen() const {
  QJsonObject root;
  root.insert("name", ASTNodeTypes::ToString(Kind()).append(' ')
      .append(TokenTypes::ToString(op_)));
  QJsonArray children;

  assert(lhs_ != nullptr);
  children.append(lhs_->JsonGen());
  assert(rhs_ != nullptr);
  children.append(rhs_->JsonGen());
  root["children"] = children;

  return root;
}

// TODO 类型转换
llvm::Value *BinaryOpExpression::CodeGen(CodeGenContext &context) {
  if (op_ == TokenValue::kAssign) {
    auto lhs{std::dynamic_pointer_cast<Identifier>(lhs_)};
    if (!lhs) {
      ErrorReportAndExit(location_, "Destination of '=' must be a variable.");
      return nullptr;
    }

    auto rhs_value{rhs_->CodeGen(context)};
    if (!rhs_value) {
      ErrorReportAndExit(location_, "The code to the right of the assignment operator failed to generate.");
      return nullptr;
    }

    auto var{context.GetSymbolValue(lhs->name_)};
    if (!var) {
      ErrorReportAndExit(location_, "Unknown variable name {}.", lhs->name_);
    }
    context.builder_.CreateStore(rhs_value, var);

    return rhs_value;
  } else if (op_ == TokenValue::kAddAssign) {
    return BinaryOpExpression{
        lhs_, std::make_shared<BinaryOpExpression>(lhs_, rhs_, TokenValue::kAdd), TokenValue::kAssign}
        .CodeGen(context);
  } else if (op_ == TokenValue::kSubAssign) {
    return BinaryOpExpression{
        lhs_, std::make_shared<BinaryOpExpression>(lhs_, rhs_, TokenValue::kSub), TokenValue::kAssign}
        .CodeGen(context);
  } else if (op_ == TokenValue::kMulAssign) {
    return BinaryOpExpression{
        lhs_, std::make_shared<BinaryOpExpression>(lhs_, rhs_, TokenValue::kMul), TokenValue::kAssign}
        .CodeGen(context);
  } else if (op_ == TokenValue::kDivAssign) {
    return BinaryOpExpression{
        lhs_, std::make_shared<BinaryOpExpression>(lhs_, rhs_, TokenValue::kDiv), TokenValue::kAssign}
        .CodeGen(context);
  } else if (op_ == TokenValue::kModAssign) {
    return BinaryOpExpression{
        lhs_, std::make_shared<BinaryOpExpression>(lhs_, rhs_, TokenValue::kMod), TokenValue::kAssign}
        .CodeGen(context);
  } else if (op_ == TokenValue::kAndAssign) {
    return BinaryOpExpression{
        lhs_, std::make_shared<BinaryOpExpression>(lhs_, rhs_, TokenValue::kAnd), TokenValue::kAssign}
        .CodeGen(context);
  } else if (op_ == TokenValue::kOrAssign) {
    return BinaryOpExpression{
        lhs_, std::make_shared<BinaryOpExpression>(lhs_, rhs_, TokenValue::kOr), TokenValue::kAssign}
        .CodeGen(context);
  } else if (op_ == TokenValue::kXorAssign) {
    return BinaryOpExpression{
        lhs_, std::make_shared<BinaryOpExpression>(lhs_, rhs_, TokenValue::kXor), TokenValue::kAssign}
        .CodeGen(context);
  } else if (op_ == TokenValue::kShlAssign) {
    return BinaryOpExpression{
        lhs_, std::make_shared<BinaryOpExpression>(lhs_, rhs_, TokenValue::kShl), TokenValue::kAssign}
        .CodeGen(context);
  } else if (op_ == TokenValue::kShrAssign) {
    return BinaryOpExpression{
        lhs_, std::make_shared<BinaryOpExpression>(lhs_, rhs_, TokenValue::kShr), TokenValue::kAssign}
        .CodeGen(context);
  }

  auto lhs{lhs_->CodeGen(context)};
  auto rhs{rhs_->CodeGen(context)};
  bool is_double{false};

  if ((lhs->getType()->getTypeID() == llvm::Type::DoubleTyID)
      || (rhs->getType()->getTypeID() == llvm::Type::DoubleTyID)) {
    is_double = true;
    if ((rhs->getType()->getTypeID() != llvm::Type::DoubleTyID)) {
      rhs = context.builder_.CreateUIToFP(rhs, llvm::Type::getDoubleTy(context.the_context_));
    }
    if ((lhs->getType()->getTypeID() != llvm::Type::DoubleTyID)) {
      lhs = context.builder_.CreateUIToFP(lhs, llvm::Type::getDoubleTy(context.the_context_));
    }
  }

  if (!lhs || !rhs) {
    ErrorReportAndExit(location_, "Binary expression Code generation failed");
    return nullptr;
  }

  // LLVM 指令类型要求严格,两运算对象必须具有相同的类型
  switch (op_) {
    case TokenValue::kAdd:
      return is_double ? context.builder_.CreateFAdd(lhs, rhs)
                       : context.builder_.CreateAdd(lhs, rhs);
    case TokenValue::kSub:
      return is_double ? context.builder_.CreateFSub(lhs, rhs)
                       : context.builder_.CreateSub(lhs, rhs);
    case TokenValue::kMul:
      return is_double ? context.builder_.CreateFMul(lhs, rhs)
                       : context.builder_.CreateMul(lhs, rhs);
    case TokenValue::kDiv:
      return is_double ? context.builder_.CreateFDiv(lhs, rhs)
                       : context.builder_.CreateSDiv(lhs, rhs);
    case TokenValue::kAnd:
      return is_double ? ErrorReportAndExit(location_,
                                            "Double type has no AND operation"),
          nullptr : context.builder_.CreateAnd(lhs, rhs);
    case TokenValue::kOr:
      return is_double
             ? ErrorReportAndExit(location_, "Double type has no OR operation"),
                 nullptr : context.builder_.CreateOr(lhs, rhs);
    case TokenValue::kXor:
      return is_double ? ErrorReportAndExit(location_,
                                            "Double type has no XOR operation"),
          nullptr : context.builder_.CreateXor(lhs, rhs);
    case TokenValue::kShl:
      return is_double ? ErrorReportAndExit(location_,
                                            "Double type has no SHL operation"),
          nullptr : context.builder_.CreateShl(lhs, rhs);
    case TokenValue::kShr:
      return is_double ? ErrorReportAndExit(location_,
                                            "Double type has no SHR operation"),
          nullptr : context.builder_.CreateAShr(lhs, rhs);
    case TokenValue::kLess:
      return is_double
             ? (lhs = context.builder_.CreateFCmpULT(lhs, rhs),
              context.builder_.CreateUIToFP(
                  lhs, llvm::Type::getDoubleTy(context.the_context_)))
             : context.builder_.CreateICmpULT(lhs, rhs);
    case TokenValue::kLessEqual:
      return is_double
             ? (lhs = context.builder_.CreateFCmpOLE(lhs, rhs),
              context.builder_.CreateUIToFP(
                  lhs, llvm::Type::getDoubleTy(context.the_context_)))
             : context.builder_.CreateICmpSLE(lhs, rhs);
    case TokenValue::kGreaterEqual:
      return is_double
             ? (lhs = context.builder_.CreateFCmpOGE(lhs, rhs),
              context.builder_.CreateUIToFP(
                  lhs, llvm::Type::getDoubleTy(context.the_context_)))
             : context.builder_.CreateICmpSGE(lhs, rhs);
    case TokenValue::kGreater:
      return is_double
             ? (lhs = context.builder_.CreateFCmpOGT(lhs, rhs),
              context.builder_.CreateUIToFP(
                  lhs, llvm::Type::getDoubleTy(context.the_context_)))
             : context.builder_.CreateICmpSGT(lhs, rhs);
    case TokenValue::kEqual:
      return is_double ? context.builder_.CreateFCmpOEQ(lhs, rhs)
                       : context.builder_.CreateICmpEQ(lhs, rhs);
    case TokenValue::kNotEqual:
      return is_double ? context.builder_.CreateFCmpONE(lhs, rhs)
                       : context.builder_.CreateICmpNE(lhs, rhs);
    default: {
      ErrorReportAndExit(location_, "Unknown binary operator");
      return nullptr;
    }
  }
}

Identifier::Identifier(std::string name)
    : name_{std::move(name)} {}

ASTNodeType Identifier::Kind() const {
  return ASTNodeType::kIdentifier;
}

QJsonObject Identifier::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind()).append(' ')
      .append(QString::fromStdString(name_));
  return root;
}

llvm::Value *Identifier::CodeGen(CodeGenContext &context) {
  auto addr{context.GetSymbolValue(name_)};
  if (!addr) {
    ErrorReportAndExit(location_, "Unknown variable name.");
    return nullptr;
  }
  // 从栈中加载
  return context.builder_.CreateLoad(addr, name_);
}

FunctionCall::FunctionCall(std::shared_ptr<Identifier> name, std::shared_ptr<ExpressionList> args)
    : name_{std::move(name)}, args_{std::move(args)} {}

ASTNodeType FunctionCall::Kind() const {
  return ASTNodeType::kFunctionCall;
}

void FunctionCall::AddArg(std::shared_ptr<Expression> arg) {
  if (args_) {
    args_->push_back(std::move(arg));
  } else {
    args_ = std::make_shared<ExpressionList>();
    args_->push_back(std::move(arg));
  }
}

QJsonObject FunctionCall::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind());

  QJsonArray children;
  children.append(name_->JsonGen());

  if (args_) {
    for (const auto &arg : *args_) {
      assert(arg != nullptr);
      children.append(arg->JsonGen());
    }
  }
  root["children"] = children;

  return root;
}

// LLVM 默认使用本机C调用约定
// TODO 类型转换
llvm::Value *FunctionCall::CodeGen(CodeGenContext &context) {
  auto func{context.the_module_->getFunction(name_->name_)};
  if (!func) {
    ErrorReportAndExit(location_, "Unknown function referenced.");
    return nullptr;
  }

  std::vector<llvm::Value *> args_value;

  if (args_) {
    if (func->arg_size() != std::size(*args_)) {
      ErrorReportAndExit(location_, "Incorrect arguments passed.");
      return nullptr;
    }

    for (const auto &arg : *args_) {
      auto value{arg->CodeGen(context)};
      if (value->getType()->isIntegerTy(8)) {
        value = context.type_system_.Cast(value, context.type_system_.int32_type_,
                                          context.builder_.GetInsertBlock());
      }
      args_value.push_back(value);
      if (!args_value.back()) {
        ErrorReportAndExit(location_, "Function parameter code generation failed.");
        return nullptr;
      }
    }
  }

  return context.builder_.CreateCall(func, args_value);
}

FunctionDeclaration::FunctionDeclaration(std::shared_ptr<Type> return_type,
                                         std::shared_ptr<Identifier> name,
                                         std::shared_ptr<DeclarationList> args,
                                         std::shared_ptr<CompoundStatement> body)
    : return_type_{std::move(return_type)},
      name_{std::move(name)},
      args_{std::move(args)},
      body_{std::move(body)} {}

ASTNodeType FunctionDeclaration::Kind() const {
  if (body_) {
    return ASTNodeType::kFunctionDefinition;
  } else {
    return ASTNodeType::kFunctionDeclaration;
  }
}

QJsonObject FunctionDeclaration::JsonGen() const {
  QJsonObject root;
  root.insert("name", ASTNodeTypes::ToString(Kind()));
  QJsonArray children;
  children.append(return_type_->JsonGen());
  children.append(name_->JsonGen());

  if (args_) {
    for (auto &it : *args_) {
      assert(it != nullptr);
      children.append(it->JsonGen());
    }
  }

  if (body_) {
    children.append(body_->JsonGen());
  }
  root["children"] = children;

  return root;
}

llvm::Value *FunctionDeclaration::CodeGen(CodeGenContext &context) {
  std::vector<llvm::Type *> arg_types;

  if (args_) {
    for (const auto &arg : *args_) {
      arg_types.push_back(context.type_system_.GetType(*arg->type_));
    }
  }
  auto ret_type = context.type_system_.GetType(*return_type_);

  // false 说明该函数不是变参数函数,该函数在 the_module_ 的符号表中注册
  auto func_type{llvm::FunctionType::get(ret_type, arg_types, false)};
  auto func{llvm::Function::Create(func_type, llvm::Function::ExternalLinkage,
                                   name_->name_, context.the_module_.get())};

  // 设置每个参数的名字,使IR更具有可读性,此步骤非必须
  std::int32_t count{};
  for (auto &arg : func->args()) {
    arg.setName((*args_)[count++]->name_->name_);
  }

  if (body_) {
    if (!func->empty()) {
      ErrorReportAndExit(location_, "Function cannot be redefined.");
      return nullptr;
    }

    // 创建了一个名为entry的基本块,并且插入到 func 中
    auto entry{llvm::BasicBlock::Create(context.the_context_, "entry", func)};

    // 告诉 builder_ 新指令应该插入到 basic_block 的末尾
    context.builder_.SetInsertPoint(entry);
    context.PushBlock(entry);

    if (args_) {
      auto origin_arg_iter{std::begin(*args_)};
      for (auto &arg : func->args()) {
        arg.setName((*origin_arg_iter)->name_->name_);
        auto addr{context.CreateEntryBlockAlloca(func, arg.getType(), arg.getName())};
        context.builder_.CreateStore(&arg, addr);
        context.SetSymbolAddr((*origin_arg_iter)->name_->name_, addr);
        context.SetSymbolType((*origin_arg_iter)->name_->name_, (*origin_arg_iter)->type_);
        ++origin_arg_iter;
      }
    }

    body_->CodeGen(context);

    if (auto ret{context.GetCurrentReturnValue()}; ret) {
      context.builder_.CreateRet(ret);
    } else {
      context.builder_.CreateRetVoid();
    }

    // 此函数对生成的代码执行各种一致性检查,以确定我们的编译器是否
    // 所有的操作都做得正确
    llvm::verifyFunction(*func);
    // 优化该函数
    if (context.GetOptimization()) {
      context.the_FPM_->run(*func);
    }

    context.PopBlock();
  }

  return func;
}

void FunctionDeclaration::AddArg(std::shared_ptr<Declaration> arg) {
  if (args_) {
    args_->push_back(std::move(arg));
  } else {
    args_ = std::make_shared<DeclarationList>();
    args_->push_back(std::move(arg));
  }
}

CharConstant::CharConstant(char value)
    : value_{value} {}

ASTNodeType CharConstant::Kind() const {
  return ASTNodeType::kCharConstant;
}

QJsonObject CharConstant::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind()).append(' ')
      .append(value_);
  return root;
}

llvm::Value *CharConstant::CodeGen(CodeGenContext &context) {
  return llvm::ConstantInt::get(context.the_context_,
                                llvm::APInt(8, static_cast<std::uint64_t>(value_)));
}

Int32Constant::Int32Constant(std::int32_t value)
    : value_{value} {}

ASTNodeType Int32Constant::Kind() const {
  return ASTNodeType::kInt32Constant;
}

QJsonObject Int32Constant::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind()).append(' ')
      .append(QString::fromStdString(std::to_string(value_)));
  return root;
}

// 整形常量用 ConstantInt 类表示,用APInt表示整型数值
// 在LLVM IR中,常量都是唯一并且共享的
llvm::Value *Int32Constant::CodeGen(CodeGenContext &context) {
  return llvm::ConstantInt::get(context.the_context_,
                                llvm::APInt(32, static_cast<std::uint64_t>(value_)));
}

DoubleConstant::DoubleConstant(double value)
    : value_{value} {}

ASTNodeType DoubleConstant::Kind() const {
  return ASTNodeType::kDoubleConstant;
}

QJsonObject DoubleConstant::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind()).append(' ')
      .append(QString::fromStdString(std::to_string(value_)));
  return root;
}

// 浮点常量用 ConstantFP 类表示,用APFloat表示浮点数值(APFloat可以表示
// 任意精度的浮点常量)
llvm::Value *DoubleConstant::CodeGen(CodeGenContext &context) {
  return llvm::ConstantFP::get(context.the_context_, llvm::APFloat(value_));
}

StringLiteral::StringLiteral(std::string value)
    : value_{std::move(value)} {}

ASTNodeType StringLiteral::Kind() const {
  return ASTNodeType::kStringLiteral;
}

QJsonObject StringLiteral::JsonGen() const {
  QJsonObject root;
  root["name"] = ASTNodeTypes::ToString(Kind()).append(' ')
      .append(QString::fromStdString(value_));
  return root;
}

llvm::Value *StringLiteral::CodeGen(CodeGenContext &context) {
  return context.builder_.CreateGlobalString(value_);
}

}  // namespace tcc

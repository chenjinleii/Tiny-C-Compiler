//
// Created by kaiser on 18-12-8.
//

#include "error.h"
#include "ast.h"
#include "code_gen.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <cassert>
#include <cstdint>
#include <vector>

namespace tcc {

// TODO 放到 TypeSystem 中
llvm::Type *GetType(const Type &type, CodeGenContext &context) {
    return context.type_system_.GetVarType(type);
}

llvm::Value *CastToBool(CodeGenContext &context, llvm::Value *condition_value) {

    if (condition_value->getType()->getTypeID() == llvm::Type::IntegerTyID) {
        condition_value = context.builder_.CreateIntCast
                (condition_value, llvm::Type::getInt1Ty(context.the_context_), true);
        return context.builder_.CreateICmpNE
                (condition_value, llvm::ConstantInt::get(llvm::Type::getInt1Ty(context.the_context_), 0, true));
    } else if (condition_value->getType()->getTypeID() == llvm::Type::DoubleTyID) {
        return context.builder_.CreateFCmpONE
                (condition_value, llvm::ConstantFP::get(context.the_context_, llvm::APFloat(0.0)));
    } else {
        return condition_value;
    }
}

std::string ASTNodeTypes::ToString(ASTNodeTypes::Type type) {
    return QMetaEnum::fromType<ASTNodeTypes::Type>().valueToKey(type);
}

llvm::Value *ASTNode::CodeGen(CodeGenContext &) {
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
            ' ' + TokenTypes::ToString(type_);
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

    if (statements_) {
        for (const auto &it : *statements_) {
            assert(it != nullptr);
            root["children"].append(it->JsonGen());
        }
    }

    return root;
}

llvm::Value *CompoundStatement::CodeGen(CodeGenContext &context) {
    llvm::Value *last{};
    if (statements_) {
        for (auto &it : *statements_) {
            if (!it) {
                ErrorReportAndExit(location_, "The statement is empty");
            }
            last = it->CodeGen(context);
        }
    }

    return last;
}

void CompoundStatement::AddStatement(std::shared_ptr<Statement> statement) {
    if (statements_) {
        statements_->push_back(std::move(statement));
    } else {
        statements_ = std::make_shared<StatementList>();
        statements_->push_back(std::move(statement));
    }
}

Json::Value ExpressionStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    assert(expression_ != nullptr);
    root["children"].append(expression_->JsonGen());
    return root;
}

llvm::Value *ExpressionStatement::CodeGen(CodeGenContext &context) {
    if (!expression_) {
        ErrorReportAndExit(location_, "The expression is empty");
    }
    return expression_->CodeGen(context);
}

Json::Value IfStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());

    assert(condition_ != nullptr);
    root["children"].append(condition_->JsonGen());

    root["children"].append(then_block_->JsonGen());
    if (else_block_) {
        root["children"].append(else_block_->JsonGen());
    }
    return root;
}

llvm::Value *IfStatement::CodeGen(CodeGenContext &context) {
    if (!condition_) {
        ErrorReportAndExit(location_, "The condition of the if statement cannot be empty");
    }

    auto condition_value{condition_->CodeGen(context)};
    if (!condition_value) {
        return nullptr;
    }

    condition_value = CastToBool(context, condition_value);

    auto parent_func{context.builder_.GetInsertBlock()->getParent()};

    auto then_block = llvm::BasicBlock::Create(context.the_context_, "if_then", parent_func);
    auto else_block = llvm::BasicBlock::Create(context.the_context_, "if_else");
    auto after_block = llvm::BasicBlock::Create(context.the_context_, "if_after");

    if (else_block_) {
        context.builder_.CreateCondBr(condition_value, then_block, else_block);
    } else {
        context.builder_.CreateCondBr(condition_value, then_block, after_block);
    }

    context.builder_.SetInsertPoint(then_block);
    context.PushBlock(then_block);
    auto then_value{then_block_->CodeGen(context)};
    if (!then_value) {
        return nullptr;
    }
    context.PopBlock();
    // 注意需要使用控制流指令例如 return/branch 来终止基本块
    context.builder_.CreateBr(after_block);

    if (else_block_) {
        parent_func->getBasicBlockList().push_back(else_block);
        context.builder_.SetInsertPoint(else_block);
        context.PushBlock(then_block);
        auto else_value{else_block_->CodeGen(context)};
        if (!else_value) {
            return nullptr;
        }
        context.PopBlock();
        context.builder_.CreateBr(after_block);
    }

    parent_func->getBasicBlockList().push_back(after_block);
    context.builder_.SetInsertPoint(after_block);

    return nullptr;
}

Json::Value WhileStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    assert(cond_ != nullptr);
    root["children"].append(cond_->JsonGen());
    root["children"].append(block_->JsonGen());
    return root;
}

llvm::Value *WhileStatement::CodeGen(CodeGenContext &context) {
    if (!cond_) {
        ErrorReportAndExit(location_, "The condition of the while statement cannot be empty");
    }

    llvm::Value *cond_value{cond_->CodeGen(context)};
    if (!cond_value) {
        return nullptr;
    }

    auto parent_func{context.builder_.GetInsertBlock()->getParent()};
    auto loop_block{llvm::BasicBlock::Create(context.the_context_, "while_loop", parent_func)};
    auto after_block{llvm::BasicBlock::Create(context.the_context_, "while_after")};

    cond_value = CastToBool(context, cond_value);
    context.builder_.CreateCondBr(cond_value, loop_block, after_block);
    context.builder_.SetInsertPoint(loop_block);

    context.PushBlock(loop_block);
    llvm::Value *block_value = block_->CodeGen(context);
    if (!block_value) {
        return nullptr;
    }
    context.PopBlock();

    cond_value = cond_->CodeGen(context);
    cond_value = CastToBool(context, cond_value);
    context.builder_.CreateCondBr(cond_value, loop_block, after_block);

    parent_func->getBasicBlockList().push_back(after_block);
    context.builder_.SetInsertPoint(after_block);

    return nullptr;
}

Json::Value ForStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());

    if (init_) {
        root["children"].append(init_->JsonGen());
    }
    if (cond_) {
        root["children"].append(cond_->JsonGen());
    }
    if (increment_) {
        root["children"].append(increment_->JsonGen());
    }
    if (block_) {
        root["children"].append(block_->JsonGen());
    }

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

// TODO 处理 for 中三个表达式任意为空的情况
llvm::Value *ForStatement::CodeGen(CodeGenContext &context) {
    llvm::Value *init_value{};
    if (init_) {
        init_value = init_->CodeGen(context);
    }
    if (!init_value) {
        return nullptr;
    }

    auto parent_func{context.builder_.GetInsertBlock()->getParent()};
    auto loop_block{llvm::BasicBlock::Create(context.the_context_, "for_loop", parent_func)};
    auto after_block{llvm::BasicBlock::Create(context.the_context_, "for_after")};

    llvm::Value *cond_value{};
    if (cond_) {
        cond_value = cond_->CodeGen(context);
    }
    if (!cond_value) {
        return nullptr;
    }
    cond_value = CastToBool(context, cond_value);

    context.builder_.CreateCondBr(cond_value, loop_block, after_block);
    context.builder_.SetInsertPoint(loop_block);

    context.PushBlock(loop_block);
    llvm::Value *block_value = block_->CodeGen(context);
    if (!block_value) {
        return nullptr;
    }
    context.PopBlock();

    llvm::Value *increment_value{};
    if (increment_) {
        increment_value = increment_->CodeGen(context);
    }
    if (!increment_value) {
        return nullptr;
    }

    cond_value = cond_->CodeGen(context);
    cond_value = CastToBool(context, cond_value);
    context.builder_.CreateCondBr(cond_value, loop_block, after_block);

    parent_func->getBasicBlockList().push_back(after_block);
    context.builder_.SetInsertPoint(after_block);

    return nullptr;
}

Json::Value ReturnStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    assert(expression_ != nullptr);
    root["children"].append(expression_->JsonGen());

    return root;
}

llvm::Value *ReturnStatement::CodeGen(CodeGenContext &context) {
    if (!expression_) {
        ErrorReportAndExit(location_, "return expression is empty");
    }
    auto return_value = expression_->CodeGen(context);
    context.SetCurrentReturnValue(return_value);

    return return_value;
}

Json::Value Declaration::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());

    assert(type_ != nullptr);
    root["children"].append(type_->JsonGen());

    assert(name_ != nullptr);
    root["children"].append(name_->JsonGen());

    if (init_) {
        root["children"].append(init_->JsonGen());
    }
    return root;
}

llvm::Value *Declaration::CodeGen(CodeGenContext &context) {
    auto type{GetType(*type_, context)};
    auto parent_func{context.builder_.GetInsertBlock()->getParent()};
    auto addr{context.CreateEntryBlockAlloca(parent_func, type, name_->name_)};

    context.builder_.CreateStore(llvm::ConstantInt::get(context.the_context_, llvm::APInt(32, 0)), addr);

    context.SetSymbolType(name_->name_, type_);
    context.SetSymbolValue(name_->name_, addr);

    if (init_) {
        BinaryOpExpression assignment(name_, init_, TokenValue::kAssign);
        assignment.CodeGen(context);
    }
    return addr;
}

Json::Value Expression::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    return root;
}

Json::Value UnaryOpExpression::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind()) + " "
            + TokenTypes::ToString(op_);

    assert(object_ != nullptr);
    root["children"].append(object_->JsonGen());

    return root;
}

llvm::Value *UnaryOpExpression::CodeGen(CodeGenContext &context) {
    return ASTNode::CodeGen(context);
}

Json::Value PostfixExpression::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind()) + " "
            + TokenTypes::ToString(op_);

    assert(object_ != nullptr);
    root["children"].append(object_->JsonGen());

    return root;
}

llvm::Value *PostfixExpression::CodeGen(CodeGenContext &context) {
    return ASTNode::CodeGen(context);
}

Json::Value BinaryOpExpression::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind()) + " "
            + TokenTypes::ToString(op_);

    assert(lhs_ != nullptr);
    root["children"].append(lhs_->JsonGen());
    assert(rhs_ != nullptr);
    root["children"].append(rhs_->JsonGen());

    return root;
}

llvm::Value *BinaryOpExpression::CodeGen(CodeGenContext &context) {
    auto lhs{lhs_->CodeGen(context)};
    auto rhs{rhs_->CodeGen(context)};
    bool fp{false};

    if ((lhs->getType()->getTypeID() == llvm::Type::DoubleTyID)
            || (rhs->getType()->getTypeID() == llvm::Type::DoubleTyID)) {
        fp = true;
        if ((rhs->getType()->getTypeID() != llvm::Type::DoubleTyID)) {
            rhs = context.builder_.CreateUIToFP(rhs, llvm::Type::getDoubleTy(context.the_context_), "ftmp");
        }
        if ((lhs->getType()->getTypeID() != llvm::Type::DoubleTyID)) {
            lhs = context.builder_.CreateUIToFP(lhs, llvm::Type::getDoubleTy(context.the_context_), "ftmp");
        }
    }

    if (!lhs || !rhs) {
        return nullptr;
    }

    // LLVM 指令类型要求严格,两运算对象必须具有相同的类型
    switch (op_) {
        case TokenValue::kAssign:

        case TokenValue::kAdd:
            return fp ? context.builder_.CreateFAdd(lhs, rhs) :
                   context.builder_.CreateAdd(lhs, rhs);
        case TokenValue::kSub:
            return fp ? context.builder_.CreateFSub(lhs, rhs) :
                   context.builder_.CreateSub(lhs, rhs);
        case TokenValue::kMul   :
            return fp ? context.builder_.CreateFMul(lhs, rhs) :
                   context.builder_.CreateMul(lhs, rhs);
        case TokenValue::kDiv:
            return fp ? context.builder_.CreateFDiv(lhs, rhs) :
                   context.builder_.CreateSDiv(lhs, rhs);
        case TokenValue::kAnd:
            return fp ? ErrorReportAndExit(location_, "Double type has no AND operation"), nullptr :
                   context.builder_.CreateAnd(lhs, rhs);
        case TokenValue::kOr:
            return fp ? ErrorReportAndExit(location_, "Double type has no OR operation"), nullptr :
                   context.builder_.CreateOr(lhs, rhs);
        case TokenValue::kXor:
            return fp ? ErrorReportAndExit(location_, "Double type has no XOR operation"), nullptr :
                   context.builder_.CreateXor(lhs, rhs);
        case TokenValue::kShl:
            return fp ? ErrorReportAndExit(location_, "Double type has no SHL operation"), nullptr :
                   context.builder_.CreateShl(lhs, rhs);
        case TokenValue::kShr:
            return fp ? ErrorReportAndExit(location_, "Double type has no SHR operation"), nullptr :
                   context.builder_.CreateAShr(lhs, rhs);
        case TokenValue::kLess:
            return fp ? (lhs = context.builder_.CreateFCmpULT(lhs, rhs),
                    context.builder_.CreateUIToFP(lhs, llvm::Type::getDoubleTy(context.the_context_))) :
                   context.builder_.CreateICmpULT(lhs, rhs);
        case TokenValue::kLessOrEqual:
            return fp ? (lhs = context.builder_.CreateFCmpOLE(lhs, rhs),
                    context.builder_.CreateUIToFP(lhs, llvm::Type::getDoubleTy(context.the_context_))) :
                   context.builder_.CreateICmpSLE(lhs, rhs);
        case TokenValue::kGreaterOrEqual:
            return fp ? (lhs = context.builder_.CreateFCmpOGE(lhs, rhs),
                    context.builder_.CreateUIToFP(lhs, llvm::Type::getDoubleTy(context.the_context_))) :
                   context.builder_.CreateICmpSGE(lhs, rhs);
        case TokenValue::kGreater:
            return fp ? (lhs = context.builder_.CreateFCmpOGT(lhs, rhs),
                    context.builder_.CreateUIToFP(lhs, llvm::Type::getDoubleTy(context.the_context_))) :
                   context.builder_.CreateICmpSGT(lhs, rhs);
        case TokenValue::kEqual:
            return fp ? context.builder_.CreateFCmpOEQ(lhs, rhs) :
                   context.builder_.CreateICmpEQ(lhs, rhs);
        case TokenValue::kNotEqual:
            return fp ? context.builder_.CreateFCmpONE(lhs, rhs) :
                   context.builder_.CreateICmpNE(lhs, rhs);
        default: {
            ErrorReportAndExit(location_, "Unknown binary operator");
            return nullptr;
        }
    }
}

Json::Value Identifier::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind()) + " " + name_;
    return root;
}

llvm::Value *Identifier::CodeGen(CodeGenContext &context) {
    auto value{context.GetSymbolValue(name_)};
    if (!value) {
        ErrorReportAndExit(location_, "Unknown variable name");
        return nullptr;
    }
    // 从栈中加载
    return context.builder_.CreateLoad(value, name_);
}

Json::Value FunctionCall::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    root["children"].append(name_->JsonGen());

    if (args_) {
        for (const auto &it : *args_) {
            assert(it != nullptr);
            root["children"].append(it->JsonGen());
        }
    }

    return root;
}

// LLVM 默认使用本机C调用约定
llvm::Value *FunctionCall::CodeGen(CodeGenContext &context) {
    auto function{context.the_module_->getFunction(name_->name_)};
    if (!function) {
        ErrorReportAndExit(location_, "Unknown function referenced");
        return nullptr;
    }

    std::vector<llvm::Value *> args_value;

    if (args_) {
        if (function->arg_size() != std::size(*args_)) {
            ErrorReportAndExit(location_, "Incorrect arguments passed");
            return nullptr;
        }

        for (const auto &arg:*args_) {
            args_value.push_back(arg->CodeGen(context));
            if (!args_value.back()) {
                return nullptr;
            }
        }
    }

    return context.builder_.CreateCall(function, args_value);
}

void FunctionCall::AddArg(std::shared_ptr<Expression> arg) {
    if (args_) {
        args_->push_back(std::move(arg));
    } else {
        args_ = std::make_shared<ExpressionList>();
        args_->push_back(std::move(arg));
    }
}

ASTNodeType FunctionDeclaration::Kind() const {
    if (body_) {
        return ASTNodeType::kFunctionDefinition;
    } else {
        return ASTNodeType::kFunctionDeclaration;
    }
}

Json::Value FunctionDeclaration::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    root["children"].append(return_type_->JsonGen());
    root["children"].append(name_->JsonGen());

    if (args_) {
        for (auto &it : *args_) {
            assert(it != nullptr);
            root["children"].append(it->JsonGen());
        }
    }

    if (body_) {
        root["children"].append(body_->JsonGen());
    }

    return root;
}

llvm::Value *FunctionDeclaration::CodeGen(CodeGenContext &context) {
    std::vector<llvm::Type *> arg_types;

    if (args_) {
        for (const auto &arg: *args_) {
            arg_types.push_back(GetType(*arg->type_, context));
        }
    }
    auto ret_type = GetType(*return_type_, context);

    // false 说明该函数不是变参数函数,该函数在 the_module_ 的符号表中注册
    auto func_type = llvm::FunctionType::get(ret_type, arg_types, false);
    auto func = llvm::Function::Create(func_type,
                                       llvm::Function::ExternalLinkage,
                                       name_->name_,
                                       context.the_module_.get());

    // 设置每个参数的名字,使IR更具有可读性,此步骤非必须
    std::int32_t count{};
    for (auto &arg:func->args()) {
        arg.setName((*args_)[count++]->name_->name_);
    }
    // TODO 函数重定义问题
    if (body_) {
        // 创建了一个名为entry的基本块,并且插入到 func 中
        auto entry{llvm::BasicBlock::Create(context.the_context_, "entry", func)};

        // 告诉 builder_ 新指令应该插入到 basic_block 的末尾
        context.builder_.SetInsertPoint(entry);
        context.PushBlock(entry);

        if (args_) {
            auto origin_arg{std::begin(*args_)};
            for (auto &arg: func->args()) {
//                arg.setName((*origin_arg)->name_->name_);
//                auto alloca{context.CreateEntryBlockAlloca(par)};
//
//                context.builder_.CreateStore(&arg, arg_alloc, false);
//                context.SetSymbolValue((*origin_arg)->name_->name_, arg_alloc);
//                context.SetSymbolType((*origin_arg)->name_->name_, (*origin_arg)->type_);
//                ++origin_arg;
            }
        }

        body_->CodeGen(context);
        // TODO 处理没有 return 语句的情况
        if (context.GetCurrentReturnValue()) {
            context.builder_.CreateRet(context.GetCurrentReturnValue());
            // 此函数对生成的代码执行各种一致性检查,以确定我们的编译器是否
            // 所有的操作都做得正确
            llvm::verifyFunction(*func);
            // 优化该函数
            context.the_FPM_->run(*func);
        } else {
            ErrorReportAndExit(location_, "Function block return value not founded");
            return nullptr;
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

Json::Value CharConstant::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind()) + " " + value_;
    return root;
}

llvm::Value *CharConstant::CodeGen(CodeGenContext &context) {
    return llvm::ConstantInt::get(context.the_context_, llvm::APInt(8, static_cast<std::uint64_t>(value_)));
}

Json::Value Int32Constant::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind())
            + " " + std::to_string(value_);

    return root;
}

// 整形常量用 ConstantInt 类表示,用APInt表示整型数值
// 在LLVM IR中,常量都是唯一并且共享的
llvm::Value *Int32Constant::CodeGen(CodeGenContext &context) {
    return llvm::ConstantInt::get(context.the_context_, llvm::APInt(32, static_cast<std::uint64_t>(value_)));
}

Json::Value DoubleConstant::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind())
            + " " + std::to_string(value_);
    return root;
}

// 浮点常量用 ConstantFP 类表示,用APFloat表示浮点数值(APFloat可以表示
// 任意精度的浮点常量)
llvm::Value *DoubleConstant::CodeGen(CodeGenContext &context) {
    return llvm::ConstantFP::get(context.the_context_, llvm::APFloat(value_));
}

Json::Value StringLiteral::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind())
            + " " + value_;
    return root;
}

llvm::Value *StringLiteral::CodeGen(CodeGenContext &context) {
    return context.builder_.CreateGlobalString(value_);
}

}

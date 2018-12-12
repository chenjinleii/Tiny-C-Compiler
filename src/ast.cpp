//
// Created by kaiser on 18-12-8.
//

#include "error.h"
#include "ast.h"
#include "code_gen.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>

#include <cassert>
#include <cstdint>
#include <vector>

namespace tcc {

llvm::Type *TypeOf(Type type, CodeGenContext &context) {
    return context.type_system_.getVarType(type.type_);
}

llvm::Value *CastToBoolean(CodeGenContext &context, llvm::Value *condValue) {

    if (condValue->getType()->getTypeID() == llvm::Type::IntegerTyID) {
        condValue = context.builder_.CreateIntCast(condValue, llvm::Type::getInt1Ty(context.the_context_), true);
        return context.builder_.CreateICmpNE(condValue,
                                             llvm::ConstantInt::get(llvm::Type::getInt1Ty(context.the_context_),
                                                                    0,
                                                                    true));
    } else if (condValue->getType()->getTypeID() == llvm::Type::DoubleTyID) {
        return context.builder_.CreateFCmpONE(condValue,
                                              llvm::ConstantFP::get(context.the_context_, llvm::APFloat(0.0)));
    } else {
        return condValue;
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

    if (statements_) {
        for (const auto &it : *statements_) {
            assert(it != nullptr);
            root["children"].append(it->JsonGen());
        }
    }

    return root;
}

llvm::Value *CompoundStatement::CodeGen(CodeGenContext &context) {
    llvm::Value *last = nullptr;
    for (auto &it : *statements_) {
        last = it->CodeGen(context);
    }
    return last;
}

Json::Value ExpressionStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    assert(expression_ != nullptr);
    root["children"].append(expression_->JsonGen());
    return root;
}

llvm::Value *ExpressionStatement::CodeGen(CodeGenContext &context) {
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
    llvm::Value *condValue = condition_->CodeGen(context);
    if (!condValue) {
        return nullptr;
    }

    condValue = CastToBoolean(context, condValue);

    auto theFunction = context.builder_.GetInsertBlock()->getParent();

    auto thenBB = llvm::BasicBlock::Create(context.the_context_, "then", theFunction);
    auto falseBB = llvm::BasicBlock::Create(context.the_context_, "else");
    auto mergeBB = llvm::BasicBlock::Create(context.the_context_, "ifcont");

    if (this->else_block_) {
        context.builder_.CreateCondBr(condValue, thenBB, falseBB);
    } else {
        context.builder_.CreateCondBr(condValue, thenBB, mergeBB);
    }

    context.builder_.SetInsertPoint(thenBB);
    context.PushBlock(thenBB);
    then_block_->CodeGen(context);
    context.PopBlock();

    thenBB = context.builder_.GetInsertBlock();

    if (thenBB->getTerminator() == nullptr) {
        context.builder_.CreateBr(mergeBB);
    }

    if (else_block_) {
        theFunction->getBasicBlockList().push_back(falseBB);
        context.builder_.SetInsertPoint(falseBB);
        context.PushBlock(thenBB);
        else_block_->CodeGen(context);
        context.PopBlock();
        context.builder_.CreateBr(mergeBB);
    }

    theFunction->getBasicBlockList().push_back(mergeBB);
    context.builder_.SetInsertPoint(mergeBB);

    return nullptr;
}

Json::Value WhileStatement::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    assert(condition_ != nullptr);
    root["children"].append(condition_->JsonGen());
    root["children"].append(block_->JsonGen());
    return root;
}

llvm::Value *WhileStatement::CodeGen(CodeGenContext &context) {
    return ASTNode::CodeGen(context);
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
    if (block_) {
        root["children"].append(block_->JsonGen());
    }

    return root;
}

llvm::Value *ForStatement::CodeGen(CodeGenContext &context) {
    auto parent_func{context.builder_.GetInsertBlock()->getParent()};

    auto block{llvm::BasicBlock::Create(context.the_context_, "for_loop", parent_func)};
    auto after{llvm::BasicBlock::Create(context.the_context_, "for_cont")};

    if (initial_) {
        initial_->CodeGen(context);
    }

    auto cond_value{condition_->CodeGen(context)};
    if (!cond_value) {
        return nullptr;
    }

    cond_value = CastToBoolean(context, cond_value);

    context.builder_.CreateCondBr(cond_value, block, after);
    context.builder_.SetInsertPoint(block);
    context.PushBlock(block);

    block_->CodeGen(context);
    context.PopBlock();

    if (increment_) {
        increment_->CodeGen(context);
    }

    cond_value = condition_->CodeGen(context);
    cond_value = CastToBoolean(context, cond_value);
    context.builder_.CreateCondBr(cond_value, block, after);

    parent_func->getBasicBlockList().push_back(after);
    context.builder_.SetInsertPoint(after);

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
    auto returnValue = expression_->CodeGen(context);
    context.SetCurrentReturnValue(returnValue);
    return returnValue;
}

Json::Value Declaration::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());

    assert(type_ != nullptr);
    root["children"].append(type_->JsonGen());

    assert(variable_name_ != nullptr);
    root["children"].append(variable_name_->JsonGen());

    if (initialization_expression_) {
        root["children"].append(initialization_expression_->JsonGen());
    }
    return root;
}

llvm::Value *Declaration::CodeGen(CodeGenContext &context) {
    llvm::Type *type = TypeOf(*type_, context);
    llvm::Value *initial = nullptr;

    llvm::Value *inst = nullptr;

    inst = context.builder_.CreateAlloca(type);

    context.SetSymbolType(variable_name_->name_, type_);
    context.SetSymbolValue(variable_name_->name_, inst);

    if (initialization_expression_ != nullptr) {
        BinaryOpExpression assignment(variable_name_, initialization_expression_, TokenValue::kAssign);
        assignment.CodeGen(context);
    }
    return inst;
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
            + TokenValues::ToString(op_);

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
            + TokenValues::ToString(op_);

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
            return fp ? ErrorReport("Double type has no AND operation") :
                   context.builder_.CreateAnd(lhs, rhs);
        case TokenValue::kOr:
            return fp ? ErrorReport("Double type has no OR operation") :
                   context.builder_.CreateOr(lhs, rhs);
        case TokenValue::kXor:
            return fp ? ErrorReport("Double type has no XOR operation") :
                   context.builder_.CreateXor(lhs, rhs);
        case TokenValue::kShl:
            return fp ? ErrorReport("Double type has no LEFT SHIFT operation") :
                   context.builder_.CreateShl(lhs, rhs);
        case TokenValue::kShr:
            return fp ? ErrorReport("Double type has no RIGHT SHIFT operation") :
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
        default:return ErrorReport("Unknown binary operator");
    }
}

Json::Value Identifier::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind()) + " " + name_;
    return root;
}

llvm::Value *Identifier::CodeGen(CodeGenContext &context) {
    llvm::Value *value = context.GetSymbolValue(name_);
    if (!value) {
        return ErrorReport("Unknown variable name ");
    }
    return context.builder_.CreateLoad(value, false, "");
}

Json::Value FunctionCall::JsonGen() const {
    Json::Value root;
    root["name"] = ASTNodeTypes::ToString(Kind());
    root["children"].append(function_name_->JsonGen());

    if (args_) {
        for (const auto &it : *args_) {
            assert(it != nullptr);
            root["children"].append(it->JsonGen());
        }
    }

    return root;
}

llvm::Value *FunctionCall::CodeGen(CodeGenContext &context) {
    auto function{context.the_module_->getFunction(function_name_->name_)};
    if (!function) {
        return ErrorReport("Unknown function referenced");
    }

    if (function->arg_size() != std::size(*args_)) {
        return ErrorReport("Incorrect # arguments passed");
    }

    std::vector<llvm::Value *> args_value;
    for (const auto &arg:*args_) {
        args_value.push_back(arg->CodeGen(context));
        if (!args_value.back()) {
            return nullptr;
        }
    }

    return context.builder_.CreateCall(function, args_value);
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
    std::vector<llvm::Type *> argTypes;

    for (auto &arg: *this->args_) {
        argTypes.push_back(TypeOf(*arg->type_, context));
    }
    llvm::Type *retType = nullptr;
    retType = TypeOf(*return_type_, context);

    auto functionType = llvm::FunctionType::get(retType, argTypes, false);
    auto function = llvm::Function::Create(functionType,
                                           llvm::GlobalValue::ExternalLinkage,
                                           function_name_->name_,
                                           context.the_module_.get());

    if (has_body_) {
        auto basicBlock = llvm::BasicBlock::Create(context.the_context_, "entry", function, nullptr);

        context.builder_.SetInsertPoint(basicBlock);
        context.PushBlock(basicBlock);

        // declare function params
        auto origin_arg = this->args_->begin();

        for (auto &ir_arg_it: function->args()) {
            ir_arg_it.setName((*origin_arg)->variable_name_->name_);
            llvm::Value *argAlloc;
            argAlloc = (*origin_arg)->CodeGen(context);

            context.builder_.CreateStore(&ir_arg_it, argAlloc, false);
            context.SetSymbolValue((*origin_arg)->variable_name_->name_, argAlloc);
            context.SetSymbolType((*origin_arg)->variable_name_->name_, (*origin_arg)->type_);
            context.SetFuncArg((*origin_arg)->variable_name_->name_, true);
            origin_arg++;
        }

        this->body_->CodeGen(context);
        if (context.GetCurrentReturnValue()) {
            context.builder_.CreateRet(context.GetCurrentReturnValue());
        } else {
            return ErrorReport("Function block return value not founded");
        }
        context.PopBlock();

    }

    return function;
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

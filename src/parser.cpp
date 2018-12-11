//
// Created by kaiser on 18-12-8.
//

// TODO 多个字符的运算符 字符和字符串 一元运算符 函数调用 位置记录

#include "location.h"
#include "error.h"
#include "parser.h"

#include <iostream>
#include <cstdlib>

namespace tcc {

std::shared_ptr<CompoundStatement> Parser::parse() {
    auto program_block{MakeASTNode<CompoundStatement>()};

    if (std::size(token_sequence_) == 0) {
        ErrorReportAndExit("no token");
    }

    while (HasNextToken()) {
        program_block->statements_->push_back(ParseGlobal());
    }

    return program_block;
}

Token Parser::GetNextToken() {
    if (HasNextToken()) {
        return token_sequence_[index_++];
    } else {
        return Token{SourceLocation{}, TokenValue::kEof};
    }
}

Token Parser::PeekNextToken() const {
    if (HasNextToken()) {
        return token_sequence_[index_];
    } else {
        return Token{SourceLocation{}, TokenValue::kEof};
    }
}

bool Parser::HasNextToken() const {
    return index_ < std::size(token_sequence_);
}

bool Parser::Try(TokenValue value) {
    if (PeekNextToken().TokenValueIs(value)) {
        GetNextToken();
        return true;
    } else {
        return false;
    }
}

bool Parser::Test(TokenValue value) {
    return PeekNextToken().TokenValueIs(value);
}

void Parser::Expect(TokenValue value) {
    if (!PeekNextToken().TokenValueIs(value)) {
        ErrorReportAndExit(PeekNextToken().GetTokenLocation(), value, PeekNextToken().GetTokenValue());
    } else {
        GetNextToken();
    }
}

std::shared_ptr<Statement> Parser::ParseGlobal() {
    auto result{ParseDeclaration()};
    return result;
}

std::shared_ptr<Statement> Parser::ParseDeclaration() {
    auto var_declaration_no_init{ParVarDeclarationNoInit()};

    if (Try(TokenValue::kLeftParen)) {
        auto function{ParseFunctionDeclaration()};
        function->return_type_ = std::move(var_declaration_no_init->type_);
        function->function_name_ = std::move(var_declaration_no_init->variable_name_);
        Expect(TokenValue::kSemicolon);
        return function;
    } else if (Try(TokenValue::kEqual)) {
        auto initialization_expression{ParseExpression()};
        var_declaration_no_init->initialization_expression_ = std::move(initialization_expression);
        Expect(TokenValue::kSemicolon);
        return var_declaration_no_init;
    } else {
        Expect(TokenValue::kSemicolon);
        return var_declaration_no_init;
    }
}

std::shared_ptr<PrimitiveType> Parser::ParseTypeSpecifier() {
    if (PeekNextToken().IsTypeSpecifier()) {
        auto result{MakeASTNode<PrimitiveType>(GetNextToken().GetTokenValue())};
        return result;
    } else {
        return nullptr;
    }
}

std::shared_ptr<Identifier> Parser::ParseIdentifier() {
    if (PeekNextToken().IsIdentifier()) {
        auto result{MakeASTNode<Identifier>(GetNextToken().GetTokenName())};
        return result;
    } else {
        return nullptr;
    }
}

std::shared_ptr<FunctionDeclaration> Parser::ParseFunctionDeclaration() {
    auto function{MakeASTNode<FunctionDeclaration>()};

    while (!Test(TokenValue::kRightParen)) {
        function->args_->push_back(ParVarDeclarationNoInit());
        Expect(TokenValue::kComma);
    }

    Expect(TokenValue::kRightParen);

    if (Try(TokenValue::kLeftCurly)) {
        function->has_body_ = true;
        function->body_ = ParseCompound();
        Expect(TokenValue::kRightCurly);
        return function;
    } else {
        return function;
    }
}

// 强制要求大括号
std::shared_ptr<CompoundStatement> Parser::ParseCompound() {
    auto compound{MakeASTNode<CompoundStatement>()};
    while (!Test(TokenValue::kRightCurly)) {
        compound->statements_->push_back(ParseStatement());
    }
    return compound;
}

std::shared_ptr<Declaration> Parser::ParVarDeclarationNoInit() {
    auto type{ParseTypeSpecifier()};
    if (!type) {
        ErrorReportAndExit("expect a type specifier");
    }

    auto identifier{ParseIdentifier()};
    if (!identifier) {
        ErrorReportAndExit("expect a identifier");
    }
    return MakeASTNode<Declaration>(std::move(type), std::move(identifier), nullptr);
}

std::shared_ptr<Declaration> Parser::ParVarDeclarationWithInit() {
    auto var_declaration_no_init{ParVarDeclarationNoInit()};

    if (Try(TokenValue::kEqual)) {
        var_declaration_no_init->initialization_expression_ = ParseExpression();
    }
    Expect(TokenValue::kSemicolon);

    return var_declaration_no_init;
}

std::shared_ptr<Statement> Parser::ParseStatement() {
    if (Try(TokenValue::kIfKeyword)) {
        return ParseIfStatement();
    } else if (Try(TokenValue::kWhileKeyword)) {
        return ParseWhileStatement();
    } else if (Try(TokenValue::kForKeyword)) {
        return ParseForStatement();
    } else if (Try(TokenValue::kReturnKeyword)) {
        return ParseReturnStatement();
    } else if (PeekNextToken().IsTypeSpecifier()) {
        return ParVarDeclarationWithInit();
    } else if (Try(TokenValue::kLeftCurly)) {
        auto compound{ParseCompound()};
        Expect(TokenValue::kRightCurly);
        return compound;
    } else {
        return ParseExpressionStatement();
    }
}

std::shared_ptr<IfStatement> Parser::ParseIfStatement() {
    auto if_statement{MakeASTNode<IfStatement>()};

    Expect(TokenValue::kLeftParen);
    if_statement->condition_ = ParseExpression();
    Expect(TokenValue::kRightParen);

    Expect(TokenValue::kLeftCurly);
    if_statement->then_block_ = ParseCompound();
    Expect(TokenValue::kRightCurly);

    if (Try(TokenValue::kElseKeyword)) {
        Expect(TokenValue::kLeftCurly);
        if_statement->else_block_ = ParseCompound();
        Expect(TokenValue::kRightCurly);
    }

    return if_statement;
}

std::shared_ptr<WhileStatement> Parser::ParseWhileStatement() {
    auto while_statement{MakeASTNode<WhileStatement>()};

    Expect(TokenValue::kLeftParen);
    while_statement->condition_ = ParseExpression();
    Expect(TokenValue::kRightParen);

    Expect(TokenValue::kLeftCurly);
    while_statement->block_ = ParseCompound();
    Expect(TokenValue::kRightCurly);

    return while_statement;
}

std::shared_ptr<ForStatement> Parser::ParseForStatement() {
    auto for_statement{MakeASTNode<ForStatement>()};

    Expect(TokenValue::kLeftParen);
    //TODO 支持声明变量
    for_statement->initial_ = ParseExpression();
    Expect(TokenValue::kSemicolon);

    for_statement->condition_ = ParseExpression();
    Expect(TokenValue::kSemicolon);
    for_statement->increment_ = ParseExpression();
    Expect(TokenValue::kRightParen);

    Expect(TokenValue::kLeftCurly);
    for_statement->block_ = ParseCompound();
    Expect(TokenValue::kRightCurly);

    return for_statement;
}

std::shared_ptr<ReturnStatement> Parser::ParseReturnStatement() {
    auto return_statement{MakeASTNode<ReturnStatement>()};

    if (Try(TokenValue::kSemicolon)) {
        return return_statement;
    } else {
        return_statement->expression_ = ParseExpression();
        Expect(TokenValue::kSemicolon);
        return return_statement;
    }
}

std::shared_ptr<ExpressionStatement> Parser::ParseExpressionStatement() {
    auto expression_statement{std::make_unique<ExpressionStatement>()};

    expression_statement->expression_ = ParseExpression();
    Expect(TokenValue::kSemicolon);

    return expression_statement;
}

std::shared_ptr<Expression> Parser::ParseExpression() {
//    auto result{ParseUnaryOpExpression()};
//
//    while (HasNextToken()) {
//        auto next{PeekNextToken()};
//        if (!next.IsOperator()) {
//            break;
//        }
//        if (next.GetTokenPrecedence() > precedence) {
//            if (next.TokenValueIs(TokenValue::kQuestionMark)) {
//                result = ParseTernaryOpExpression(std::move(result));
//            } else {
//                GetNextToken();
//                result = std::move(HackExpression(std::move(MakeASTNode<BinaryOpExpression>
//                                                                    (std::move(result),
//                                                                     std::move(ParseExpression(next.GetTokenPrecedence()
//                                                                                                       + next.IsPrefixOperator())),
//                                                                     next.GetTokenValue()))));
//            }
//        } else {
//            break;
//        }
//    }
//    return std::move(result);

    auto LHS = ParsePrimary();
    if (!LHS) {
        return nullptr;
    }

    return ParseBinOpRHS(0, std::move(LHS));
}

//std::shared_ptr<Expression> Parser::ParseUnaryOpExpression() {
//    if (Test(TokenValue::kAdd) || Test(TokenValue::kSub) ||
//            Test(TokenValue::kInc) || Test(TokenValue::kDec) || Test(TokenValue::kNeg)) {
//        auto next{GetNextToken()};
//        return std::move(MakeASTNode<UnaryOpExpression>(ParseUnaryOpExpression(), next.GetTokenValue()));
//    } else if (Try(TokenValue::kSizeofKeyword)) {
//        return ParseSizeof();
//    }
//    return ParsePostfixExpression();
//}
//
//std::shared_ptr<Expression> Parser::ParsePostfixExpression() {
//    auto postfix{ParsePrimary()};
//
//    if (Try(TokenValue::kLeftParen)) {
//        auto function_call{MakeASTNode<FunctionCall>()};
//        function_call->function_name_ = std::make_unique<Identifier>(postfix->name_);
//
//        if (!PeekNextToken().TokenValueIs(TokenValue::kRightParen)) {
//            while (true) {
//                if (auto arg{ParseExpression()}) {
//                    function_call->args_->push_back(std::move(arg));
//                } else {
//                    return nullptr;
//                }
//
//                if (Test(TokenValue::kRightParen)) {
//                    break;
//                }
//
//                if (!Test(TokenValue::kComma)) {
//                    ErrorReportAndExit("expect ) or ,");
//                }
//                GetNextToken();
//            }
//        }
//    } else {
//        auto result{MakeASTNode<PostfixExpression>()};
//        result->object_ = std::move(postfix);
//        result->op_ = GetNextToken().GetTokenValue();
//    }
//
//    return postfix;
//};

std::shared_ptr<Expression> Parser::ParsePrimary() {
    if (Test(TokenValue::kIdentifier)) {
        return ParseIdentifier();
    } else if (Test(TokenValue::kIntConstant)) {
        return ParseIntConstant();
    } else if (Test(TokenValue::kCharConstant)) {
        return ParseCharConstant();
    } else if (Test(TokenValue::kDoubleConstant)) {
        return ParseDoubleConstant();
    } else if (Test(TokenValue::kStringLiteral)) {
        return ParseStringLiteral();
    } else if (Try(TokenValue::kLeftParen)) {
        auto expr{ParseExpression()};
        Expect(TokenValue::kRightParen);
        return expr;
    } else {
        return nullptr;
    }
}

//std::shared_ptr<Expression> Parser::ParseTernaryOpExpression(std::shared_ptr<Expression> condition) {
//    GetNextToken();
//    auto ternary{MakeASTNode<TernaryOpExpression>()};
//    ternary->condition_ = std::move(condition);
//    ternary->lhs_ = ParseExpression();
//    Expect(TokenValue::KColon);
//    ternary->rhs_ = ParseExpression();
//    return ternary;
//}
//
//std::shared_ptr<Expression> Parser::HackExpression(std::shared_ptr<BinaryOpExpression> biop) {
////    auto op{GetNextToken().GetTokenValue()};
////    if (op == TokenValue::kAddAssign || op == TokenValue::kSubAssign ||
////            op == TokenValue::kMulAssign || op == TokenValue::kDivAssign ||
////            op == TokenValue::kModAssign || op == TokenValue::kShlAssign ||
////            op == TokenValue::kShrAssign) {
////        auto t = e->getToken();
////        op.pop_back();
////        t.tok = op;
////        Token t2 = t;
////        t2.tok = "=";
////        auto e2 = makeNode<BinaryExpression>(t2);
////        auto e3 = makeNode<BinaryExpression>(t);
////        e3->add(e->first());
////        e3->add(e->second());
////        e2->add(e->first());
////        e2->add(e3);
////        return e2;
////    } else {
////        //const folding
////        if (e->rhs()->kind() == Number().kind() && e->lhs()->kind() == Number().kind()) {
////            Token::Type ty;
////            double result;
////            double a, b;
////            if (e->rhs()->getToken().type == Token::Type::Int &&
////                    e->lhs()->getToken().type == Token::Type::Int) {
////                ty = Token::Type::Int;
////            } else {
////                ty = Token::Type::Float;
////            }
////            a = ((Number *) e->lhs())->getFloat();
////            b = ((Number *) e->rhs())->getFloat();
////            auto op = e->tok();
////            if (op == "+") { result = a + b; }
////            else if (op == "-") { result = a - b; }
////            else if (op == "*") { result = a * b; }
////            else if (op == "/") { result = a / b; }
////            else if (op == "%") { result = a / b; }
////            else { return e; }
////            if (ty == Token::Type::Int) {
////                return (BinaryExpression *) makeNode<Number>(
////                        Token(ty, format("{}", (int) result), e->getToken().line,
////                              e->getToken().col));
////            } else {
////                return (BinaryExpression *) makeNode<Number>(
////                        Token(ty, format("{}", result), e->getToken().line,
////                              e->getToken().col));
////            }
////
////        } else {
////            return e;
////        }
////    }
//}

//std::shared_ptr<IntConstant> Parser::ParseSizeof() {
//    Expect(TokenValue::kLeftParen);
//    auto type{ParseTypeSpecifier()};
//    if (!type) {
//        ErrorReportAndExit("expect a type name");
//    }
//    Expect(TokenValue::kRightParen);
//    if (type->type_ == TokenValue::kIntKeyword) {
//        return MakeASTNode<IntConstant>(4);
//    } else if (type->type_ == TokenValue::kDoubleKeyword) {
//        return MakeASTNode<IntConstant>(8);
//    } else if (type->type_ == TokenValue::kCharKeyword) {
//        return MakeASTNode<IntConstant>(1);
//    } else {
//        ErrorReportAndExit("Unknown type");
//        return nullptr;
//    }
//}

std::shared_ptr<IntConstant> Parser::ParseIntConstant() {
    auto result{MakeASTNode<IntConstant>(GetNextToken().GetInt32Value())};
    return result;
}

std::shared_ptr<DoubleConstant> Parser::ParseDoubleConstant() {
    auto result{MakeASTNode<DoubleConstant>(GetNextToken().GetDoubleValue())};
    return result;
}

std::shared_ptr<CharConstant> Parser::ParseCharConstant() {
    auto result{MakeASTNode<CharConstant>(GetNextToken().GetCharValue())};
    return result;
}

std::shared_ptr<StringLiteral> Parser::ParseStringLiteral() {
    auto result{MakeASTNode<StringLiteral>(GetNextToken().GetStringValue())};
    return result;
}

std::shared_ptr<Expression> Parser::ParseBinOpRHS(int ExprPrec, std::shared_ptr<Expression> LHS) {
    while (true) {
        int TokPrec = token_sequence_[index_ - 1].GetTokenPrecedence();

        // If this is a binop that binds at least as tightly as the current binop,
        // consume it, otherwise we are done.
        if (TokPrec < ExprPrec) {
            return LHS;
        }

        // Okay, we know this is a binop.
        auto BinOp = token_sequence_[index_ - 1].GetTokenValue();
        GetNextToken(); // eat binop

        // Parse the primary expression after the binary operator.
        auto RHS = ParsePrimary();
        if (!RHS) {
            return nullptr;
        }

        // If BinOp binds less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        int NextPrec = token_sequence_[index_ - 1].GetTokenPrecedence();
        if (TokPrec < NextPrec) {
            RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
            if (!RHS) {
                return nullptr;
            }
        }

        // Merge LHS/RHS.
        LHS = std::make_shared<BinaryOpExpression>(BinOp, std::move(LHS),
                                                   std::move(RHS));
    }
}

std::unique_ptr<Expression> Parser::ParseIdentifierExpression() {
    auto identifier{ParseIdentifier()};

    if (CurrentTokenIs(TokenValue::kRightParen)) {
        auto function_call{std::make_unique<FunctionCall>()};
        function_call->function_name_ = std::move(identifier);
        if (!CurrentTokenIs(TokenValue::kRightParen)) {
            while (true) {
                if (auto arg{ParseExpression()}) {
                    function_call->args_->push_back(std::move(arg));
                } else {
                    return nullptr;
                }

                if (CurrentTokenIs(TokenValue::kRightParen)) {
                    break;
                }

                if (!CurrentTokenIs(TokenValue::kComma)) {
                    ErrorReportAndExit("expect ) or ,");
                }
                GetNextToken();
            }
        }
        return function_call;
    } else {
        return std::make_unique<Identifier>(GetCurrentToken().GetTokenName());
    }
}

std::unique_ptr<Expression> Parser::ParseParenExpr() {
    GetNextToken();

    auto result{ParseExpression()};
    if (!result) {
        return nullptr;
    }

    ExpectCurrent(TokenValue::kRightParen);
    return result;
}

std::unique_ptr<Expression> Parser::ParsePrimary() {
    switch (GetCurrentToken().GetTokenType()) {
        case TokenType::kIdentifier:return ParseIdentifierExpression();
        case TokenType::kInterger:return ParseIntConstant();
        case TokenType::kDouble: return ParseDoubleConstant();
        case TokenType::kDelimiter:
            if (CurrentTokenIs(TokenValue::kLeftParen)) {
                return ParseParenExpr();
            }
            break;
        default:ErrorReportAndExit("expression error");
            break;
    }
    return nullptr;
}

std::unique_ptr<Expression> Parser::ParseBinOpRHS(std::int32_t precedence,
                                                  std::unique_ptr<Expression> lhs) {
    while (true) {
        std::int32_t token_precedence{GetCurrentToken().GetTokenPrecedence()};

        if (token_precedence < precedence) {
            return lhs;
        }

        char binop{GetCurrentToken().GetTokenName()[0]};
        GetNextToken();

        auto rhs{ParsePrimary()};
        if (!rhs) {
            return nullptr;
        }

        std::int32_t next_precedence{GetCurrentToken().GetTokenPrecedence()};
        if (token_precedence < next_precedence) {
            rhs = ParseBinOpRHS(token_precedence + 1, std::move(rhs));
            if (!rhs) {
                return nullptr;
            }
        }
        lhs = std::make_unique<BinaryOpExpression>(std::move(lhs), std::move(rhs), binop);
    }
}

}

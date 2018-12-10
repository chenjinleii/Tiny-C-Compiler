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

std::unique_ptr<Block> Parser::parse() {
    auto program_block{std::make_unique<Block>()};

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
        ErrorReportAndExit(value, PeekNextToken().GetTokenValue());
    }
}

std::unique_ptr<Statement> Parser::ParseGlobal() {
    std::unique_ptr<Statement> result;

    if (Test(TokenValue::kEnumKeyword)) {
        //result=;
        //TODO enum
    } else if (Test(TokenValue::kStructKeyword)) {
        //result=;
        //TODO struct
    } else {
        result = ParseDeclaration();
    }

    if (result->Kind() != ASTNodeType::kFunctionDefinition) {
        Expect(TokenValue::kSemicolon);
    }

    return result;
}

/*
 * declaration:
 *      declaration-specifiers init-declarator-list;
 *
 * declaration-specifiers:
 *      storage-class-specifier declaration-specifiers
 *      type-specifier declaration-specifiers
 *      type-qualifier declaration-specifiers
 *      function-specifier declaration-specifiers
 *
 * init-declarator-list:
 *      init-declarator
 *      init-declarator-list, init-declarator
 *
 * init-declarator:
 *      declarator
 *      declarator = initializer
 */

/*
 * 上述为C99的声明文法,有点复杂,先实现一个简化版本的
 *
 *
 *
 *
 */


std::unique_ptr<Statement> Parser::ParseDeclaration() {
    auto var_declaration_no_init{ParVarDeclarationNoInit()};

    if (CurrentTokenIs(TokenValue::kLeftParen)) {
        auto function{ParseFunctionDeclaration()};
        function->return_type_ = std::move(var_declaration_no_init->type_);
        function->function_name_ = std::move(var_declaration_no_init->variable_name_);
        return function;
    } else if (CurrentTokenIs(TokenValue::kEqual)) {
        auto initialization_expression{ParseExpression()};
        var_declaration_no_init->initialization_expression_ = std::move(initialization_expression);
        return var_declaration_no_init;
    } else {
        return var_declaration_no_init;
    }
}

std::unique_ptr<Identifier> Parser::ParseTypeSpecifier() {
    if (GetCurrentToken().IsTypeSpecifier()) {
        auto result{std::make_unique<Identifier>(GetCurrentToken().GetTokenName(), true)};
        GetNextToken();
        return result;
    } else {
        return nullptr;
    }
}

std::unique_ptr<Identifier> Parser::ParseIdentifier() {
    if (GetCurrentToken().IsIdentifier()) {
        auto result{std::make_unique<Identifier>(GetCurrentToken().GetTokenName(), false)};
        GetNextToken();
        return result;
    } else {
        return nullptr;
    }
}

std::unique_ptr<FunctionDeclaration> Parser::ParseFunctionDeclaration() {
    auto function{std::make_unique<FunctionDeclaration>()};

    while (HasNext() && !NextTokenIs(TokenValue::kRightParen)) {
        function->args_->push_back(ParVarDeclarationNoInit());
        ExpectCurrent(TokenValue::kComma);
    }

    GetNextToken();
    ExpectCurrent(TokenValue::kRightParen);

    if (CurrentTokenIs(TokenValue::kLeftCurly)) {
        GetNextToken();
        function->has_body_ = true;
        function->body_ = ParseBlock();
        ExpectCurrent(TokenValue::kRightCurly);
        return function;
    } else {
        return function;
    }
}

std::unique_ptr<Block> Parser::ParseBlock() {
    auto block{std::make_unique<Block>()};

    while (HasNext() && CurrentTokenIs(TokenValue::kRightCurly)) {
        block->statements_->push_back(ParseStatement());
    }

    return block;
}

std::unique_ptr<VariableDeclaration> Parser::ParVarDeclarationNoInit() {
    auto type{ParseTypeSpecifier()};
    if (!type) {
        ErrorReportAndExit("expect a type specifier");
    }

    auto identifier{ParseIdentifier()};
    if (!identifier) {
        ErrorReportAndExit("expect a identifier");
    }
    return std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier));
}

std::unique_ptr<VariableDeclaration> Parser::ParVarDeclarationWithInit() {
    auto var_declaration_no_init{ParVarDeclarationNoInit()};

    if (CurrentTokenIs(TokenValue::kEqual)) {
        auto initialization_expression{ParseExpression()};
        var_declaration_no_init->initialization_expression_ = std::move(initialization_expression);
    }

    ExpectCurrent(TokenValue::kSemicolon);

    return var_declaration_no_init;
}

std::unique_ptr<Statement> Parser::ParseStatement() {
    if (CurrentTokenIs(TokenValue::kIfKey)) {
        return ParseIfStatement();
    } else if (CurrentTokenIs(TokenValue::kWhileKey)) {
        return ParseWhileStatement();
    } else if (CurrentTokenIs(TokenValue::kForKey)) {
        return ParseForStatement();
    } else if (CurrentTokenIs(TokenValue::kReturnKey)) {
        return ParseReturnStatement();
    } else if (GetCurrentToken().IsTypeSpecifier()) {
        return ParVarDeclarationWithInit();
    } else {
        return ParseExpressionStatement();
    }
}

std::unique_ptr<IfStatement> Parser::ParseIfStatement() {
    GetNextToken();
    auto if_statement{std::make_unique<IfStatement>()};

    ExpectCurrent(TokenValue::kLeftParen);
    if_statement->condition_ = ParseExpression();
    ExpectCurrent(TokenValue::kRightParen);

    // 强制要求有大括号
    ExpectCurrent(TokenValue::kLeftCurly);
    if_statement->then_block_ = ParseBlock();
    ExpectCurrent(TokenValue::kRightCurly);

    if (CurrentTokenIs(TokenValue::kElseKey)) {
        GetNextToken();
        ExpectCurrent(TokenValue::kLeftCurly);
        if_statement->else_block_ = ParseBlock();
        ExpectCurrent(TokenValue::kRightCurly);
    }

    return if_statement;
}

std::unique_ptr<WhileStatement> Parser::ParseWhileStatement() {
    GetNextToken();
    auto while_statement{std::unique_ptr<WhileStatement>()};

    ExpectCurrent(TokenValue::kLeftParen);
    while_statement->condition_ = ParseExpression();
    ExpectCurrent(TokenValue::kRightParen);

    // 强制要求有大括号
    ExpectCurrent(TokenValue::kLeftCurly);
    while_statement->block_ = ParseBlock();
    ExpectCurrent(TokenValue::kRightCurly);

    return while_statement;
}

std::unique_ptr<ForStatement> Parser::ParseForStatement() {
    GetNextToken();
    auto for_statement{std::make_unique<ForStatement>()};

    ExpectCurrent(TokenValue::kLeftParen);
    //TODO 支持声明变量
    for_statement->initial_ = ParseExpression();
    ExpectCurrent(TokenValue::kSemicolon);

    for_statement->condition_ = ParseExpression();
    ExpectCurrent(TokenValue::kSemicolon);
    for_statement->increment_ = ParseExpression();
    ExpectCurrent(TokenValue::kRightParen);

    // 强制要求有大括号
    ExpectCurrent(TokenValue::kLeftCurly);
    for_statement->block_ = ParseBlock();
    ExpectCurrent(TokenValue::kRightCurly);

    return for_statement;
}

std::unique_ptr<ReturnStatement> Parser::ParseReturnStatement() {
    auto return_statement{std::make_unique<ReturnStatement>()};

    if (CurrentTokenIs(TokenValue::kSemicolon)) {
        GetNextToken();
        return return_statement;
    } else {
        GetNextToken();
        return_statement->expression_ = ParseExpression();
        ExpectCurrent(TokenValue::kSemicolon);
        return return_statement;
    }
}

std::unique_ptr<ExpressionStatement> Parser::ParseExpressionStatement() {
    auto expression_statement{std::make_unique<ExpressionStatement>()};

    expression_statement->expression_ = ParseExpression();
    ExpectCurrent(TokenValue::kSemicolon);

    return expression_statement;
}

// TODO 强制类型转换,?:运算符,一元运算符(除调用运算符)
std::unique_ptr<Expression> Parser::ParseExpression() {
    auto lhs{ParsePrimary()};
    if (!lhs) {
        return nullptr;
    }

    return ParseBinOpRHS(0, std::move(lhs));
}

// TODO 更多类型
std::unique_ptr<Expression> Parser::ParsePrimary() {
    switch (GetCurrentToken().GetTokenType()) {
        case TokenType::kIdentifier:return ParseIdentifierExpression();
        case TokenType::kInterger:return ParseInteger();
        case TokenType::kDouble: return ParseDouble();
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

std::unique_ptr<Integer> Parser::ParseInteger() {
    auto result{std::make_unique<Integer>(GetCurrentToken().GetInt32Value())};
    GetNextToken();
    return result;
}

std::unique_ptr<Double> Parser::ParseDouble() {
    auto result{std::make_unique<Double>(GetCurrentToken().GetDoubleValue())};
    GetNextToken();
    return result;
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

}

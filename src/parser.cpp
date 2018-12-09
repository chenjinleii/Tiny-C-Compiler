//
// Created by kaiser on 18-12-8.
//

#include "parser.h"

#include <iostream>
#include <cstdlib>

std::unique_ptr<Block> Parser::parse() {
    auto program_block{std::make_unique<Block>()};

    if (std::size(token_sequence_) == 0) {
        ErrorReportAndExit("no token");
    }

    while (HasNext()) {
        program_block->statements_->push_back(ParseGlobal());
    }

    return program_block;
}

void Parser::ErrorReportAndExit(const std::string &msg) {
    std::cerr << msg << '\n';
    std::exit(EXIT_FAILURE);
}

Token Parser::GetCurrentToken() {
    return token_sequence_[index_];
}

Token Parser::GetNextToken() {
    return token_sequence_[++index_];
}

Token Parser::PeekNextToken() {
    return token_sequence_[index_ + 1];
}

bool Parser::Try(TokenValue value) {
    if (PeekNextToken().GetTokenValue() == value) {
        GetNextToken();
        return true;
    } else {
        return false;
    }
}

void Parser::Expect(TokenValue value) {
    if (GetNextToken().GetTokenValue() != value) {
        ErrorReportAndExit("expect error");
    } else {
        GetNextToken();
    }
}

bool Parser::HasNext() const {
    return index_ + 1 < std::size(token_sequence_);
}

std::unique_ptr<Statement> Parser::ParseGlobal() {
    std::unique_ptr<Statement> result;
    if (Try(TokenValue::kEnumKey)) {
        //result=;
        //TODO enum
    } else if (Try(TokenValue::kStructKey)) {
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

// TODO 目前只支持形如 int a;的声明,其他待完成
std::unique_ptr<Statement> Parser::ParseDeclaration() {
    auto var_declaration_no_init{ParVarDeclarationNoInit()};

    if (Try(TokenValue::kLeftParen)) {
        auto function{ParseFunctionDeclaration()};
        function->return_type_ = std::move(var_declaration_no_init->type_);
        function->function_name_ = std::move(var_declaration_no_init->variable_name_);
        return function;
    } else if (Try(TokenValue::kEqual)) {
        auto initialization_expression{ParseExpression()};
        var_declaration_no_init->initialization_expression_ = std::move(initialization_expression);
        return var_declaration_no_init;
    } else {
        return var_declaration_no_init;
    }
}

std::unique_ptr<IdentifierOrType> Parser::ParseTypeSpecifier() {
    if (PeekNextToken().IsTypeSpecifier()) {
        GetNextToken();
        return std::make_unique<IdentifierOrType>
                (GetCurrentToken().GetTokenName(), true);
    } else {
        return nullptr;
    }
}

std::unique_ptr<IdentifierOrType> Parser::ParseIdentifier() {
    if (PeekNextToken().IsIdentifier()) {
        GetNextToken();
        return std::make_unique<IdentifierOrType>
                (GetCurrentToken().GetTokenName(), false);
    } else {
        return nullptr;
    }
}

std::unique_ptr<FunctionDeclaration> Parser::ParseFunctionDeclaration() {
    auto function{std::make_unique<FunctionDeclaration>()};

    while (HasNext() && PeekNextToken().GetTokenValue() != TokenValue::kRightParen) {
        function->args_->push_back(ParVarDeclarationNoInit());
        Expect(TokenValue::kComma);
    }
    Expect(TokenValue::kRightParen);

    if (Try(TokenValue::kLeftCurly)) {
        function->has_body_ = true;
        function->body_ = ParseBlock();
        Expect(TokenValue::kRightCurly);
        return function;
    } else {
        return function;
    }
}

std::unique_ptr<Block> Parser::ParseBlock() {
    auto block{std::make_unique<Block>()};

    while (HasNext() && PeekNextToken().GetTokenValue() != TokenValue::kRightCurly) {
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

    if (Try(TokenValue::kEqual)) {
        auto initialization_expression{ParseExpression()};
        var_declaration_no_init->initialization_expression_ = std::move(initialization_expression);
    }
    Expect(TokenValue::kSemicolon);
    return var_declaration_no_init;
}

std::unique_ptr<Statement> Parser::ParseStatement() {
    if (Try(TokenValue::kIfKey)) {
        return ParseIfStatement();
    } else if (Try(TokenValue::kWhileKey)) {
        return ParseWhileStatement();
    } else if (Try(TokenValue::kForKey)) {
        return ParseForStatement();
    } else if (Try(TokenValue::kReturnKey)) {
        return ParseReturnStatement();
    } else if (PeekNextToken().IsTypeSpecifier()) {
        return ParVarDeclarationWithInit();
    } else {
        return ParseExpressionStatement();
    }
}

std::unique_ptr<IfStatement> Parser::ParseIfStatement() {
    auto if_statement{std::make_unique<IfStatement>()};

    Expect(TokenValue::kLeftParen);
    if_statement->condition_ = ParseExpression();
    Expect(TokenValue::kRightParen);

    // 强制要求有大括号
    Expect(TokenValue::kLeftCurly);
    if_statement->then_block_ = ParseBlock();
    Expect(TokenValue::kRightCurly);

    if (Try(TokenValue::kElseKey)) {
        Expect(TokenValue::kLeftCurly);
        if_statement->else_block_ = ParseBlock();
        Expect(TokenValue::kRightCurly);
    }

    return if_statement;
}

std::unique_ptr<WhileStatement> Parser::ParseWhileStatement() {
    auto while_statement{std::unique_ptr<WhileStatement>()};

    Expect(TokenValue::kLeftParen);
    while_statement->condition_ = ParseExpression();
    Expect(TokenValue::kRightParen);

    // 强制要求有大括号
    Expect(TokenValue::kLeftCurly);
    while_statement->block_ = ParseBlock();
    Expect(TokenValue::kRightCurly);

    return while_statement;
}

std::unique_ptr<ForStatement> Parser::ParseForStatement() {
    auto for_statement{std::make_unique<ForStatement>()};

    Expect(TokenValue::kLeftParen);
    //TODO 支持声明变量
    for_statement->initial_ = ParseExpression();
    Expect(TokenValue::kSemicolon);

    for_statement->condition_ = ParseExpression();
    Expect(TokenValue::kSemicolon);
    for_statement->increment_ = ParseExpression();
    Expect(TokenValue::kRightParen);

    // 强制要求有大括号
    Expect(TokenValue::kLeftCurly);
    for_statement->block_ = ParseBlock();
    Expect(TokenValue::kRightCurly);

    return for_statement;
}

std::unique_ptr<ReturnStatement> Parser::ParseReturnStatement() {
    auto return_statement{std::make_unique<ReturnStatement>()};

    if (Try(TokenValue::kSemicolon)) {
        return return_statement;
    } else {
        return_statement->expression_ = ParseExpression();
        Expect(TokenValue::kSemicolon);
        return return_statement;
    }
}

std::unique_ptr<ExpressionStatement> Parser::ParseExpressionStatement() {
    auto expression_statement{std::make_unique<ExpressionStatement>()};

    expression_statement->expression_ = ParseExpression();
    Expect(TokenValue::kSemicolon);

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
            if (GetCurrentToken().GetTokenValue() == TokenValue::kLeftParen) {
                return ParseParenExpr();
            }
            break;
        default:ErrorReportAndExit("expression error");
            break;
    }
    return nullptr;
}

std::unique_ptr<Expression> Parser::ParseBinOpRHS(std::int32_t precedence,
                                                  std::unique_ptr<Expression> rhs) {

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

    if (Try(TokenValue::kRightParen)) {

    } else {
        return std::make_unique<IdentifierOrType>(GetCurrentToken().GetTokenName());
    }
}

std::unique_ptr<Expression> Parser::ParseParenExpr() {
    GetNextToken();

    auto result{ParseExpression()};
    if (!result) {
        return nullptr;
    }

    Expect(TokenValue::kRightParen);
    return result;
}
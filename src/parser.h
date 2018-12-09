//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_PARSER_H
#define TINY_C_COMPILER_PARSER_H

#include "ast.h"
#include "token.h"

#include <memory>
#include <vector>
#include <string>
#include <cstdint>

class Parser {
public:
    explicit Parser(const std::vector<Token> &token_sequence) :
            token_sequence_{token_sequence} {}
    std::unique_ptr<Block> parse();
private:
    void ErrorReportAndExit(const std::string &msg);
    bool HasNext() const;
    Token GetCurrentToken();
    Token GetNextToken();
    Token PeekNextToken();

    bool CurrentTokenIs(TokenValue value);
    bool NextTokenIs(TokenValue value);
    void ExpectCurrent(TokenValue value);
    void ExpectNext(TokenValue value);

    std::unique_ptr<Statement> ParseGlobal();
    std::unique_ptr<Statement> ParseDeclaration();
    std::unique_ptr<IdentifierOrType> ParseTypeSpecifier();
    std::unique_ptr<IdentifierOrType> ParseIdentifier();

    std::unique_ptr<FunctionDeclaration> ParseFunctionDeclaration();
    std::unique_ptr<Block> ParseBlock();
    std::unique_ptr<VariableDeclaration> ParVarDeclarationNoInit();
    std::unique_ptr<VariableDeclaration> ParVarDeclarationWithInit();
    std::unique_ptr<Statement> ParseStatement();
    std::unique_ptr<IfStatement> ParseIfStatement();
    std::unique_ptr<WhileStatement> ParseWhileStatement();
    std::unique_ptr<ForStatement> ParseForStatement();
    std::unique_ptr<ReturnStatement> ParseReturnStatement();
    std::unique_ptr<ExpressionStatement> ParseExpressionStatement();

    std::unique_ptr<Expression> ParseExpression();
    std::unique_ptr<Expression> ParsePrimary();
    std::unique_ptr<Expression> ParseBinOpRHS(std::int32_t precedence,
                                              std::unique_ptr<Expression> lhs);
    std::unique_ptr<Integer> ParseInteger();
    std::unique_ptr<Double> ParseDouble();
    std::unique_ptr<Expression> ParseIdentifierExpression();
    std::unique_ptr<Expression> ParseParenExpr();

    std::vector<Token> token_sequence_;
    std::vector<Token>::size_type index_{};

    inline static Token eof_token_{Token{TokenType::kEof, TokenValue::kUnreserved, -1, "eof"}};
};

#endif //TINY_C_COMPILER_PARSER_H
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

namespace tcc {

class Parser {
public:
    explicit Parser(const std::vector<Token> &token_sequence) :
            token_sequence_{token_sequence} {}
    std::unique_ptr<Block> parse();
private:
    Token GetNextToken();
    Token PeekNextToken() const;
    bool HasNextToken() const;
    bool Try(TokenValue value);
    bool Test(TokenValue value);
    void Expect(TokenValue value);

    std::unique_ptr<Statement> ParseGlobal();
    std::unique_ptr<Statement> ParseDeclaration();
    std::unique_ptr<Identifier> ParseTypeSpecifier();
    std::unique_ptr<Identifier> ParseIdentifier();

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

    std::unique_ptr<EnumDeclaration> ParseEnumDeclaration();
    std::unique_ptr<StructDeclaration> ParseStructDeclaration();

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
};

}

#endif //TINY_C_COMPILER_PARSER_H

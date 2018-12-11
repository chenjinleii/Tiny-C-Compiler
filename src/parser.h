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
    std::shared_ptr<CompoundStatement> parse();
private:
    Token GetNextToken();
    Token PeekNextToken() const;
    bool HasNextToken() const;
    bool Try(TokenValue value);
    bool Test(TokenValue value);
    void Expect(TokenValue value);

    std::shared_ptr<Statement> ParseGlobal();
    std::shared_ptr<Statement> ParseDeclaration();
    std::shared_ptr<PrimitiveType> ParseTypeSpecifier();
    std::shared_ptr<Identifier> ParseIdentifier();
    std::shared_ptr<FunctionDeclaration> ParseFunctionDeclaration();

    std::shared_ptr<CompoundStatement> ParseCompound();
    std::shared_ptr<Declaration> ParVarDeclarationNoInit();
    std::shared_ptr<Declaration> ParVarDeclarationWithInit();
    std::shared_ptr<Statement> ParseStatement();
    std::shared_ptr<IfStatement> ParseIfStatement();
    std::shared_ptr<WhileStatement> ParseWhileStatement();
    std::shared_ptr<ForStatement> ParseForStatement();
    std::shared_ptr<ReturnStatement> ParseReturnStatement();
    std::shared_ptr<ExpressionStatement> ParseExpressionStatement();

    std::shared_ptr<Expression> ParseExpression();
    std::shared_ptr<Expression> ParsePrimary();
    std::shared_ptr<CharConstant> ParseCharConstant();
    std::shared_ptr<IntConstant> ParseIntConstant();
    std::shared_ptr<DoubleConstant> ParseDoubleConstant();
    std::shared_ptr<StringLiteral> ParseStringLiteral();

    std::shared_ptr<Expression> ParseUnaryOpExpression();
    std::shared_ptr<Expression> ParsePostfixExpression();
    std::shared_ptr<Expression> ParseTernaryOpExpression(std::shared_ptr<Expression> condition);
    std::shared_ptr<Expression> HackExpression(std::shared_ptr<BinaryOpExpression> biop);
    std::shared_ptr<IntConstant> ParseSizeof();

    std::shared_ptr<Expression> ParseBinOpRHS(int ExprPrec,
                                              std::shared_ptr<Expression> LHS);

    std::vector<Token> token_sequence_;
    std::vector<Token>::size_type index_{};

    template<typename T, typename... Args>
    std::shared_ptr<T> MakeASTNode(Args... args) {
        auto t{std::make_unique<T>(args...)};
        t->location_ = PeekNextToken().GetTokenLocation();
        return std::move(t);
    }
};

}

#endif //TINY_C_COMPILER_PARSER_H

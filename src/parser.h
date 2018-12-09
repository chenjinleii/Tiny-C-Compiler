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

class Parser {
public:
    explicit Parser(const std::vector<Token> &token_sequence) :
            token_sequence_{token_sequence} {}
    std::unique_ptr<Block> parse();
private:
    void ErrorReportAndExit(const std::string &msg);
    Token GetCurrentToken();
    Token GetNextToken();
    Token PeekNextToken();
    bool Try(TokenValue value);
    void Expect(TokenValue value);
    bool HasNext() const;

    std::unique_ptr<Statement> ParseGlobal();
    std::unique_ptr<Statement> ParseDeclaration();
    std::unique_ptr<IdentifierOrType> ParseTypeSpecifier();
    std::unique_ptr<IdentifierOrType> ParseIdentifier();
    std::unique_ptr<Expression> ParseExpression();
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

    std::vector<Token> token_sequence_;
    std::vector<Token>::size_type index_{};
};

#endif //TINY_C_COMPILER_PARSER_H
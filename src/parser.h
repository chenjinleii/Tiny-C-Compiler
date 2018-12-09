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
    void ErrorReport(const std::string &msg);
    Token GetCurrentToken();
    Token GetNextToken();
    Token PeekNextToken();
    bool Try(TokenValue value);
    void Expect(TokenValue value);
    bool HasNext() const;

    std::unique_ptr<Statement> ParseGlobal();
    std::unique_ptr<Statement> ParseDeclaration();
    std::unique_ptr<IdentifierOrType> ParseTypeSpecifier();

    std::vector<Token> token_sequence_;
    std::vector<Token>::size_type index_{};

    std::unique_ptr<Block> program_block_{std::make_unique<Block>()};
};

#endif //TINY_C_COMPILER_PARSER_H
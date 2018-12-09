//
// Created by kaiser on 18-12-8.
//

#include "parser.h"

#include <iostream>
#include <cstdlib>

std::unique_ptr<Block> Parser::parse() {
    while (HasNext()) {
        program_block_->statements_->push_back(ParseGlobal());
    }

    return std::move(program_block_);
}

void Parser::ErrorReport(const std::string &msg) {
    std::cerr << msg << '\n';
    std::exit(EXIT_FAILURE);
}

Token Parser::GetCurrentToken() {
    return token_sequence_[index_];
}

Token Parser::GetNextToken() {
    return token_sequence_[index_++];
}

Token Parser::PeekNextToken() {
    return token_sequence_[index_];
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
        std::cerr << "expect error\n";
        std::exit(EXIT_FAILURE);
    }
}

bool Parser::HasNext() const {
    return index_ < std::size(token_sequence_);
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

std::unique_ptr<Statement> Parser::ParseDeclaration() {
    auto type{ParseTypeSpecifier()};

}

std::unique_ptr<IdentifierOrType> Parser::ParseTypeSpecifier() {

}

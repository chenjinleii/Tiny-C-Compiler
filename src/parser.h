//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_PARSER_H
#define TINY_C_COMPILER_PARSER_H

#include "ast.h"
#include "token.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace tcc {

class Parser {
 public:
  explicit Parser(std::vector<Token> token_sequence)
      : token_sequence_{std::move(token_sequence)} {}
  std::shared_ptr<CompoundStatement> Parse();
  static std::shared_ptr<CompoundStatement> Test(
      std::vector<Token> token_sequence, std::ostream &os = std::cout);

 private:
  Token GetNextToken();
  Token PeekNextToken() const;
  bool HasNextToken() const;
  bool Try(TokenValue value);
  bool Test(TokenValue value);
  void Expect(TokenValue expect);

  std::shared_ptr<Statement> ParseGlobal();
  std::shared_ptr<Statement> ParseDeclaration();
  std::shared_ptr<PrimitiveType> ParseTypeSpecifier();
  std::shared_ptr<Identifier> ParseIdentifier();
  std::shared_ptr<FunctionDeclaration> ParseFunction();
  std::shared_ptr<FunctionDeclaration> ParseExtern();

  std::shared_ptr<Statement> ParseCompound(bool is_func);
  std::shared_ptr<Declaration> ParseDeclarationWithoutInit();
  std::shared_ptr<Declaration> ParseDeclarationWithInit();
  std::shared_ptr<Statement> ParseStatement();
  std::shared_ptr<IfStatement> ParseIfStatement();
  std::shared_ptr<WhileStatement> ParseWhileStatement();
  std::shared_ptr<ForStatement> ParseForStatement();
  std::shared_ptr<ReturnStatement> ParseReturnStatement();
  std::shared_ptr<ExpressionStatement> ParseExpressionStatement();

  std::shared_ptr<Expression> ParseExpression();
  std::shared_ptr<Expression> ParsePrimary();
  std::shared_ptr<Expression> ParseUnaryOpExpression();
  std::shared_ptr<Int32Constant> ParseSizeof();
  std::shared_ptr<Expression> ParseParenExpression();
  std::shared_ptr<Expression> ParseIdentifierExpression();
  std::shared_ptr<Expression> ParseBinOpRhs(std::int32_t precedence,
                                            std::shared_ptr<Expression> lhs);
  std::shared_ptr<CharConstant> ParseCharConstant();
  std::shared_ptr<Int32Constant> ParseInt32Constant();
  std::shared_ptr<DoubleConstant> ParseDoubleConstant();
  std::shared_ptr<StringLiteral> ParseStringLiteral();

  std::vector<Token> token_sequence_;
  std::vector<Token>::size_type index_{};

  template<typename T, typename... Args>
  std::shared_ptr<T> MakeASTNode(Args... args) {
    auto t{std::make_shared<T>(args...)};
    t->location_ = PeekNextToken().GetTokenLocation();
    return std::move(t);
  }
};

}  // namespace tcc

#endif  // TINY_C_COMPILER_PARSER_H

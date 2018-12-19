//
// Created by kaiser on 18-12-8.
//

#include "location.h"
#include "error.h"
#include "parser.h"

#include <cstdlib>

namespace tcc {

std::shared_ptr<CompoundStatement> Parser::Parse() {
  auto root{MakeASTNode<CompoundStatement>()};

  if (std::size(token_sequence_) == 0) {
    ErrorReportAndExit("no token");
  }

  while (HasNextToken()) {
    root->AddStatement(ParseGlobal());
  }

  return root;
}

std::shared_ptr<CompoundStatement> Parser::Test(std::vector<Token> token_sequence,
                                                std::ostream &os) {
  auto ast_root{Parser{std::move(token_sequence)}.Parse()};
  auto json_root{ast_root->JsonGen()};

  os << json_root;
  std::cout << "AST Successfully Written\n";
  return ast_root;
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
  auto result{MakeASTNode<Statement>()};
  if (Try(TokenValue::kExtern)) {
    result = ParseExtern();
  } else {
    result = ParseDeclaration();
  }

  if (result->Kind() != ASTNodeType::kFunctionDefinition) {
    Expect(TokenValue::kSemicolon);
  }

  return result;
}

std::shared_ptr<Statement> Parser::ParseDeclaration() {
  auto declaration_no_init{ParDeclarationWithoutInit()};

  if (Try(TokenValue::kLeftParen)) {
    auto function{ParseFunctionDeclaration()};
    function->return_type_ = std::move(declaration_no_init->type_);
    function->name_ = std::move(declaration_no_init->name_);
    return function;
  } else if (Try(TokenValue::kAssign)) {
    auto initialization_expression{ParseExpression()};
    declaration_no_init->init_ = std::move(initialization_expression);
    return declaration_no_init;
  } else {
    return declaration_no_init;
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
    function->AddArg(ParDeclarationWithoutInit());
    if (!Test(TokenValue::kRightParen)) {
      Expect(TokenValue::kComma);
    } else {
      break;
    }
  }

  Expect(TokenValue::kRightParen);

  if (Try(TokenValue::kLeftCurly)) {
    function->body_ = ParseCompound(true);
    Expect(TokenValue::kRightCurly);
    return function;
  } else {
    return function;
  }
}

std::shared_ptr<FunctionDeclaration> Parser::ParseExtern() {
  auto function{MakeASTNode<FunctionDeclaration>()};

  auto return_type{ParseTypeSpecifier()};
  if (!return_type) {
    ErrorReportAndExit(PeekNextToken().GetTokenLocation(), "expect a type specifier");
  }
  function->return_type_ = return_type;

  auto function_name{ParseIdentifier()};
  if (!function_name) {
    ErrorReportAndExit(PeekNextToken().GetTokenLocation(), "expect a identifier");
  }
  function->name_ = function_name;

  Expect(TokenValue::kLeftParen);

  while (!Test(TokenValue::kRightParen)) {
    function->AddArg(ParDeclarationWithoutInit());
    if (!Test(TokenValue::kRightParen)) {
      Expect(TokenValue::kComma);
    } else {
      break;
    }
  }

  Expect(TokenValue::kRightParen);

  return function;
}

std::shared_ptr<Statement> Parser::ParseCompound(bool is_func) {
  if (!is_func) {
    if (Try(TokenValue::kLeftCurly)) {
      auto compound{MakeASTNode<CompoundStatement>()};
      while (!Test(TokenValue::kRightCurly)) {
        compound->AddStatement(ParseStatement());
      }
      Expect(TokenValue::kRightCurly);
      return compound;
    } else {
      return ParseStatement();
    }
  } else {
    auto compound{MakeASTNode<CompoundStatement>()};
    while (!Test(TokenValue::kRightCurly)) {
      compound->AddStatement(ParseStatement());
    }
    return compound;
  }
}

std::shared_ptr<Declaration> Parser::ParDeclarationWithoutInit() {
  auto type{ParseTypeSpecifier()};
  if (!type) {
    ErrorReportAndExit("expect a type specifier");
  }

  auto identifier{ParseIdentifier()};
  if (!identifier) {
    ErrorReportAndExit("expect a identifier");
  }
  return MakeASTNode<Declaration>(type, identifier);
}

std::shared_ptr<Declaration> Parser::ParDeclarationWithInit() {
  auto var_declaration_no_init{ParDeclarationWithoutInit()};

  if (Try(TokenValue::kAssign)) {
    var_declaration_no_init->init_ = ParseExpression();
  }
  Expect(TokenValue::kSemicolon);

  return var_declaration_no_init;
}

std::shared_ptr<Statement> Parser::ParseStatement() {
  if (Try(TokenValue::kIf)) {
    return ParseIfStatement();
  } else if (Try(TokenValue::kWhile)) {
    return ParseWhileStatement();
  } else if (Try(TokenValue::kFor)) {
    return ParseForStatement();
  } else if (Try(TokenValue::kReturn)) {
    return ParseReturnStatement();
  } else if (PeekNextToken().IsTypeSpecifier()) {
    return ParDeclarationWithInit();
  } else if (Test(TokenValue::kLeftCurly)) {
    auto compound{ParseCompound(false)};
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

  if_statement->then_block_ = ParseCompound(false);

  if (Try(TokenValue::kElse)) {
    if_statement->else_block_ = ParseCompound(false);
  }

  return if_statement;
}

std::shared_ptr<WhileStatement> Parser::ParseWhileStatement() {
  auto while_statement{MakeASTNode<WhileStatement>()};

  Expect(TokenValue::kLeftParen);
  while_statement->cond_ = ParseExpression();
  Expect(TokenValue::kRightParen);

  while_statement->block_ = ParseCompound(false);

  return while_statement;
}

std::shared_ptr<ForStatement> Parser::ParseForStatement() {
  auto for_statement{MakeASTNode<ForStatement>()};

  Expect(TokenValue::kLeftParen);

  if (PeekNextToken().IsTypeSpecifier()) {
    for_statement->declaration_ = ParDeclarationWithInit();
  } else {
    for_statement->init_ = ParseExpression();
    Expect(TokenValue::kSemicolon);
  }

  for_statement->cond_ = ParseExpression();
  Expect(TokenValue::kSemicolon);
  for_statement->increment_ = ParseExpression();
  Expect(TokenValue::kRightParen);

  for_statement->block_ = ParseCompound(false);

  return for_statement;
}

std::shared_ptr<ReturnStatement> Parser::ParseReturnStatement() {
  auto return_statement{MakeASTNode<ReturnStatement>()};
  if (Try(TokenValue::kSemicolon)) {
    return return_statement;
  } else {
    return_statement->expression_ = ParseExpression();
    Expect(TokenValue::kSemicolon);
  }

  return return_statement;
}

std::shared_ptr<ExpressionStatement> Parser::ParseExpressionStatement() {
  auto expression_statement{std::make_shared<ExpressionStatement>()};

  expression_statement->expression_ = ParseExpression();
  Expect(TokenValue::kSemicolon);

  return expression_statement;
}

std::shared_ptr<Expression> Parser::ParseExpression() {
  auto lhs{ParsePrimary()};
  if (!lhs) {
    return nullptr;
  }

  return ParseBinOpRHS(0, std::move(lhs));
}

std::shared_ptr<Expression> Parser::ParsePrimary() {
  if (Test(TokenValue::kIdentifier)) {
    return ParseIdentifierExpression();
  } else if (Test(TokenValue::kIntConstant)) {
    return ParseInt32Constant();
  } else if (Test(TokenValue::kCharConstant)) {
    return ParseCharConstant();
  } else if (Test(TokenValue::kDoubleConstant)) {
    return ParseDoubleConstant();
  } else if (Test(TokenValue::kStringLiteral)) {
    return ParseStringLiteral();
  } else if (Try(TokenValue::kLeftParen)) {
    return ParseParenExpression();
  } else if (Test(TokenValue::kAdd) || Test(TokenValue::kSub) ||
      Test(TokenValue::kInc) || Test(TokenValue::kDec) ||
      Test(TokenValue::kNeg) || Test(TokenValue::kLogicNeg)) {
    return ParseUnaryOpExpression();
  } else if (Try(TokenValue::kSizeof)) {
    return ParseSizeof();
  } else {
    return nullptr;
  }
}

std::shared_ptr<Expression> Parser::ParseUnaryOpExpression() {
  auto op{GetNextToken()};

  if (PeekNextToken().IsIdentifier()) {
    return MakeASTNode<UnaryOpExpression>(ParseIdentifierExpression(), op.GetTokenValue());
  } else if (PeekNextToken().IsChar()) {
    if (op.TokenValueIs(TokenValue::kAdd)) {
      return MakeASTNode<Int32Constant>(ParseCharConstant()->value_ + 1);
    } else if (op.TokenValueIs(TokenValue::kSub)) {
      return MakeASTNode<Int32Constant>(ParseCharConstant()->value_ - 1);
    } else if (op.TokenValueIs(TokenValue::kNeg)) {
      return MakeASTNode<Int32Constant>(~(ParseCharConstant()->value_));
    } else if (op.TokenValueIs(TokenValue::kLogicNeg)) {
      return MakeASTNode<Int32Constant>(ParseCharConstant()->value_ ? 0 : 1);
    } else {
      ErrorReportAndExit("Cannot apply inc, dec to constants");
      return nullptr;
    }
  } else if (PeekNextToken().IsInt32()) {
    if (op.TokenValueIs(TokenValue::kAdd)) {
      return MakeASTNode<Int32Constant>(+ParseInt32Constant()->value_);
    } else if (op.TokenValueIs(TokenValue::kSub)) {
      return MakeASTNode<Int32Constant>(-ParseInt32Constant()->value_);
    } else if (op.TokenValueIs(TokenValue::kNeg)) {
      return MakeASTNode<Int32Constant>(~(ParseInt32Constant()->value_));
    } else if (op.TokenValueIs(TokenValue::kLogicNeg)) {
      return MakeASTNode<Int32Constant>(ParseInt32Constant()->value_ ? 0 : 1);
    } else {
      ErrorReportAndExit("Cannot apply inc, dec to constants");
      return nullptr;
    }
  } else if (PeekNextToken().IsDouble()) {
    if (op.TokenValueIs(TokenValue::kAdd)) {
      return MakeASTNode<DoubleConstant>(+ParseDoubleConstant()->value_);
    } else if (op.TokenValueIs(TokenValue::kSub)) {
      return MakeASTNode<DoubleConstant>(-ParseDoubleConstant()->value_);
    } else if (op.TokenValueIs(TokenValue::kLogicNeg)) {
      return MakeASTNode<Int32Constant>(static_cast<bool> (ParseDoubleConstant()->value_));
    } else {
      ErrorReportAndExit("Cannot apply inc, dec to constants");
      return nullptr;
    }
  } else {
    ErrorReportAndExit("Cannot apply");
    return nullptr;
  }
}

// 强制要求有括号
std::shared_ptr<Int32Constant> Parser::ParseSizeof() {
  Expect(TokenValue::kLeftParen);
  auto type{ParseTypeSpecifier()};
  if (!type) {
    ErrorReportAndExit("expect a type name");
  }
  Expect(TokenValue::kRightParen);
  if (type->type_ == TokenValue::kInt) {
    return MakeASTNode<Int32Constant>(4);
  } else if (type->type_ == TokenValue::kDouble) {
    return MakeASTNode<Int32Constant>(8);
  } else if (type->type_ == TokenValue::kChar) {
    return MakeASTNode<Int32Constant>(1);
  } else {
    ErrorReportAndExit("Unknown type");
    return nullptr;
  }
}

std::shared_ptr<Expression> Parser::ParseParenExpression() {
  auto expr{ParseExpression()};
  if (!expr) {
    return nullptr;
  }

  Expect(TokenValue::kRightParen);
  return expr;
}

std::shared_ptr<Expression> Parser::ParseIdentifierExpression() {
  auto identifier{ParseIdentifier()};

  if (Try(TokenValue::kLeftParen)) {
    auto function_call{MakeASTNode<FunctionCall>()};
    function_call->name_ = std::move(identifier);
    function_call->args_ = std::make_shared<ExpressionList>();
    if (!Test(TokenValue::kRightParen)) {
      while (true) {
        if (auto arg{ParseExpression()}) {
          function_call->AddArg(std::move(arg));
        } else {
          return nullptr;
        }

        if (Test(TokenValue::kRightParen)) {
          break;
        }

        if (!Test(TokenValue::kComma)) {
          ErrorReportAndExit("expect ) or ,");
        }
        GetNextToken();
      }
    }
    Expect(TokenValue::kRightParen);
    return function_call;
  } else if (Try(TokenValue::kInc)) {
    return MakeASTNode<PostfixExpression>(std::move(identifier), TokenValue::kInc);
  } else if (Try(TokenValue::kDec)) {
    return MakeASTNode<PostfixExpression>(std::move(identifier), TokenValue::kDec);
  } else {
    return identifier;
  }
}

// precedence 表示该函数允许吃的最小运算符优先级
std::shared_ptr<Expression> Parser::ParseBinOpRHS(std::int32_t precedence,
                                                  std::shared_ptr<Expression> lhs) {
  while (true) {
    std::int32_t curr_precedence = PeekNextToken().GetTokenPrecedence();
    if (curr_precedence < precedence) {
      return lhs;
    }

    auto op = GetNextToken().GetTokenValue();
    auto rhs = ParsePrimary();
    if (!rhs) {
      return nullptr;
    }

    if (curr_precedence < PeekNextToken().GetTokenPrecedence()) {
      rhs = ParseBinOpRHS(curr_precedence + 1, std::move(rhs));
      if (!rhs) {
        return nullptr;
      }
    }

    lhs = std::make_shared<BinaryOpExpression>(std::move(lhs),
                                               std::move(rhs), op);
  }
}

std::shared_ptr<CharConstant> Parser::ParseCharConstant() {
  auto result{MakeASTNode<CharConstant>(GetNextToken().GetCharValue())};
  return result;
}

std::shared_ptr<Int32Constant> Parser::ParseInt32Constant() {
  auto result{MakeASTNode<Int32Constant>(GetNextToken().GetInt32Value())};
  return result;
}

std::shared_ptr<DoubleConstant> Parser::ParseDoubleConstant() {
  auto result{MakeASTNode<DoubleConstant>(GetNextToken().GetDoubleValue())};
  return result;
}

std::shared_ptr<StringLiteral> Parser::ParseStringLiteral() {
  auto result{MakeASTNode<StringLiteral>(GetNextToken().GetStringValue())};
  return result;
}

}

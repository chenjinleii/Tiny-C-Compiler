//
// Created by kaiser on 18-12-8.
//

#include "token.h"

#include <QtCore/QMetaEnum>

#include <iostream>

namespace tcc {

QString TokenTypes::ToString(TokenTypes::Types Type) {
  return QMetaEnum::fromType<TokenTypes::Types>().valueToKey(Type) + 1;
}

PrecedenceDictionary::PrecedenceDictionary() {
  precedence_.insert({TokenTypes::kAssign, 20});
  precedence_.insert({TokenTypes::kSubAssign, 20});
  precedence_.insert({TokenTypes::kAddAssign, 20});
  precedence_.insert({TokenTypes::kShlAssign, 20});
  precedence_.insert({TokenTypes::kShrAssign, 20});
  precedence_.insert({TokenTypes::kModAssign, 20});
  precedence_.insert({TokenTypes::kXorAssign, 20});
  precedence_.insert({TokenTypes::kMulAssign, 20});
  precedence_.insert({TokenTypes::kOrAssign, 20});
  precedence_.insert({TokenTypes::kAndAssign, 20});
  precedence_.insert({TokenTypes::kDivAssign, 20});
  precedence_.insert({TokenTypes::kLogicOr, 40});
  precedence_.insert({TokenTypes::kLogicAnd, 50});
  precedence_.insert({TokenTypes::kOr, 60});
  precedence_.insert({TokenTypes::kXor, 70});
  precedence_.insert({TokenTypes::kAnd, 80});
  precedence_.insert({TokenTypes::kEqual, 90});
  precedence_.insert({TokenTypes::kNotEqual, 90});
  precedence_.insert({TokenTypes::kLessOrEqual, 100});
  precedence_.insert({TokenTypes::kLess, 100});
  precedence_.insert({TokenTypes::kGreaterOrEqual, 100});
  precedence_.insert({TokenTypes::kGreater, 100});
  precedence_.insert({TokenTypes::kShl, 110});
  precedence_.insert({TokenTypes::kShr, 110});
  precedence_.insert({TokenTypes::kSub, 120});
  precedence_.insert({TokenTypes::kAdd, 120});
  precedence_.insert({TokenTypes::kMod, 130});
  precedence_.insert({TokenTypes::kMul, 130});
  precedence_.insert({TokenTypes::kDiv, 130});
  precedence_.insert({TokenTypes::kArrow, 150});
  precedence_.insert({TokenTypes::kPeriod, 150});
}

std::int32_t PrecedenceDictionary::Find(TokenValue value) const {
  if (auto iter{precedence_.find(value)}; iter != std::end(precedence_)) {
    return iter->second;
  } else {
    return -1;
  }
}

Token::Token(const SourceLocation &location, TokenValue value)
    : location_{location}, value_{value} {
  precedence_ = precedence_dictionary_.Find(value);
}

Token::Token(const SourceLocation &location, TokenValue value,
             const std::string &name)
    : location_{location}, value_{value}, name_{name} {}

Token::Token(const SourceLocation &location, char char_value)
    : location_{location},
      value_{TokenTypes::kCharConstant},
      char_value_{char_value} {}

Token::Token(const SourceLocation &location, std::int32_t int32_value)
    : location_{location},
      value_{TokenTypes::kIntConstant},
      int32_value_{int32_value} {}

Token::Token(const SourceLocation &location, double double_value)
    : location_{location},
      value_{TokenTypes::kDoubleConstant},
      double_value_{double_value} {}

Token::Token(const SourceLocation &location, const std::string &string_value)
    : location_{location},
      value_{TokenTypes::kStringLiteral},
      string_value_{string_value} {}

std::string Token::ToString() const {
  auto sp1_count{std::size(location_.ToString())};
  std::string sp1;
  if (sp1_count < 20) {
    sp1.assign(20 - sp1_count, ' ');
  } else {
    sp1.assign(20, ' ');
  }

  auto sp2_count{std::size(TokenTypes::ToString(value_))};
  std::string sp2;
  if (sp2_count < 20) {
    sp2.assign(20 - sp2_count, ' ');
  } else {
    sp2.assign(20, ' ');
  }

  std::string str(location_.ToString() + sp1 +
      "type: " + TokenTypes::ToString(value_).toStdString() + sp2);

  if (IsChar()) {
    str += "value: " + std::to_string(GetCharValue());
  } else if (IsInt32()) {
    str += "value: " + std::to_string(GetInt32Value());
  } else if (IsDouble()) {
    str += "value: " + std::to_string(GetDoubleValue());
  } else if (IsString()) {
    str += "value: " + GetStringValue();
  } else if (IsIdentifier()) {
    str += "name:  " + name_;
  }

  return str;
}

bool Token::IsChar() const { return value_ == TokenValue::kCharConstant; }

bool Token::IsInt32() const { return value_ == TokenValue::kIntConstant; }

bool Token::IsDouble() const { return value_ == TokenValue::kDoubleConstant; }

bool Token::IsString() const { return value_ == TokenValue::kStringLiteral; }

bool Token::IsTypeSpecifier() const {
  return value_ == TokenValue::kChar || value_ == TokenValue::kInt ||
      value_ == TokenValue::kDouble || value_ == TokenValue::kVoid;
}

bool Token::IsIdentifier() const { return value_ == TokenValue::kIdentifier; }

bool Token::TokenValueIs(TokenValue value) const { return value_ == value; }

TokenValue Token::GetTokenValue() const { return value_; }

std::string Token::GetTokenName() const { return name_; }

std::int32_t Token::GetTokenPrecedence() const { return precedence_; }

char Token::GetCharValue() const { return char_value_; }

std::int32_t Token::GetInt32Value() const { return int32_value_; }

double Token::GetDoubleValue() const { return double_value_; }

std::string Token::GetStringValue() const { return string_value_; }

SourceLocation Token::GetTokenLocation() const { return location_; }

void Token::AppendStringValue(const std::string &str) { string_value_ += str; }

}  // namespace tcc

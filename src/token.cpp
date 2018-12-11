//
// Created by kaiser on 18-12-8.
//

#include "token.h"

#include <QMetaEnum>

#include <iostream>

namespace tcc {

std::string TokenValues::ToString(TokenValues::Value value) {
    return QMetaEnum::fromType<TokenValues::Value>().valueToKey(value);
}

PrecedenceDictionary::PrecedenceDictionary() {
    precedence_.insert({TokenValues::kNeg, 0});
    precedence_.insert({TokenValues::kArrow, 150});
    precedence_.insert({TokenValues::kDec, 150});
    precedence_.insert({TokenValues::kSubAssign, 20});
    precedence_.insert({TokenValues::kSub, 120});
    precedence_.insert({TokenValues::kInc, 150});
    precedence_.insert({TokenValues::kAddAssign, 20});
    precedence_.insert({TokenValues::kAdd, 120});
    precedence_.insert({TokenValues::kShlAssign, 20});
    precedence_.insert({TokenValues::kShl, 110});
    precedence_.insert({TokenValues::kLessOrEqual, 100});
    precedence_.insert({TokenValues::kLess, 100});
    precedence_.insert({TokenValues::kShrAssign, 20});
    precedence_.insert({TokenValues::kShr, 110});
    precedence_.insert({TokenValues::kGreaterOrEqual, 100});
    precedence_.insert({TokenValues::kGreater, 100});
    precedence_.insert({TokenValues::kModAssign, 20});
    precedence_.insert({TokenValues::kMod, 130});
    precedence_.insert({TokenValues::kEqual, 90});
    precedence_.insert({TokenValues::kAssign, 20});
    precedence_.insert({TokenValues::kNotEqual, 90});
    precedence_.insert({TokenValues::kLogicNeg, 140});
    precedence_.insert({TokenValues::kLogicAnd, 50});
    precedence_.insert({TokenValues::kAndAssign, 20});
    precedence_.insert({TokenValues::kAnd, 80});
    precedence_.insert({TokenValues::kLogicOr, 40});
    precedence_.insert({TokenValues::kOrAssign, 20});
    precedence_.insert({TokenValues::kOr, 60});
    precedence_.insert({TokenValues::kMulAssign, 20});
    precedence_.insert({TokenValues::kMul, 130});
    precedence_.insert({TokenValues::kDivAssign, 20});
    precedence_.insert({TokenValues::kDiv, 130});
    precedence_.insert({TokenValues::kXorAssign, 20});
    precedence_.insert({TokenValues::kXor, 70});
    precedence_.insert({TokenValues::kPeriod, 150});
}

std::int32_t PrecedenceDictionary::Find(TokenValue value) {
    if (auto iter{precedence_.find(value)};iter != std::end(precedence_)) {
        return iter->second;
    } else {
        return -1;
    }
}

Token::Token(const SourceLocation &location, TokenValue value)
        : location_{location}, value_{value} {
    precedence_ = precedence_dictionary_.Find(value);
}

Token::Token(const SourceLocation &location, TokenValue value, const std::string &name)
        : location_{location}, value_{value}, name_{name} {}

Token::Token(const SourceLocation &location, char char_value)
        : location_{location}, value_{TokenValues::kCharConstant}, char_value_{char_value} {}

Token::Token(const SourceLocation &location, std::int32_t int32_value)
        : location_{location}, value_{TokenValues::kIntConstant}, int32_value_{int32_value} {}

Token::Token(const SourceLocation &location, double double_value)
        : location_{location}, value_{TokenValues::kDoubleConstant}, double_value_{double_value} {}

Token::Token(const SourceLocation &location, const std::string &string_value)
        : location_{location}, value_{TokenValues::kStringLiteral}, string_value_{string_value} {}

std::string Token::ToString() const {
    auto sp1_count{std::size(location_.ToString())};
    std::string sp1;
    if (sp1_count < 20) {
        sp1.assign(20 - sp1_count, ' ');
    } else {
        sp1.assign(20, ' ');
    }

    auto sp2_count{std::size(TokenValues::ToString(value_))};
    std::string sp2;
    if (sp2_count < 20) {
        sp2.assign(20 - sp2_count, ' ');
    } else {
        sp2.assign(20, ' ');
    }

    std::string str(location_.ToString() + sp1 + "type: " + TokenValues::ToString(value_) + sp2);

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

bool Token::IsChar() const {
    return value_ == TokenValue::kCharConstant;
}

bool Token::IsInt32() const {
    return value_ == TokenValue::kIntConstant;
}

bool Token::IsDouble() const {
    return value_ == TokenValue::kDoubleConstant;
}

bool Token::IsString() const {
    return value_ == TokenValue::kStringLiteral;
}

bool Token::IsTypeSpecifier() const {
    return value_ == TokenValue::kCharKeyword ||
            value_ == TokenValue::kIntKeyword ||
            value_ == TokenValue::kDoubleKeyword ||
            value_ == TokenValue::kVoidKeyword;
}

bool Token::IsIdentifier() const {
    return value_ == TokenValue::kIdentifier;
}

bool Token::TokenValueIs(TokenValue value) const {
    return value_ == value;
}

TokenValue Token::GetTokenValue() const {
    return value_;
}

std::string Token::GetTokenName() const {
    return name_;
}

std::int32_t Token::GetTokenPrecedence() const {
    return precedence_;
}

char Token::GetCharValue() const {
    return char_value_;
}

std::int32_t Token::GetInt32Value() const {
    return int32_value_;
}

double Token::GetDoubleValue() const {
    return double_value_;
}

std::string Token::GetStringValue() const {
    return string_value_;
}

}

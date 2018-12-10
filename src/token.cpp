//
// Created by kaiser on 18-12-8.
//

#include "token.h"

#include <QMetaEnum>

#include <iostream>

namespace tcc {

std::string SourceLocation::ToString() const {
    return file_name_ + ":" + std::to_string(row_) + ":" + std::to_string(column_) + ":";
}

std::string TokenValues::ToString(TokenValues::Value value) {
    return QMetaEnum::fromType<Value>().valueToKey(value);
}

Token::Token(const SourceLocation &location, TokenValue value)
        : location_{location}, value_{value} {
    if (auto iter{Precedence.find(value)};iter != std::end(Precedence)) {
        precedence_ = iter->second;
    }
}

Token::Token(const SourceLocation &location, TokenValue value, const std::string &name)
        : location_{location}, value_{value}, name_{name} {}

Token::Token(const SourceLocation &location, char char_value)
        : location_{location}, value_{TokenValues::kCharacter}, char_value_{char_value} {}

Token::Token(const SourceLocation &location, std::int32_t int32_value)
        : location_{location}, value_{TokenValues::kInteger}, int32_value_{int32_value} {}

Token::Token(const SourceLocation &location, double double_value)
        : location_{location}, value_{TokenValues::kDouble}, double_value_{double_value} {}

Token::Token(const SourceLocation &location, const std::string &string_value)
        : location_{location}, value_{TokenValues::kString}, string_value_{string_value} {}

std::string Token::ToString() const {
    std::string str(location_.ToString() + " type " + TokenValues::ToString(value_) + ' ');

    if (IsChar()) {
        str += std::to_string(GetCharValue());
    } else if (IsInt32()) {
        str += std::to_string(GetInt32Value());
    } else if (IsDouble()) {
        str += std::to_string(GetDoubleValue());
    } else if (IsString()) {
        str += GetStringValue();
    } else if (IsIdentifier()) {
        str += name_;
    }

    return str;
}

bool Token::IsChar() const {
    return value_ == TokenValue::kCharacter;
}

bool Token::IsInt32() const {
    return value_ == TokenValue::kInteger;
}

bool Token::IsDouble() const {
    return value_ == TokenValue::kDouble;
}

bool Token::IsString() const {
    return value_ == TokenValue::kString;
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
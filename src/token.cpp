//
// Created by kaiser on 18-12-8.
//

#include "token.h"
#include <iostream>

Token::Token(TokenType type, TokenValue value, const std::string &name) :
        type_{type}, value_{value}, name_{name} {}

Token::Token(TokenType type,
             TokenValue value,
             std::int32_t symbol_precedence,
             const std::string &name) : Token{type, value, name} {
    precedence_ = symbol_precedence;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             bool bool_value) : Token{type, value, name} {
    bool_value_ = bool_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             char char_value) : Token{type, value, name} {
    char_value_ = char_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             unsigned char unsigned_char_value) : Token{type, value, name} {
    unsigned_char_value_ = unsigned_char_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             signed char signed_char_value) : Token{type, value, name} {
    signed_char_value_ = signed_char_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             short short_value) : Token{type, value, name} {
    short_value_ = short_value;
}

Token::Token(TokenType type, TokenValue value,
             const std::string &name, int int_value) : Token{type, value, name} {
    int_value_ = int_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             long long_value) : Token{type, value, name} {
    long_value_ = long_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             long long long_long_value) : Token{type, value, name} {
    long_long_value_ = long_long_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             unsigned short unsigned_short_value) : Token{type, value, name} {
    unsigned_short_value_ = unsigned_short_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             unsigned int unsigned_int_value) : Token{type, value, name} {
    unsigned_int_value_ = unsigned_int_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             unsigned long unsigned_long_value) : Token{type, value, name} {
    unsigned_long_value_ = unsigned_long_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             unsigned long long unsigned_long_long_value) : Token{type, value, name} {
    unsigned_long_long_value_ = unsigned_long_long_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             float float_value) : Token{type, value, name} {
    float_value_ = float_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             double double_value) : Token{type, value, name} {
    double_value_ = double_value;
}

Token::Token(TokenType type,
             TokenValue value,
             const std::string &name,
             const std::string &string_value) : Token{type, value, name} {
    string_value_ = string_value;
}

TokenType Token::GetTokenType() const {
    return type_;
}

TokenValue Token::GetTokenValue() const {
    return value_;
}

std::string Token::GetTokenName() const {
    return name_;
}

std::int32_t Token::GetTokPrecedence() const {
    return precedence_;
}

// TODO 支持更多的类型
bool Token::IsTypeSpecifier() const {
    return value_ == TokenValue::kIntKey ||
            value_ == TokenValue::kDoubleKey;
}

bool Token::IsIdentifier() const {
    return type_ == TokenType::kIdentifier;
}

bool Token::IsOperator() const {
    return type_ == TokenType::kOperator;
}

std::int32_t Token::GetInt32Value() const {
    return int_value_;
}

double Token::GetDoubleValue() const {
    return double_value_;
}
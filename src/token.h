//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_TOKEN_H
#define TINY_C_COMPILER_TOKEN_H

#include <string>
#include <cstdint>
#include <vector>
#include <iostream>

enum class TokenType {
    kBoolean,
    kCharacter,
    KUnsignedCharacter,
    KSignedCharacter,

    kShortInterger,
    kInterger,
    kLongInterger,
    kLongLongInterger,

    kUnsignedShortInterger,
    kUnsignedInterger,
    kUnsignedLongInterger,
    kUnsignedLongLongInterger,

    kFolat,
    kDouble,

    kString,

    kIdentifier,
    kKeyword,
    kOperator,
    kDelimiter,
    kEof,

    kUnknown
};

enum class TokenValue {
    kAutoKey,
    kBreakKey,
    kCaseKey,
    kCharKey,
    kConstKey,
    kContinueKey,
    kDefaultKey,
    kDoKey,
    kDoubleKey,
    kElseKey,
    kEnumKey,
    kExternKey,
    kFloatKey,
    kForKey,
    kGotoKey,
    kIfKey,
    kInlineKey,
    kIntKey,
    kLongKey,
    kRegisterKey,
    kRestrictKey,
    kReturnKey,
    kShortKey,
    kSignedKey,
    kSizeofKey,
    kStaticKey,
    kStructKey,
    kSwitchKey,
    kTypedefKey,
    kUnionKey,
    kUnsignedKey,
    kVoidKey,
    kVolatileKey,
    kWhileKey,
    kBoolKey,
    kComplexKey,
    kImaginaryKey,

    kAssign,           // =

    kInc,              // ++
    kDec,              // --

    kPlus,             // +
    kMinus,            // -
    kMultiply,         // *
    kDivide,           // /
    kMod,              // %
    kNeg,              // ~
    kAnd,              // &
    kOr,               // |
    kXor,              // ^
    kShl,              // <<
    kShr,              // >>

    kLogicNeg,         // !
    kLogicAnd,         // &&
    kLogicOr,          // ||

    kEqual,            // ==
    kNotEqual,         // !=
    kLess,             // <
    kGreater,          // >
    kLessOrEqual,      // <=
    kGreaterOrEqual,   // >=

    kArrow,            // ->
    kPeriod,           // .

    kComma,            // ,

    kLeftParen,        // (
    kRightParen,       // )
    kLeftSquare,       // [
    kRightSquare,      // ]
    kLeftCurly,        // {
    kRightCurly,       // }
    kSemicolon,        // ;

    kIdentifier,
    kUnreserved
};

class Token {
public:
    Token() = default;
    Token(TokenType type, TokenValue value, const std::string &name);
    Token(TokenType type,
          TokenValue value,
          std::int32_t symbol_precedence,
          const std::string &name);

    Token(TokenType type, TokenValue value, const std::string &name, bool bool_value);
    Token(TokenType type, TokenValue value, const std::string &name, char char_value);
    Token(TokenType type,
          TokenValue value,
          const std::string &name,
          unsigned char unsigned_char_value);
    Token(TokenType type,
          TokenValue value,
          const std::string &name,
          signed char signed_char_value);

    Token(TokenType type, TokenValue value, const std::string &name, short short_value);
    Token(TokenType type, TokenValue value, const std::string &name, int int_value);
    Token(TokenType type, TokenValue value, const std::string &name, long long_value);
    Token(TokenType type,
          TokenValue value,
          const std::string &name,
          long long long_long_value);

    Token(TokenType type,
          TokenValue value,
          const std::string &name,
          unsigned short unsigned_short_value);
    Token(TokenType type,
          TokenValue value,
          const std::string &name,
          unsigned int unsigned_int_value);
    Token(TokenType type,
          TokenValue value,
          const std::string &name,
          unsigned long unsigned_long_value);
    Token(TokenType type, TokenValue value,
          const std::string &name,
          unsigned long long unsigned_long_long_value);

    Token(TokenType type, TokenValue value, const std::string &name, float float_value);
    Token(TokenType type,
          TokenValue value,
          const std::string &name,
          double double_value);

    Token(TokenType type,
          TokenValue value,
          const std::string &name,
          const std::string &string_value);

    TokenType GetTokenType() const;
    TokenValue GetTokenValue() const;
    std::string GetTokenName() const;
    std::int32_t GetTokPrecedence() const;

    bool IsTypeSpecifier() const;
    bool IsIdentifier() const;
    bool IsOperator() const;

    std::int32_t GetInt32Value() const;
    double GetDoubleValue() const;
private:
    TokenType type_{TokenType::kUnknown};
    TokenValue value_{TokenValue::kUnreserved};
    std::string name_;
    std::int32_t precedence_{};

    bool bool_value_{};
    char char_value_{};
    unsigned char unsigned_char_value_{};
    signed char signed_char_value_{};

    short short_value_{};
    int int_value_{};
    long long_value_{};
    long long long_long_value_{};

    unsigned short unsigned_short_value_{};
    unsigned int unsigned_int_value_{};
    unsigned long unsigned_long_value_{};
    unsigned long long unsigned_long_long_value_{};

    float float_value_{};
    double double_value_{};

    std::string string_value_;
};

#endif //TINY_C_COMPILER_TOKEN_H
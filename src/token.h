//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_TOKEN_H
#define TINY_C_COMPILER_TOKEN_H

#include "location.h"

#include <QObject>

#include <string>
#include <cstdint>
#include <unordered_map>

namespace tcc {

class TokenValues : public QObject {
Q_OBJECT
public:
    enum Value {
        kCharConstant,
        kIntConstant,
        kDoubleConstant,
        kStringLiteral,

        kIdentifier,

        kAutoKeyword,
        kBreakKeyword,
        kCaseKeyword,
        kCharKeyword,
        kConstKeyword,
        kContinueKeyword,
        kDefaultKeyword,
        kDoKeyword,
        kDoubleKeyword,
        kElseKeyword,
        kEnumKeyword,
        kExternKeyword,
        kFloatKeyword,
        kForKeyword,
        kGotoKeyword,
        kIfKeyword,
        kInlineKeyword,
        kIntKeyword,
        kLongKeyword,
        kRegisterKeyword,
        kRestrictKeyword,
        kReturnKeyword,
        kShortKeyword,
        kSignedKeyword,
        kSizeofKeyword,
        kStaticKeyword,
        kStructKeyword,
        kSwitchKeyword,
        kTypedefKeyword,
        kUnionKeyword,
        kUnsignedKeyword,
        kVoidKeyword,
        kVolatileKeyword,
        kWhileKeyword,
        kBoolKeyword,
        kComplexKeyword,
        kImaginaryKeyword,

        kAssign,           // =
        kAddAssign,        // +=
        kSubAssign,        // -=
        kMulAssign,        // *=
        kDivAssign,        // /=
        kModAssign,        // %=
        kAndAssign,        // &=
        kOrAssign,         // |=
        kXorAssign,        // ^=
        kShlAssign,        // <<=
        kShrAssign,        // >>=

        kInc,              // ++
        kDec,              // --

        kAdd,              // +
        kSub,              // -
        kMul,              // *
        kDiv,              // /
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

        kQuestionMark,     // ?
        kComma,            // ,

        kLeftParen,        // (
        kRightParen,       // )
        kLeftSquare,       // [
        kRightSquare,      // ]
        kLeftCurly,        // {
        kRightCurly,       // }
        kSemicolon,        // ;
        KColon,            // :
        kEllipsis,         // ...

        kNone,
        kEof
    };
    Q_ENUM(Value)

    static std::string ToString(Value value);
};

using TokenValue=TokenValues::Value;

class PrecedenceDictionary {
public:
    PrecedenceDictionary();
    std::int32_t Find(TokenValue value) const;
private:
    std::unordered_map<TokenValue, std::int32_t> precedence_;
};

// TODO 更多内置类型
class Token {
public:
    Token() = default;
    Token(const SourceLocation &location, TokenValue value);
    Token(const SourceLocation &location, TokenValue value, const std::string &name);
    Token(const SourceLocation &location, char char_value);
    Token(const SourceLocation &location, std::int32_t int32_value);
    Token(const SourceLocation &location, double double_value);
    Token(const SourceLocation &location, const std::string &string_value);

    std::string ToString() const;

    bool IsChar() const;
    bool IsInt32() const;
    bool IsDouble() const;
    bool IsString() const;

    bool IsTypeSpecifier() const;
    bool IsIdentifier() const;
    bool IsOperator() const;
    bool IsPrefixOperator() const;
    bool IsPostfixOperator() const;

    bool TokenValueIs(TokenValue value) const;
    SourceLocation GetTokenLocation() const;
    TokenValue GetTokenValue() const;
    std::string GetTokenName() const;
    std::int32_t GetTokenPrecedence() const;

    char GetCharValue() const;
    std::int32_t GetInt32Value() const;
    double GetDoubleValue() const;
    std::string GetStringValue() const;
private:
    SourceLocation location_;
    TokenValue value_{TokenValue::kNone};
    std::string name_;
    std::int32_t precedence_{-1};

    char char_value_{};
    std::int32_t int32_value_{};
    double double_value_{};
    std::string string_value_;
    PrecedenceDictionary precedence_dictionary_;
};

}

#endif //TINY_C_COMPILER_TOKEN_H

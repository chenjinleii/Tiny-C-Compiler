//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_TOKEN_H
#define TINY_C_COMPILER_TOKEN_H

#include <string>
#include <cstdint>

namespace tcc {

// TODO 更多内置类型
enum class TokenValue {
    kCharacter,
    kInteger,
    kDouble,
    kString,

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

    kEllipsis,         //...

    kNone,
    kEof
};

class SourceLocation {
public:
    SourceLocation() = default;
    SourceLocation(const std::string &file_name, std::int32_t line, std::int32_t column);
    std::string ToString() const;

    std::string file_name_;
    std::int32_t line_{1};
    std::int32_t column_{};
};

class Token {
public:
    Token() = default;
    Token(const SourceLocation &location, TokenValue value, const std::string &name);
    Token(const SourceLocation &location, TokenValue value, std::int32_t precedence, const std::string &name);
    Token(const SourceLocation &location, TokenValue value, const std::string &name, char char_value);
    Token(const SourceLocation &location, TokenValue value, const std::string &name, int int32_value);
    Token(const SourceLocation &location, TokenValue value, const std::string &name, double double_value);
    Token(const SourceLocation &location, TokenValue value, const std::string &name, const std::string &string_value);

    bool IsChar() const;
    bool IsInt32() const;
    bool IsDouble() const;
    bool IsString() const;

    bool IsTypeSpecifier() const;
    bool IsIdentifier() const;

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
};

}

#endif //TINY_C_COMPILER_TOKEN_H
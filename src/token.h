//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_TOKEN_H
#define TINY_C_COMPILER_TOKEN_H

#include <QObject>

#include <string>
#include <cstdint>
#include <unordered_map>

namespace tcc {

class SourceLocation {
public:
    std::string ToString() const;

    std::string file_name_;
    std::int32_t row_{1};
    std::int32_t column_{};
};

class TokenValues : public QObject {
Q_OBJECT
public:
    enum Value {
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
    Q_ENUM(Value)

    static std::string ToString(Value value);
};

using TokenValue=TokenValues::Value;

inline std::unordered_map<TokenValue, std::int32_t> Precedence{
        {TokenValues::kNeg, 0},
        {TokenValues::kArrow, 150},
        {TokenValues::kDec, 150},
        {TokenValues::kSubAssign, 20},
        {TokenValues::kSub, 120},
        {TokenValues::kInc, 150},
        {TokenValues::kAddAssign, 20},
        {TokenValues::kAdd, 120},
        {TokenValues::kShlAssign, 20},
        {TokenValues::kShl, 110},
        {TokenValues::kLessOrEqual, 100},
        {TokenValues::kLess, 100},
        {TokenValues::kShrAssign, 20},
        {TokenValues::kShr, 110},
        {TokenValues::kGreaterOrEqual, 100},
        {TokenValues::kGreater, 100},
        {TokenValues::kModAssign, 20},
        {TokenValues::kMod, 130},
        {TokenValues::kEqual, 90},
        {TokenValues::kAssign, 20},
        {TokenValues::kNotEqual, 90},
        {TokenValues::kLogicNeg, 140},
        {TokenValues::kLogicAnd, 50},
        {TokenValues::kAndAssign, 20},
        {TokenValues::kAnd, 80},
        {TokenValues::kLogicOr, 40},
        {TokenValues::kOrAssign, 20},
        {TokenValues::kOr, 60},
        {TokenValues::kMulAssign, 20},
        {TokenValues::kMul, 130},
        {TokenValues::kDivAssign, 20},
        {TokenValues::kDiv, 130},
        {TokenValues::kXorAssign, 20},
        {TokenValues::kXor, 70},
        {TokenValues::kPeriod, 150}
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